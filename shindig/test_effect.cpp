#include "stdafx.h"
#include "test_effect.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"

namespace
{
  void depth_stencil_default(D3D11_DEPTH_STENCIL_DESC* desc)
  {
    desc->DepthEnable = TRUE;
    desc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc->DepthFunc = D3D11_COMPARISON_LESS;
    desc->StencilEnable = FALSE;
    desc->StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    desc->StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    const D3D11_DEPTH_STENCILOP_DESC defaultStencilOp =
    { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
    desc->FrontFace = defaultStencilOp;
    desc->BackFace = defaultStencilOp;
  }

  void rasterizer_default(D3D11_RASTERIZER_DESC* desc)
  {
    desc->FillMode = D3D11_FILL_SOLID;
    desc->CullMode = D3D11_CULL_BACK;
    desc->FrontCounterClockwise = FALSE;
    desc->DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
    desc->DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
    desc->SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    desc->DepthClipEnable = TRUE;
    desc->ScissorEnable = FALSE;
    desc->MultisampleEnable = FALSE;
    desc->AntialiasedLineEnable = FALSE;
  }
}

TestEffect::TestEffect()
  : _vs_effect(NULL)
	, _ps_effect(NULL)
	, _vs_fs(NULL)
	, _ps_fs(NULL)
	, _boom(false)
  , _boom_count(0)
{
}

bool TestEffect::init()
{
  using namespace fastdelegate;
  System& sys = System::instance();
	ResourceManager& r = ResourceManager::instance();

  RETURN_ON_FAIL_BOOL(r.load_effect_states(sys.convert_path("effects/states.fx", System::kDirRelative).c_str(), MakeDelegate(this, &TestEffect::states_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_vertex_shader(sys.convert_path("effects/post_process.fx", System::kDirRelative).c_str(), "vsMain", MakeDelegate(this, &TestEffect::post_vs_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_pixel_shader(sys.convert_path("effects/post_process.fx", System::kDirRelative).c_str(), "psMain", MakeDelegate(this, &TestEffect::post_ps_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_vertex_shader(sys.convert_path("effects/default_vs.fx", System::kDirRelative).c_str(), "vsMain", MakeDelegate(this, &TestEffect::vs_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_pixel_shader(sys.convert_path("effects/default_vs.fx", System::kDirRelative).c_str(), "psMain", MakeDelegate(this, &TestEffect::ps_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_scene(sys.convert_path("data/scenes/diskette.rdx", System::kDirDropBox).c_str(), MakeDelegate(this, &TestEffect::scene_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	RETURN_ON_FAIL_BOOL(r.load_materials(sys.convert_path("data/scenes/diskette.json", System::kDirDropBox).c_str(), MakeDelegate(this, &TestEffect::materials_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

	D3D11_RASTERIZER_DESC raster_desc;
	rasterizer_default(&raster_desc);

	//D3D11_BLEND_DESC blend_desc;
	//blend_default(&blend_desc);

	D3D11_DEPTH_STENCIL_DESC depth_desc;
	depth_stencil_default(&depth_desc);

	ID3D11Device* device = Graphics::instance().device();
	device->CreateRasterizerState(&raster_desc, &_rasterizer_state);
	device->CreateDepthStencilState(&depth_desc, &_depth_state);
	_layout.Attach(_vs_effect->create_input_layout(_scene->meshes()[0]->_input_element_descs));

	RETURN_ON_FAIL_BOOL(_rt.create(512, 512), ErrorPredicate<bool>, LOG_ERROR_LN);

	struct
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	} vtx[] = {
		// 0, 1  [0, 1, 2] [2, 1, 3]
		// 2, 3
		{ D3DXVECTOR3(-1, +1, 0.5f), D3DXVECTOR2(0, 0) },
		{ D3DXVECTOR3(+1, +1, 0.5f), D3DXVECTOR2(1, 0) },
		{ D3DXVECTOR3(-1, -1, 0.5f), D3DXVECTOR2(0, 1) },
		{ D3DXVECTOR3(+1, -1, 0.5f), D3DXVECTOR2(1, 1) },
	};

	int indices[] = { 0, 1, 2, 2, 1, 3};

	create_static_vertex_buffer(device, 4, sizeof(vtx[0]), (uint8_t*)vtx, &_full_screen_vb);
	create_static_index_buffer(device, 6, sizeof(indices[0]), (uint8_t*)indices, &_full_screen_ib);

	_viewport = CD3D11_VIEWPORT(0.0f, 0.0f, 1.0f, 1.0f);

	D3D11_INPUT_ELEMENT_DESC fs_desc[] = { 
		CD3D11_INPUT_ELEMENT_DESC("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0),
		CD3D11_INPUT_ELEMENT_DESC("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0),
	};
	_full_screen_layout.Attach(_vs_fs->create_input_layout(fs_desc, ELEMS_IN_ARRAY(fs_desc)));

  //sys.load_callback_times(sys.convert_path("data/mp3/session.txt", System::kDirDropBox).c_str());
  //sys.add_callback(System::Freq(100, 150, 0.075f, fastdelegate::MakeDelegate(this, &TestEffect::callback)));
	//sys.start_mp3();

	return true;
}

void TestEffect::callback(float freq, float amp)
{
	_boom = true;
  _boom_count += 10;
}

bool TestEffect::close()
{
	_scene->release();
	SAFE_DELETE(_vs_effect);
	SAFE_DELETE(_ps_effect);
	SAFE_DELETE(_vs_fs);
	SAFE_DELETE(_ps_fs);
	//_blend_state.Release();
	return true;
}

void TestEffect::render_meshes()
{
	ID3D11Device* device = Graphics::instance().device();
	ID3D11DeviceContext* context = Graphics::instance().context();

	// set shaders
	context->VSSetShader(_vs_effect->vertex_shader(), NULL, 0);
	context->PSSetShader(_ps_effect->pixel_shader(), NULL, 0);
	context->GSSetShader(NULL, NULL, 0);

	context->IASetInputLayout(_layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set blend state
	float blend_factor[4] = {0,0,0,0};
	UINT sample_mask = 0xffffffff;
	context->OMSetBlendState(_blend_state, &blend_factor[0], sample_mask);
	context->RSSetState(_rasterizer_state);
	context->OMSetDepthStencilState(_depth_state, 0);

	// set cbuffer stuff
	D3DXMATRIX proj, view;
	D3DXMatrixPerspectiveFovLH(&proj, (float)D3DXToRadian(45), 1, 1, 1000.0f);
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,0,-10), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));

	_vs_effect->set_vs_variable("mWorldViewProj", view * proj);
	_vs_effect->unmap_buffers();
	_vs_effect->set_cbuffer();

	// rendar!
	for (size_t i = 0; i < _scene->meshes().size(); ++i) {
		Mesh* m = _scene->meshes()[i];

		const auto& material_connection = _materials.material_connections[m->_name];
		const auto& material = _materials.materials[material_connection.material_name];
		_ps_effect->set_ps_variable("diffuse", material.diffuse);
		_ps_effect->unmap_buffers();
		_ps_effect->set_cbuffer();

		UINT ofs = 0;
		UINT strides = m->_vertex_buffer_stride;
		ID3D11Buffer* bufs[] = { m->_vertex_buffer };
		context->IASetVertexBuffers(0, 1, &bufs[0], &strides, &ofs);
		context->IASetIndexBuffer(m->_index_buffer, m->_index_buffer_format, ofs);

		context->DrawIndexed(m->_index_count, 0, 0);
	}
}

bool TestEffect::render()
{
  if (!_vs_effect || !_vs_fs || !_ps_effect || !_ps_fs) {
    return true;
  }

	ID3D11Device* device = Graphics::instance().device();
	ID3D11DeviceContext* context = Graphics::instance().context();

	_rt.set();
  float c = std::min<int>(_boom_count, 20) / 10.0f;
  _rt.clear(D3DXCOLOR(c, c, c, 1));
  if (_boom_count > 0)
    _boom_count--;

	//_boom = false;

	render_meshes();
	Graphics::instance().set_default_render_target();


	context->VSSetShader(_vs_fs->vertex_shader(), NULL, 0);
	context->PSSetShader(_ps_fs->pixel_shader(), NULL, 0);
	context->GSSetShader(NULL, NULL, 0);

	context->IASetInputLayout(_full_screen_layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set blend state
	float blend_factor[4] = {0,0,0,0};
	UINT sample_mask = 0xffffffff;
	context->OMSetBlendState(_blend_state, &blend_factor[0], sample_mask);
	context->RSSetState(_rasterizer_state);
	//context->RSSetViewports(1, &_viewport);
	context->OMSetDepthStencilState(_depth_state, 0);


	ID3D11Buffer* bufs[] = { _full_screen_vb };
	const UINT strides[] = {sizeof(D3DXVECTOR3) + sizeof(D3DXVECTOR2)};
	const UINT ofs [] = { 0 };
	context->IASetVertexBuffers(0, 1, bufs, strides, ofs);
	context->IASetIndexBuffer(_full_screen_ib, DXGI_FORMAT_R32_UINT, 0);

	ID3D11ShaderResourceView* t[] = { _rt.shader_resource_view() };
	context->PSSetShaderResources(0, 1, t);

	ID3D11SamplerState* s[] = { _sampler_state };
	context->PSSetSamplers(0, 1, s);

	context->DrawIndexed(6, 0, 0);

	ID3D11ShaderResourceView* null_textures[] = { NULL };
	context->PSSetShaderResources(0, 1, null_textures);

	return true;
}

void TestEffect::states_loaded(const ResourceManager::EffectStates& states)
{
	_blend_state.Release();
	{
		auto i = states.blend_states.find("AdditiveBlending");
		if (i != states.blend_states.end()) {
			_blend_state = i->second;
		}
	}

	_sampler_state.Release();
	{
		auto i = states.sampler_states.find("MeshTextureSampler");
		if (i != states.sampler_states.end()) {
			_sampler_state = i->second;
		}
	}

}

void TestEffect::vs_loaded(EffectWrapper* effect)
{
  SAFE_DELETE(_vs_effect);
  _vs_effect = effect;
}

void TestEffect::ps_loaded(EffectWrapper* effect)
{
	SAFE_DELETE(_ps_effect);
	_ps_effect = effect;
}

void TestEffect::scene_loaded(Scene* scene)
{
	_scene = scene;
	_scene->add_ref();
}

void TestEffect::materials_loaded(const MaterialFile& materials)
{
	_materials = materials;
}

void TestEffect::post_vs_loaded(EffectWrapper* effect)
{
	SAFE_DELETE(_vs_fs);
	_vs_fs = effect;
}

void TestEffect::post_ps_loaded(EffectWrapper* effect)
{
	SAFE_DELETE(_ps_fs);
	_ps_fs = effect;
}
