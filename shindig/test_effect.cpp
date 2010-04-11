#include "stdafx.h"
#include "test_effect.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "graphics.hpp"

TestEffect::TestEffect()
  : _vs_effect(NULL)
	, _ps_effect(NULL)
{
	container_delete(_scene._meshes);
}

bool TestEffect::init()
{
  System& sys = System::instance();
	ResourceManager& r = ResourceManager::instance();

  RETURN_ON_FAIL_BOOL(r.load_effect_states(sys.convert_path("effects/states.fx", System::kDirRelative).c_str(), fastdelegate::MakeDelegate(this, &TestEffect::states_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(r.load_vertex_shader(sys.convert_path("effects/default_vs.fx", System::kDirRelative).c_str(), "vsMain", fastdelegate::MakeDelegate(this, &TestEffect::vs_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(r.load_pixel_shader(sys.convert_path("effects/default_vs.fx", System::kDirRelative).c_str(), "psMain", fastdelegate::MakeDelegate(this, &TestEffect::ps_loaded)),
		ErrorPredicate<bool>, LOG_ERROR_LN);

  ReduxLoader loader(sys.convert_path("data/scenes/diskette.rdx", System::kDirDropBox), &_scene, NULL);
	RETURN_ON_FAIL_BOOL(loader.load(), ErrorPredicate<bool>, LOG_ERROR_LN);

	// connect the meshes to the effect
	_layout = _vs_effect->create_input_layout(_scene._meshes[0]->_input_element_descs);

	return true;
}

bool TestEffect::close()
{
	SAFE_DELETE(_vs_effect);
	SAFE_DELETE(_ps_effect);
	_blend_state.Release();
	return true;
}

bool TestEffect::render()
{
	ID3D11Device* device = Graphics::instance().device();
	ID3D11DeviceContext* context = Graphics::instance().context();

	// set blend state
	D3DXVECTOR4 blend_factor(1,1,1,1);
	UINT sample_mask = 0;
	context->OMSetBlendState(_blend_state, &blend_factor[0], sample_mask);

	// set shaders
	context->VSSetShader(_vs_effect->vertex_shader(), NULL, 0);
	context->PSSetShader(_ps_effect->pixel_shader(), NULL, 0);

	// set cbuffer stuff
	D3DXMATRIX proj, view;
	D3DXMatrixPerspectiveFovLH(&proj, (float)D3DXToRadian(45), 4/3.0f, 1, 1000.0f);
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,0,-100), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
	_vs_effect->set_variable("mWorldViewProj", view * proj);
	_vs_effect->unmap_buffers();

	// rendar!
	Mesh* m = _scene._meshes[0];
	UINT ofs = 0;
	UINT strides = m->_vertex_buffer_stride;
	ID3D11Buffer* bufs[1] = { m->_vertex_buffer };
	context->IASetVertexBuffers(0, 1, &bufs[0], &strides, &ofs);
	context->IASetIndexBuffer(m->_index_buffer, m->_index_buffer_format, ofs);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(_layout);

	D3D11_VIEWPORT viewports[1] = { CD3D11_VIEWPORT(0.f, 0.f, 640.f, 480.f) };
	context->RSSetViewports(1, viewports);
	_vs_effect->set_cbuffer();
	context->DrawIndexed(m->_index_count, 0, 0);
	return true;
}

void TestEffect::states_loaded(const ResourceManager::BlendStates& states)
{
	_blend_state.Release();
	_blend_state = states.find("AdditiveBlending")->second;
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
