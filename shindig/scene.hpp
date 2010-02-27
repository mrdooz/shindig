#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <D3DX10math.h>

class FreeFlyCamera;
class DebugRenderer;


struct Camera
{
  virtual const D3DXMATRIX view_matrix() const = 0;
  virtual const D3DXMATRIX proj_matrix() const = 0;
};

class Scene
{
public:
  const D3DXVECTOR3& get_extents() const { return _extents; }

private:

  D3DXVECTOR3 _extents;

  FreeFlyCamera* _free_fly_camera;
  DebugRenderer* _debug_renderer;

};


#endif
