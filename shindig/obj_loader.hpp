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
		Face() {}
    Face(int va, int vb, int vc, int na, int nb, int nc, int ta, int tb, int tc) : va(va), vb(vb), vc(vc), na(na), nb(nb), nc(nc), ta(ta), tb(tb), tc(tc) {}
		union {
			struct { int va; int vb; int vc; };
			int v[3];
		};
		union {
			struct { int na; int nb; int nc; };
			int n[3];
		};
		union {
			struct { int ta; int tb; int tc; };
			int t[3];
		};
  };

	enum {
		VtxPos = 1 << 0,
		VtxNormal = 1 << 1,
		VtxTex = 1 << 2,
	};

  struct BinaryHeader
  {
		char filename[MAX_PATH];
    DWORD textfile_size;
    FILETIME textfile_write_time;
		int mesh_count;
  };

  typedef std::vector<D3DXVECTOR3> Verts;
	typedef std::vector<D3DXVECTOR3> Normals;
	typedef std::vector<D3DXVECTOR2> TexCoords;
  typedef std::vector<Face> Faces;
  typedef std::map<int, std::vector<int> > VertsByFace;

	struct Group
	{
		Group(const string2& name) : name(name), vert_ofs(0), normal_ofs(0), tex_ofs(0), face_ofs(0), face_count(0) {}
		string2 name;
		string2 material_name;
		Verts verts;
		Normals normals;
		TexCoords tex_coords;
		Faces faces;
		int vert_ofs;
		int normal_ofs;
		int tex_ofs;
		int face_ofs;
		int face_count;
		VertsByFace verts_by_face;
	};

	typedef std::vector<Group *> Groups;


  bool load_binary_file(const char *filename, Mesh2 **mesh);
	bool save_binary_file(const char *filename, const Groups& groups);
	bool load_binary_file(const char *filename, Groups* groups);
	bool load_binary_file(const char *filename, Meshes *meshes);

	void calc_bounding_sphere(const Verts& verts, float *radius, D3DXVECTOR3 *center);

  bool parse_file(const char *filename, Groups *groups);
	void handle_face_transition(bool *reading_face_data, Groups *groups, Group **cur_group, int running_face_idx, int running_vert_idx, int running_normal_idx, int running_tex_idx);
};
