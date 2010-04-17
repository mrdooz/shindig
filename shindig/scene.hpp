#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include "object.hpp"

class FreeFlyCamera;
class DebugRenderer;
struct Mesh;

/*
struct Camera
{
  virtual const D3DXMATRIX view_matrix() const = 0;
  virtual const D3DXMATRIX proj_matrix() const = 0;
};
*/

// data returned from a json material file
struct MaterialFile
{
	struct Material
	{
		std::string name;
		float transparency;
		D3DXCOLOR ambient;
		D3DXCOLOR diffuse;
		D3DXCOLOR emissive;
		D3DXCOLOR specular;
	};


	struct MaterialConnection
	{
		MaterialConnection() {}
		MaterialConnection(const std::string& mesh, const std::string& material) : mesh_name(mesh), material_name(material) {}
		std::string mesh_name;
		std::string material_name;
	};


	struct EffectConnection
	{
		std::string effect_name;
		std::vector<std::string> materials;
	};

	stdext::hash_map< std::string, Material > materials;
	stdext::hash_map< std::string, MaterialConnection> material_connections;
	std::vector<EffectConnection> effect_connections;
};

class Scene : public Object
{
public:
	friend class ReduxLoader;
	typedef std::vector<Mesh*> Meshes;

	Scene();
  const D3DXVECTOR3& get_extents() const { return _extents; }
	Meshes& meshes() { return _meshes; }

private:
	void destroy();
	~Scene();

  D3DXVECTOR3 _extents;

	Meshes _meshes;
  FreeFlyCamera* _free_fly_camera;
  DebugRenderer* _debug_renderer;
};

#endif
