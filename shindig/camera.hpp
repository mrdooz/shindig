#pragma once
#include <celsus/math_utils.hpp>

struct DebugCamera;
struct KeyInfo;
struct MouseInfo;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual D3DXMATRIX view() const;
	virtual D3DXMATRIX proj() const;

protected:
	Frame _frame;
	//D3DXVECTOR3 _pos, _lookat, _up;
	float _aspect, _fov;
	float _near_plane, _far_plane;

private:

	void mouse_move(const MouseInfo& m);
	void key_down(const KeyInfo& k);
	void key_up(const KeyInfo& k);

	void debug_camera(DebugCamera *d);
};

class ObjectCamera : public Camera
{
public:
	ObjectCamera();
private:
	float _alpha, _theta;
};
