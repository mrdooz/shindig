#include "stdafx.h"
#include "obj_loader.hpp"
#include "celsus/text_scanner.hpp"

bool ObjLoader::load_from_file(const char *filename, Mesh2 *mesh)
{
  TextScanner scanner;
  if (!scanner.load(filename))
    return false;

  std::vector<D3DXVECTOR3> verts;
  std::vector<Face> faces;
  std::set<int> invalid_verts;
  std::set<int> invalid_faces;

  verts.reserve(10000);
  faces.reserve(10000);

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
        if (f.size() == 3)
          verts.push_back(D3DXVECTOR3(f[0], f[1], f[2]));
        else
          invalid_verts.insert(vert_idx);
        vert_idx++;
      }
      break;
    case 'f':
      {
        scanner.skip_chars("f \t");
        std::vector<int> i;
        scanner.read_ints(&i);
        if (i.size() == 3)
          faces.push_back(Face(i[0], i[1], i[2]));
        else
          invalid_faces.insert(face_idx);
        face_idx++;
      }
      break;
    default:
      break;
    }
    scanner.skip_to_next_line();
  }

  return true;

  return true;
}
