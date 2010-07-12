#include "stdafx.h"
#include "imgui.hpp"
#include "system.hpp"
#include <celsus/graphics.hpp>
#include "resource_manager.hpp"
#include "lua_utils.hpp"

IMGui *IMGui::_instance = nullptr;

IMGui& IMGui::instance()
{
  if (!_instance)        
    _instance = new IMGui();
  return *_instance;
}

IMGui::IMGui()
{

}

bool IMGui::init()
{
  using namespace fastdelegate;
  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  Graphics& g = Graphics::instance();
  ID3D11Device *d = g.device();

  if (!_vb.create(10000))
    return false;

  RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/imgui_states.lua", System::kDirRelative), MakeDelegate(this, &IMGui::load_states), true));
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/imgui.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &IMGui::load_effect)));

  return true;
}

bool IMGui::close()
{
  return true;
}

bool IMGui::init_frame()
{
  return true;
}

bool IMGui::render()
{
  return true;
}

bool IMGui::load_states(const string2& filename)
{
  auto& s = System::instance();
  if (!::load_states(filename, "default_blend", "default_dss", NULL, &_blend_state.p, &_dss.p, NULL))
    return false;

  return true;
}

void IMGui::load_effect(EffectWrapper *effect)
{
  SAFE_DELETE(_effect);
  effect = _effect;

  InputDesc(). 
    add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
    add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
    create(_layout, _effect);
}
