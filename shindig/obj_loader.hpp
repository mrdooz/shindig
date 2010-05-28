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
    DWORD textfile_size;
    FILETIME textfile_write_time;
		int	vertex_size, vertex_count;
		int index_size, index_count;
		float radius;
		D3DXVECTOR3 center;
  };

  typedef std::vector<D3DXVECTOR3> Verts;
  typedef std::vector<Face> Faces;
  typedef std::map<int, std::vector<int> > VertsByFace;

  bool load_binary_file(const char *filename, Mesh2 **mesh);

	void calc_bounding_sphere(const Verts& verts, float *radius, D3DXVECTOR3 *center);

  bool parse_file(const char *filename, Verts *verts, Faces *faces, VertsByFace *verts_by_face);
};
