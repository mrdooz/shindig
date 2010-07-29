#pragma once

struct DebugCamera;
struct KeyInfo;

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

	void keydown(const KeyInfo& k);
	void keyup(const KeyInfo& k);

	void debug_camera(DebugCamera *d);
};

class ObjectCamera : public Camera
{
public:
	ObjectCamera();
private:
	float _alpha, _theta;
};
