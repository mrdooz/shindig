#pragma once

class Font;
class RenderTarget;
class EffectWrapper;

class DebugWriter
{
public:
	DebugWriter();
	~DebugWriter();
	bool init(int width, int height);
	void close();
	void render();
	void reset_frame();
	void write(const char *msg);

private:
	string2	_text;
	int _width;
	int _height;
	Font *_font;
	CComPtr<ID3D11Texture2D> _texture;
	CComPtr<ID3D11ShaderResourceView> _view;
	EffectWrapper *_effect;
};