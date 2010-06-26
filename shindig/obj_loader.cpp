#include "stdafx.h"
#include "obj_loader.hpp"
#include "celsus/text_scanner.hpp"
#include "celsus/math_utils.hpp"
#include "celsus/file_utils.hpp"
#include <celsus/refptr.hpp>
#include <celsus/vertex_types.hpp>
#include "mesh2.hpp"
#include "material.hpp"

bool ObjLoader::save_binary_file(const char *filename, const Groups& groups)
{
	BinaryHeader h;
	strcpy(h.filename, filename);
	get_file_size(filename, &h.textfile_size, NULL);
	get_file_time(filename, NULL, NULL, &h.textfile_write_time);

	string2 binary_file(filename);
	binary_file += ".bin";

	FileWriter w;
	if (!w.open(binary_file))
		return false;

	w.write(h);
	w.write(groups.size());

	for (int i = 0; i < (int)groups.size(); ++i) {
		Group *g = groups[i];
		w.write_string(g->name);
		w.write_string(g->material_name);
		w.write_vector(g->verts);
		w.write_vector(g->faces);
		w.write(g->vert_ofs);
		w.write(g->face_ofs);
		w.write(g->face_count);
		// verts by face
		w.write(g->verts_by_face.size());
		for (VertsByFace::iterator it = g->verts_by_face.begin(); it != g->verts_by_face.end(); ++it) {
			w.write(it->first);
			w.write_vector(it->second);
		}
	}

	return true;
}
/*
bool ObjLoader::load_binary_file(const char *filename, Groups* groups)
{
	RefPtr<FileReader> file(new FileReader());
	if (!file->load(filename))
		return false;
	DataReader r(file.get());
	BinaryHeader h;
	r.read(&h);

	int num_groups = 0;
	r.read(&num_groups);
	for (int i = 0; i < num_groups; ++i) {
		string2 name;
		r.read_string(&name);
		Group *g = new Group(name);
		r.read_string(&g->material_name);
		r.read_vector(&g->verts);
		r.read_vector(&g->faces);
		r.read(&g->vert_ofs);
		r.read(&g->face_ofs);
		r.read(&g->face_count);
		int c;
		r.read(&c);
		for (int j = 0; j < c; ++j) {
			int e;
			r.read(&e);
			r.read_vector(&g->verts_by_face[e]);
		}

		groups->push_back(g);
	}

	return true;
}
*/

bool ObjLoader::load_binary_file(const char *filename, Meshes *meshes)
{

	RefPtr<FileReader> file(new FileReader());
	if (!file->load(filename))
		return false;
	DataReader r(file.get());
	BinaryHeader h;
	r.read(&h);

	for (int i = 0; i < h.mesh_count; ++i) {

		Mesh2 *m = new Mesh2();
		std::vector<uint8_t> verts, indices;
		r.read_vector(&verts);
		r.read_vector(&indices);
		r.read(&m->_bounding_radius);
		r.read(&m->_bounding_center);
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

		meshes->push_back(m);
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

bool ObjLoader::load_from_file(const char *filename, Meshes *meshes)
{

	string2 binary_name(filename);
	binary_name += ".bin";

	if (load_binary_file(binary_name, meshes))
		return true;

	Groups groups;
	if (!parse_file(filename, &groups))
		return false;

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

			Mesh2 *m = new Mesh2();
			auto d = Graphics::instance().device();
			RETURN_ON_FAIL_BOOL_E(create_static_vertex_buffer(d, new_verts.size(), sizeof(PosNormalTex), (const uint8_t *)&new_verts[0], &m->_vb));
			RETURN_ON_FAIL_BOOL_E(create_static_index_buffer(d, new_indices.size(), sizeof(int), (const uint8_t *)&new_indices[0], &m->_ib));

			m->_bounding_radius = radius;
			m->_bounding_center = center;
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24));
			m->_ib_format = DXGI_FORMAT_R32_UINT;
			m->_stride = sizeof(PosNormalTex);
			m->_vertex_count = new_verts.size();
			m->_vertex_size = sizeof(PosNormalTex);
			m->_index_count = new_indices.size();

			meshes->push_back(m);

			// save mesh
			w.write_vector(new_verts);
			w.write_vector(new_indices);
			w.write(m->_bounding_radius);
			w.write(m->_bounding_center);
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

			Mesh2 *m = new Mesh2();
			auto d = Graphics::instance().device();
			if (FAILED(create_static_vertex_buffer(d, verts.size(), 2 * sizeof(D3DXVECTOR3), (const uint8_t *)interleaved, &m->_vb)))
				return false;

			if (FAILED(create_static_index_buffer(d, faces.size() * 3, sizeof(int), (const uint8_t *)&faces[0], &m->_ib)))
				return false;

			m->_bounding_radius = radius;
			m->_bounding_center = center;
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
			m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
			m->_ib_format = DXGI_FORMAT_R32_UINT;
			m->_stride = 2 * sizeof(D3DXVECTOR3);
			m->_vertex_count = verts.size();
			m->_vertex_size = 2 * sizeof(D3DXVECTOR3);
			m->_index_count = faces.size() * 3;

			meshes->push_back(m);

			// save mesh
			w.write_raw(interleaved, verts.size() * 2 * sizeof(D3DXVECTOR3));
			w.write_vector(faces);
			w.write(m->_bounding_radius);
			w.write(m->_bounding_center);
			w.write(VtxPos | VtxNormal);
			w.write(m->_ib_format);
			w.write(m->_stride);
			w.write(m->_vertex_count);
			w.write(m->_vertex_size);
			w.write(m->_index_count);
			w.write_string(m->_material_name);


		}

	}

//	if (!loaded_binary)
//		save_binary_file(filename, groups);

/*
	std::string binary_name(filename);
	binary_name += ".bin";
	BinaryHeader h;
	get_file_size(filename, &h.textfile_size, NULL);
	get_file_time(filename, NULL, NULL, &h.textfile_write_time);
	h.vertex_size = 2 * sizeof(D3DXVECTOR3);
	h.vertex_count = verts.size();
	h.index_size = sizeof(int);
	h.index_count = faces.size() * 3;
	h.center = center;
	h.radius = radius;

	FILE *f = fopen(binary_name.c_str(), "wb");
	if (!f)
		return true;
	SCOPED_OBJ([=](){ fclose(f); } );
	fwrite(&h, sizeof(h), 1, f);
	fwrite(interleaved, h.vertex_size, h.vertex_count, f);
	fwrite((void*)&faces[0], h.index_size, h.index_count, f);
*/
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

	Group *cur_group = nullptr; 

  TextScanner scanner;
  if (!scanner.load(filename))
    return false;

	int running_vert_idx = 0, running_normal_idx = 0, running_tex_idx = 0;
	int running_face_idx = 0;

	string2 s;
	bool new_mesh = true;
  while (true) {

		if (!scanner.read_string(&s))
			break;

		if (s == "vn") {
			if (cur_group->normals.empty())
				cur_group->normals.reserve(10000);
			std::vector<float> f;
			scanner.read_floats(&f);
			if (f.size() != 3)
				return false;
			cur_group->normals.push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
			running_normal_idx++;

		} else if (s == "vt") {
			if (cur_group->tex_coords.empty())
				cur_group->tex_coords.reserve(10000);
			std::vector<float> t;
			scanner.read_floats(&t);
			if (t.size() != 3)
				return false;
			cur_group->tex_coords.push_back(D3DXVECTOR2(t[0], t[1]));
			running_tex_idx++;

		} else if (s == "g") {

			scanner.read_string(&cur_group->name);

			if (!scanner.skip_to_next_line())
				break;
		} else if (s == "s") {

			// new smoothing group
			if (!scanner.skip_to_next_line())
				break;

		} else if (s == "v") {

			if (new_mesh) {
				if (cur_group != NULL) {
					groups->push_back(cur_group);
				}
				cur_group = new Group("default");
				cur_group->face_ofs = running_face_idx;
				cur_group->vert_ofs = running_vert_idx;
				cur_group->normal_ofs = running_normal_idx;
				cur_group->tex_ofs = running_tex_idx;
				cur_group->verts.reserve(10000);
				cur_group->faces.reserve(10000);
				new_mesh = false;
			}

			std::vector<float> f;
			scanner.read_floats(&f);
			if (f.size() != 3)
				return false;
			cur_group->verts.push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
			running_vert_idx++;

		} else if (s == "f") {
			// really a misnomer, but this will force us to save the mesh when we find the next vertex
			new_mesh = true;

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

	if (!cur_group->faces.empty() && !cur_group->verts.empty())
		groups->push_back(cur_group);

  return true;
}
