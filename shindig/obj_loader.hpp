#pragma once

class Mesh2;
struct Material;

// wavefront obj loader
class ObjLoader
{
public:
	typedef std::vector<Material *> Materials;
	typedef std::vector<Mesh2 *> Meshes;
  bool load_from_file(const char *filename, Meshes *meshes);
	bool load_material_file(const char *filename, Materials *materials);
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

	struct Group
	{
		Group(const string2& name) : name(name), vert_ofs(0), vert_count(0), face_ofs(0), face_count(0) {}
		string2 name;
		string2 material_name;
		Verts verts;
		Faces faces;
		int vert_ofs;
		int vert_count;
		int face_ofs;
		int face_count;
		VertsByFace verts_by_face;
	};

	typedef std::vector<Group *> Groups;


  bool load_binary_file(const char *filename, Mesh2 **mesh);

	void calc_bounding_sphere(const Verts& verts, float *radius, D3DXVECTOR3 *center);

  bool parse_file(const char *filename, Groups *groups);
};
