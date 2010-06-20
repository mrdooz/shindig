#include "stdafx.h"
#include "obj_loader.hpp"
#include "celsus/text_scanner.hpp"
#include "celsus/math_utils.hpp"
#include "celsus/file_utils.hpp"
#include "mesh2.hpp"

bool ObjLoader::load_binary_file(const char *filename, Mesh2 **mesh)
{
  std::string binary_name(filename);
  binary_name += ".bin";

  if (!file_exists(binary_name.c_str()))
    return false;

  FILE *f = fopen(binary_name.c_str(), "rb");
  if (f == NULL) 
    return false;
  SCOPED_OBJ([=](){fclose(f);});

	// read the header, and check that the text file's size and date
	// match
  BinaryHeader header;
	if (fread(&header, sizeof(header), 1, f) != 1)
		return false;

	DWORD s;
	if (!get_file_size(filename, &s, NULL)) 
		return false;
	FILETIME w;
	if (!get_file_time(filename, NULL, NULL, &w)) 
		return false;

	if (s != header.textfile_size || w.dwLowDateTime != header.textfile_write_time.dwLowDateTime || w.dwHighDateTime != header.textfile_write_time.dwHighDateTime)
		return false;

	uint8_t *verts = new uint8_t[header.vertex_size * header.vertex_count];
	SCOPED_OBJ([=](){ delete [] verts; } );
	fread(verts, header.vertex_size, header.vertex_count, f);

	uint8_t *idx = new uint8_t[header.index_size * header.index_count];
	SCOPED_OBJ([=](){ delete [] idx; });
	fread(idx, header.index_size, header.index_count, f);

	Mesh2 *m = *mesh = new Mesh2();
	auto d = Graphics::instance().device();
	if (FAILED(create_static_vertex_buffer(d, header.vertex_count, header.vertex_size, verts, &m->_vb)))
		return false;

	if (FAILED(create_static_index_buffer(d, header.index_count, header.index_size, idx, &m->_ib)))
		return false;

	m->_bounding_radius = header.radius;
	m->_bounding_center = header.center;
	m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
	m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
	m->_ib_format = DXGI_FORMAT_R32_UINT;
	m->_stride = header.vertex_size;
	m->_vertex_count = header.vertex_count;
	m->_vertex_size = header.vertex_size;
	m->_index_count = header.index_count;

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

bool ObjLoader::load_from_file(const char *filename, Mesh2 **mesh)
{
  if (load_binary_file(filename, mesh))
    return true;

  // note, we convert coordinates to LHS, and flip the winding
  // order of the faces while parsing
  Verts verts;
  Faces faces;

  // save which face each vertex belongs do
  VertsByFace verts_by_face;

  if (!parse_file(filename, &verts, &faces, &verts_by_face))
    return false;

	D3DXVECTOR3 center;
	float radius;
	calc_bounding_sphere(verts, &radius, &center);

  std::vector<D3DXVECTOR3> face_normals;
  std::vector<D3DXVECTOR3> vertex_normals;

  // calc face normals
  face_normals.reserve(faces.size());
  for (auto i = faces.begin(), e = faces.end(); i != e; ++i) {
    const D3DXVECTOR3& v0 = verts[i->a];
    const D3DXVECTOR3& v1 = verts[i->b];
    const D3DXVECTOR3& v2 = verts[i->c];
    face_normals.push_back(vec3_normalize(vec3_cross(v1 - v0, v2 - v0)));
  }

  // calc vertex normals
  vertex_normals.reserve(verts.size());
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

	Mesh2 *m = *mesh = new Mesh2();
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
  return true;
}

bool ObjLoader::parse_file(const char *filename, Verts *verts, Faces *faces, VertsByFace *verts_by_face)
{
  TextScanner scanner;
  if (!scanner.load(filename))
    return false;

  verts->reserve(10000);
  faces->reserve(10000);

  int vert_idx = 0;
  int face_idx = 0;

  while (true) {

#define IS_ID(id) !memcmp(id, buf, 2)
		static char buf[2];
		if (!scanner.peek(buf, 2))
			break;

		if (IS_ID("vn")) {

			if (!scanner.skip_to_next_line())
				break;
		} else if (IS_ID("g ")) {

			if (!scanner.skip_to_next_line())
				break;
		} else if (IS_ID("s ")) {

			// new smoothing group
			if (!scanner.skip_to_next_line())
				break;

		} else if (IS_ID("v ")) {

			scanner.skip_chars("v \t");
			std::vector<float> f;
			scanner.read_floats(&f);
			if (f.size() != 3)
				return false;
			verts->push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
			vert_idx++;

		} else if (IS_ID("f ")) {

			if (!scanner.skip_chars("f \t"))
				break;

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
					int i0 = v[0] - 1, i1 = v[2] - 1, i2 = v[1] - 1;
					faces->push_back(Face(i0, i1, i2));
					(*verts_by_face)[i0].push_back(face_idx);
					(*verts_by_face)[i1].push_back(face_idx);
					(*verts_by_face)[i2].push_back(face_idx);
					face_idx++;
				}
				break;
			case 4:
				{
					int i0 = v[0] - 1, i1 = v[2] - 1, i2 = v[1] - 1, i3 = v[3] - 1;
					faces->push_back(Face(i0, i1, i2));
					(*verts_by_face)[i0].push_back(face_idx);
					(*verts_by_face)[i1].push_back(face_idx);
					(*verts_by_face)[i2].push_back(face_idx);
					face_idx++;

					faces->push_back(Face(i0, i1, i3));
					(*verts_by_face)[i0].push_back(face_idx);
					(*verts_by_face)[i1].push_back(face_idx);
					(*verts_by_face)[i3].push_back(face_idx);
					face_idx++;
				}
				break;
			default:
				return false;
			}

			if (!scanner.skip_to_next_line())
				break;

		} else {
			scanner.skip_to_next_line();
		}
  }

#undef IS_ID
  return true;
}
