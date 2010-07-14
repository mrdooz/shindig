#pragma once

struct DebugCamera;

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual D3DXMATRIX view() const;
	virtual D3DXMATRIX proj() const;

protected:
	D3DXVECTOR3 _pos, _lookat, _up;
	float _aspect, _fov;
	float _near_plane, _far_plane;

private:
	void debug_camera(DebugCamera *d);
};

class ObjectCamera : public Camera
{
public:
	ObjectCamera();
private:
	float _alpha, _theta;
};
