#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <D3DX10math.h>
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
