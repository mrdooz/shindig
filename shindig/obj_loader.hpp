#pragma once

class Mesh2;

// wavefront obj loader
class ObjLoader
{
public:
  bool load_from_file(const char *filename, Mesh2 **mesh);
private:

  struct Face
  {
    Face(int a, int b, int c) : a(a), b(b), c(c) {}
    int a, b, c;
  };

  struct BinaryHeader
  {
    int textfile_size;
    FILETIME textfile_write_time;
  };

  typedef std::vector<D3DXVECTOR3> Verts;
  typedef std::vector<Face> Faces;
  typedef std::map<int, std::vector<int> > VertsByFace;

  bool load_binary_file(const char *filename, Mesh2 **mesh);

  bool parse_file(const char *filename, Verts *verts, Faces *faces, VertsByFace *verts_by_face);
};
