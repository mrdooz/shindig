#include "stdafx.h"
#include "obj_loader.hpp"
#include "mesh2.hpp"
#include "geometry.hpp"
#include "material.hpp"

void ObjLoader::handle_face_transition(bool *reading_face_data, Groups *groups, Group **cur_group, int running_face_idx, int running_vert_idx, int running_normal_idx, int running_tex_idx)
{
	if (*reading_face_data) {
		*reading_face_data = false;
		groups->push_back(*cur_group);

		Group *g = *cur_group = new Group("default");
		g->face_ofs = running_face_idx;
		g->vert_ofs = running_vert_idx;
		g->normal_ofs = running_normal_idx;
		g->tex_ofs = running_tex_idx;
		g->verts.reserve(10000);
		g->normals.reserve(10000);
		g->tex_coords.reserve(10000);
		g->faces.reserve(10000);
	}
}

bool ObjLoader::load_binary_file(const char *filename, Geometries *geometries)
{
	RefPtr<FileReader> file(new FileReader());
	if (!file->load(filename))
		return false;
	DataReader r(file.get());
	BinaryHeader h;
	r.read(&h);

	for (int i = 0; i < h.mesh_count; ++i) {

		Geometry *g = new Geometry();
		Mesh2 *m = new Mesh2();
		std::vector<uint8_t> verts, indices;
		r.read_vector(&verts);
		r.read_vector(&indices);
		r.read(&g->_sphere.radius);
		r.read(&g->_sphere.center);
		int vtx_flags;
		r.read(&vtx_flags);
		r.read(&m->_ib_format);
		r.read(&m->_stride);
		r.read(&m->_vertex_count);
		r.read(&m->_vertex_size);
		r.read(&m->_index_count);
		r.read_string(&m->_material_name);

		// create buffers and desc
		if (vtx_flags & VtxPos) m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
		if (vtx_flags & VtxNormal) m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
		if (vtx_flags & VtxTex) m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24));

		auto d = Graphics::instance().device();
		RETURN_ON_FAIL_BOOL_E(create_static_vertex_buffer(d, verts, &m->_vb));
		RETURN_ON_FAIL_BOOL_E(create_static_index_buffer(d, indices.size() / sizeof(int), sizeof(int), (const uint8_t *)&indices[0], &m->_ib));

		g->_mesh.reset(m);
		geometries->push_back(g);
	}

	return true;
}

void ObjLoader::calc_bounding_sphere(const Verts& verts, float *radius, D3DXVECTOR3 *center)
{
	// calc a bounding box from the verts
	D3DXVECTOR3 min_v(verts[0]), max_v(verts[0]);
	for (int i = 1; i < (int)verts.size(); ++i) {
		D3DXVec3Minimize(&min_v, &min_v, &verts[i]);
		D3DXVec3Maximize(&max_v, &max_v, &verts[i]);
	}

	const D3DXVECTOR3 c(0.5f * (min_v + max_v));

	// float max distance from center
	float dist = D3DXVec3LengthSq(&(verts[0] - c));
	for (int i = 1; i < (int)verts.size(); ++i) 
		dist = std::max<float>(dist, D3DXVec3LengthSq(&(verts[i] - c)));

	*radius = sqrtf(dist);
	*center = c;
}

bool ObjLoader::load_from_file(const char *filename, Geometries *geometries)
{
	string2 binary_name(filename);
	binary_name += ".bin";

	if (load_binary_file(binary_name, geometries))
		return true;

	Groups groups;
	if (!parse_file(filename, &groups)) {
		LOG_WARNING_LN("Error parsing file: %s", filename);
		return false;
		}

	// write binary header
	FileWriter w;
	w.open(binary_name);
	BinaryHeader h;
	strcpy(h.filename, filename);
	get_file_size(filename, &h.textfile_size, NULL);
	get_file_time(filename, NULL, NULL, &h.textfile_write_time);
	h.mesh_count = (int)groups.size();
	w.write(h);


	for (int i = 0; i < (int)groups.size(); ++i) {
		Group *cur = groups[i];

		const Verts& verts = cur->verts;
		const Faces& faces = cur->faces;
		const VertsByFace& verts_by_face = cur->verts_by_face;

		D3DXVECTOR3 center;
		float radius;
		calc_bounding_sphere(verts, &radius, &center);

		std::vector<D3DXVECTOR3> face_normals;
		std::vector<D3DXVECTOR3> vertex_normals;

		// if we have vertex normals in the face data, we can skip these
		// calculations, but we have to create new vertices instead
		if (faces[0].na != -1) {
			const bool has_uvs = faces[0].ta != -1;
			std::vector<PosNormalTex> new_verts;
			std::vector<int32_t> new_indices;
			typedef std::map<int64_t, int32_t> VertRemap;
			VertRemap vert_remap;	// maps the vertex id to an index in the new_vertices list
			// we use a 64 bit index to identify the verts, so we can max have 2^21 verts
			if (verts.size() > 1 << 21)
				return false;
			new_verts.reserve(verts.size());
			new_indices.reserve(faces.size());
			for (int i = 0; i < (int)faces.size(); ++i) {
				for (int j = 0; j < 3; ++j) {
					int32_t t = faces[i].t[j] == -1 ? 0 : faces[i].t[j];
					int32_t n = faces[i].n[j] == -1 ? 0 : faces[i].n[j];
					int32_t v = faces[i].v[j] == -1 ? 0 : faces[i].v[j];
					uint64_t id = ((uint64_t)t << 21*2) + ((uint64_t)n << 21*1) + ((uint64_t)v << 21*0);

					int32_t idx = 0;
					VertRemap::iterator it = vert_remap.find(id);
					if (it == vert_remap.end()) {
						// insert new vertex
						idx = new_verts.size();
						new_verts.push_back(PosNormalTex(cur->verts[v], cur->normals[n], has_uvs ? cur->tex_coords[t] : D3DXVECTOR2(0,0)));
						vert_remap.insert(std::make_pair(id, idx));
					} else {
						idx = it->second;
					}
					new_indices.push_back(idx);
				}
			}

			Geometry *g = new Geometry();
			Mesh2 *m = new Mesh2();
			g->_mesh.reset(m);
			auto d = Graphics::instance().device();
			RETURN_ON_FAIL_BOOL_E(create_static_vertex_buffer(d, new_verts.size(), sizeof(PosNormalTex), (const uint8_t *)&new_verts[0], &m->_vb));
			RETURN_ON_FAIL_BOOL_E(create_static_index_buffer(d, new_indices.size(), sizeof(int), (const uint8_t *)&new_indices[0], &m->_ib));

			g->_sphere.radius = radius;
			g->_sphere.center = center;
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24));
			m->_ib_format = DXGI_FORMAT_R32_UINT;
			m->_stride = sizeof(PosNormalTex);
			m->_vertex_count = new_verts.size();
			m->_vertex_size = sizeof(PosNormalTex);
			m->_index_count = new_indices.size();
      m->_material_name = cur->material_name;

			geometries->push_back(g);

			// save mesh
			w.write_vector(new_verts);
			w.write_vector(new_indices);
			w.write(g->_sphere.radius);
			w.write(g->_sphere.center);
			w.write(VtxPos | (faces[0].na != -1 ? VtxNormal : 0) | (faces[0].ta != -1 ? VtxTex : 0));
			w.write(m->_ib_format);
			w.write(m->_stride);
			w.write(m->_vertex_count);
			w.write(m->_vertex_size);
			w.write(m->_index_count);
			w.write_string(m->_material_name);
			 
 		} else {
			// calc face normals
			face_normals.reserve(faces.size());
			const int v_ofs = cur->vert_ofs;
			for (auto i = faces.begin(), e = faces.end(); i != e; ++i) {
				const D3DXVECTOR3& v0 = verts[i->va];
				const D3DXVECTOR3& v1 = verts[i->vb];
				const D3DXVECTOR3& v2 = verts[i->vc];
				face_normals.push_back(vec3_normalize(vec3_cross(v1 - v0, v2 - v0)));
			}

			// calc vertex normals
			vertex_normals.reserve(verts.size());
			const int f_ofs = cur->face_ofs;
			for (int i = 0; i < (int)verts.size(); ++i) {
				D3DXVECTOR3 n(0,0,0);
				auto it = verts_by_face.find(i);
				if (it != verts_by_face.end()) {
					for (auto j = it->second.begin(); j != it->second.end(); ++j) {
						n += face_normals[*j];
					}
					vertex_normals.push_back(vec3_normalize(n/(float)it->second.size()));
				} else {
					// the vertex isn't used by any face, so we just add a dummy normal
					vertex_normals.push_back(D3DXVECTOR3(0,0,0));
				}
			}

			// create interleaved vertex data
			D3DXVECTOR3 *interleaved = new D3DXVECTOR3[2 * verts.size()];
			SCOPED_OBJ([=](){ delete [] interleaved; } );
			for (int i = 0; i < (int)verts.size(); ++i) {
				interleaved[i*2+0] = verts[i];
				interleaved[i*2+1] = vertex_normals[i];
			}

			Geometry *g = new Geometry();
			Mesh2 *m = new Mesh2();
			g->_mesh.reset(m);
			auto d = Graphics::instance().device();
			if (FAILED(create_static_vertex_buffer(d, verts.size(), 2 * sizeof(D3DXVECTOR3), (const uint8_t *)interleaved, &m->_vb)))
				return false;

			if (FAILED(create_static_index_buffer(d, faces.size() * 3, sizeof(int), (const uint8_t *)&faces[0], &m->_ib)))
				return false;

			g->_sphere.radius = radius;
			g->_sphere.center = center;
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
			m->_ib_format = DXGI_FORMAT_R32_UINT;
			m->_stride = 2 * sizeof(D3DXVECTOR3);
			m->_vertex_count = verts.size();
			m->_vertex_size = 2 * sizeof(D3DXVECTOR3);
			m->_index_count = faces.size() * 3;

			geometries->push_back(g);

			// save mesh
			w.write_raw(interleaved, verts.size() * 2 * sizeof(D3DXVECTOR3));
			w.write_vector(faces);
			w.write(g->_sphere.radius);
			w.write(g->_sphere.center);
			w.write(VtxPos | VtxNormal);
			w.write(m->_ib_format);
			w.write(m->_stride);
			w.write(m->_vertex_count);
			w.write(m->_vertex_size);
			w.write(m->_index_count);
			w.write_string(m->_material_name);


		}

	}

  return true;
}

bool ObjLoader::load_material_file(const char *filename, std::vector<Material *> *materials)
{
	TextScanner scanner;
	if (!scanner.load(filename))
		return false;

	Material *cur = nullptr;

	static string2 float_values[] = { "Ns", "Ni", "d", "Tr" };
	static string2 float3_values[] = { "Tf", "Ka", "Kd", "Ks", "Ke" };
	static string2 string_values[] = { "map_Ka", "map_Kd", "map_d", "map_bump", "bump" };

	while (true) {

		if (!scanner.skip_chars_lenient(" \t"))
			break;
		string2 s;
		if (!scanner.read_string(&s)) 
			break;

		if (s == "newmtl") {
			string2 name;
			if (!scanner.read_string(&name)) 
				break;
			// new material found. store old one first, if it exists
			if (cur)
				materials->push_back(cur);
			cur = new Material(name);
		} else {
			bool found = false;
			// float values
			float f;
			for (int i = 0; !found && i < ELEMS_IN_ARRAY(float_values); ++i) {
				if (s == float_values[i]) {
					if (!scanner.read_float(&f))
						break;
					cur->float_values.insert(std::make_pair(s, f));
					found = true;
				}
			}

			// float3
			std::vector<float> floats;
			for (int i = 0; !found && i < ELEMS_IN_ARRAY(float3_values); ++i) {
				if (s == float3_values[i]) {
					if (!scanner.read_floats(&floats))
						break;
					if (floats.size() == 3) {
						cur->float3_values.insert(std::make_pair(s, D3DXVECTOR3(floats[0], floats[1], floats[2])));
						found = true;
					}
				}
			}

			// string
			for (int i = 0; !found && i < ELEMS_IN_ARRAY(string_values); ++i) {
				if (s == string_values[i]) {
					string2 val;
					if (!scanner.read_string(&val))
						break;
					cur->string_values.insert(std::make_pair(s, val));
					found = true;
				}
			}

		}

		if (!scanner.skip_to_next_line())
			break;
	}

	if (cur)
		materials->push_back(cur);

	return true;
}

bool ObjLoader::parse_file(const char *filename, Groups *groups)
{
/*
	  v 0.000000 2.000000 2.000000
		v 0.000000 0.000000 2.000000
		v 2.000000 0.000000 2.000000
		v 2.000000 2.000000 2.000000
		v 0.000000 2.000000 0.000000
		v 0.000000 0.000000 0.000000
		v 2.000000 0.000000 0.000000
		v 2.000000 2.000000 0.000000
		f 1 2 3 4
		f 8 7 6 5
		f 4 3 7 8
		f 5 1 4 8
		f 5 6 2 1
		f 2 6 7 3

		  5--8
		 /  /
		1--4
		|/ |/
		2--3

*/


  TextScanner scanner;
  if (!scanner.load(filename)) {
		LOG_WARNING_LN("Error loading file in text scanner: %s", filename);
		return false;
	}

	int running_vert_idx = 0, running_normal_idx = 0, running_tex_idx = 0;
	int running_face_idx = 0;

	// The order of data in the file is:
	// vertex data | normal data | texture data | usemtl
	// face data
	// a new mesh is created at start-up, and then at each transition between face->something else

	string2 s;
	bool reading_face_data = false;
	Group *cur_group = new Group("default");
	groups->push_back(cur_group);
  while (true) {

		if (!scanner.read_string(&s))
			break;

		if (s == "vn") {
			handle_face_transition(&reading_face_data, groups, &cur_group, running_face_idx, running_vert_idx, running_normal_idx, running_tex_idx);

			std::vector<float> f;
			scanner.read_floats(&f);
			if (f.size() != 3)
				return false;
			cur_group->normals.push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
			running_normal_idx++;

		} else if (s == "vt") {
			handle_face_transition(&reading_face_data, groups, &cur_group, running_face_idx, running_vert_idx, running_normal_idx, running_tex_idx);
			std::vector<float> t;
			scanner.read_floats(&t);
			if (t.size() != 3)
				return false;
			cur_group->tex_coords.push_back(D3DXVECTOR2(t[0], t[1]));
			running_tex_idx++;

		} else if (s == "g") {
			handle_face_transition(&reading_face_data, groups, &cur_group, running_face_idx, running_vert_idx, running_normal_idx, running_tex_idx);
			scanner.read_string(&cur_group->name);

			if (!scanner.skip_to_next_line())
				break;
		} else if (s == "s") {

			// new smoothing group
			if (!scanner.skip_to_next_line())
				break;

		} else if (s == "v") {
			handle_face_transition(&reading_face_data, groups, &cur_group, running_face_idx, running_vert_idx, running_normal_idx, running_tex_idx);

			std::vector<float> f;
			scanner.read_floats(&f);
			if (f.size() != 3)
				return false;
			cur_group->verts.push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
			running_vert_idx++;

		} else if (s == "f") {
			reading_face_data = true;

			// faces can have different formats
			// f v1/vt1
			// f v1/vt1/vn1
			// f v1//vn1
			std::vector<int> tmp;
			std::vector<int> v;
			std::vector<int> t;
			std::vector<int> n;
			int t2;
			while (scanner.read_ints(&tmp)) {
				if (tmp.size() == 1) {
					// face isn't just a list of vertex, but texcoord and normals too, so we have to parse
					// these guys
					v.push_back(tmp[0]);
					char buf[2];
					if (!scanner.peek(buf, 2)) break;
					if (!scanner.skip_chars("/")) break;
					if (buf[0] == '/') {
						if (buf[1] == '/') {
							// v1//vn1
							if (!scanner.read_int(&t2)) break;
							n.push_back(t2);
						} else {
							// vt1
							if (!scanner.read_int(&t2)) break;
							t.push_back(t2);
							char ch;
							if (!scanner.peek(&ch)) break;
							if (!scanner.skip_chars("/")) break;
							if (ch == '/') {
								// vn1
								if (!scanner.read_int(&t2)) break;
								n.push_back(t2);
							}
						}
					}

				} else {
					// face is a nice list of space seperated indices
					v = tmp;
					break;
				}
			}

			switch( v.size() ) {
			case 3:
				{
					int 
						iv1 = v[0] - 1 - cur_group->vert_ofs, 
						iv2 = v[1] - 1 - cur_group->vert_ofs, 
						iv3 = v[2] - 1 - cur_group->vert_ofs,
						in1 = n.empty() ? -1 : n[0] - 1 - cur_group->normal_ofs,
						in2 = n.empty() ? -1 : n[1] - 1 - cur_group->normal_ofs,
						in3 = n.empty() ? -1 : n[2] - 1 - cur_group->normal_ofs,
						it1 = t.empty() ? -1 : t[0] - 1 - cur_group->tex_ofs,
						it2 = t.empty() ? -1 : t[1] - 1 - cur_group->tex_ofs,
						it3 = t.empty() ? -1 : t[2] - 1 - cur_group->tex_ofs;
					// 2, 1, 3
					cur_group->faces.push_back(Face(iv2, iv1, iv3, in2, in1, in3, it2, it1, it3));
					cur_group->verts_by_face[iv1].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv2].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv3].push_back(cur_group->face_count);
					cur_group->face_count++;
					running_face_idx++;
				}
				break;
			case 4:
				{
					int 
						iv1 = v[0] - 1 - cur_group->vert_ofs, 
						iv2 = v[1] - 1 - cur_group->vert_ofs, 
						iv3 = v[2] - 1 - cur_group->vert_ofs,
						iv4 = v[3] - 1 - cur_group->vert_ofs,
						in1 = n.empty() ? -1 : n[0] - 1 - cur_group->normal_ofs,
						in2 = n.empty() ? -1 : n[1] - 1 - cur_group->normal_ofs,
						in3 = n.empty() ? -1 : n[2] - 1 - cur_group->normal_ofs,
						in4 = n.empty() ? -1 : n[3] - 1 - cur_group->normal_ofs,
						it1 = t.empty() ? -1 : t[0] - 1 - cur_group->tex_ofs,
						it2 = t.empty() ? -1 : t[1] - 1 - cur_group->tex_ofs,
						it3 = t.empty() ? -1 : t[2] - 1 - cur_group->tex_ofs,
						it4 = t.empty() ? -1 : t[3] - 1 - cur_group->tex_ofs;
					// 1--4
					// |  |
					// 2--3
					// 2, 1, 3
					// 3, 1, 4
					cur_group->faces.push_back(Face(iv2, iv1, iv3, in2, in1, in3, it2, it1, it3));
					cur_group->verts_by_face[iv2].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv1].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv3].push_back(cur_group->face_count);
					cur_group->face_count++;
					running_face_idx++;

					cur_group->faces.push_back(Face(iv3, iv1, iv4, in3, in1, in4, it3, it1, it4));
					cur_group->verts_by_face[iv3].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv1].push_back(cur_group->face_count);
					cur_group->verts_by_face[iv4].push_back(cur_group->face_count);
					cur_group->face_count++;
					running_face_idx++;

				}
				break;
			default:
				return false;
			}

			if (!scanner.skip_to_next_line())
				break;

		} else if (s == "usemtl") {
			scanner.read_string(&cur_group->material_name);

			if (!scanner.skip_to_next_line())
				break;

		} else {
			scanner.skip_to_next_line();
		}
  }

  return true;
}
