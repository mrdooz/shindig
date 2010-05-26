#include "stdafx.h"
#include "obj_loader.hpp"
#include "celsus/text_scanner.hpp"
#include "celsus/math_utils.hpp"
#include "mesh2.hpp"


bool ObjLoader::load_from_file(const char *filename, Mesh2 **mesh)
{
  // note, we convert coordinates to LHS, and flip the winding
  // order of the faces while parsing
  Verts verts;
  Faces faces;

  // save which face each vertex belongs do
  VertsByFace verts_by_face;

  if (!parse_file(filename, &verts, &faces, &verts_by_face))
    return false;

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
  for (int i = 0; i < (int)verts.size(); ++i) {
    interleaved[i*2+0] = verts[i];
    interleaved[i*2+1] = vertex_normals[i];
  }

	Mesh2 *m = *mesh = new Mesh2();
  auto d = Graphics::instance().device();
  if (FAILED(create_static_vertex_buffer(d, verts.size(), 2 * sizeof(D3DXVECTOR3), (const uint8_t *)interleaved, &m->_vb)))
    return false;

  SAFE_ADELETE(interleaved);

  if (FAILED(create_static_index_buffer(d, faces.size() * 3, sizeof(int), (const uint8_t *)&faces[0], &m->_ib)))
    return false;

	m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
	m->_input_desc.push_back(CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12));
  m->_ib_format = DXGI_FORMAT_R32_UINT;
  m->_stride = 2 * sizeof(D3DXVECTOR3);
  m->_vertex_count = verts.size();
  m->_vertex_size = 2 * sizeof(D3DXVECTOR3);
  m->_index_count = faces.size() * 3;

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

  while (true)
  {
    char ch;
    if (!scanner.peek(&ch))
      break;
    switch(ch)
    {
    case 's':
      // new smoothing group
      break;
    case 'v':
      {
        scanner.skip_chars("v \t");
        std::vector<float> f;
        scanner.read_floats(&f);
        if (f.size() != 3)
          return false;
        verts->push_back(D3DXVECTOR3(f[0], f[1], -f[2]));
        vert_idx++;
      }
      break;
    case 'f':
      {
        scanner.skip_chars("f \t");
        std::vector<int> i;
        scanner.read_ints(&i);
        if (i.size() != 3)
          return false;
        int i0 = i[0] - 1, i1 = i[2] - 1, i2 = i[1] - 1;
        faces->push_back(Face(i0, i1, i2));
        (*verts_by_face)[i0].push_back(face_idx);
        (*verts_by_face)[i1].push_back(face_idx);
        (*verts_by_face)[i2].push_back(face_idx);
        face_idx++;
      }
      break;
    default:
      break;
    }
    scanner.skip_to_next_line();
  }

  return true;
}
