#include "StdAfx.h"
#include "redux_loader.hpp"
#include "scene.hpp"
#include "mesh.hpp"

namespace json = json_spirit;

ReduxLoader::ReduxLoader(const std::string& filename, Scene* scene, AnimationManager* animation_manager)
  : filename_(filename)
  , scene_(scene)
  , animation_manager_(animation_manager)
{
}

bool ReduxLoader::load()
{
	try {
		Path rdx_path(filename_);
		std::string rdx_filename(rdx_path.replace_extension("rdx").str());

		uint8_t* data = 0;
		uint32_t data_len = 0;
		ENFORCE(load_file(data, data_len, rdx_filename.c_str()))(rdx_filename);

		ChunkIo reader;
		reader.init_reader(data, data_len);
		while (!reader.is_eof()) {
			ChunkHeader header = reader.cur_header();
			switch ( header.id_) {
			case ChunkHeader::Info: 
				{
				}
				break;
			case ChunkHeader::Hierarchy:
				{
					load_hierarchy(reader);
				}
				break;
			case ChunkHeader::Mesh:
				{
					load_mesh(reader);
				}
				break;
			case ChunkHeader::Light:
				{
				}
				break;
			case ChunkHeader::Camera:
				{
					load_camera(reader);
				}
				break;

			case ChunkHeader::Animation:
				{
					load_animation(reader);
				}
				break;
			}
			reader.next();
		}
	} catch (std::runtime_error& e) {
		LOG_WARNING_LN(e.what());
		return false;
	}
	return true;

}

void ReduxLoader::load_camera(ChunkIo& reader)
{
/*
  Camera* camera = new Camera();
  camera->name_ = reader.read_string();

  camera->eye_pos_ = reader.read_generic<D3DXVECTOR3>();
  camera->forward_vector_ = reader.read_generic<D3DXVECTOR3>();
  camera->up_vector_ = reader.read_generic<D3DXVECTOR3>();
  camera->right_vector_ = reader.read_generic<D3DXVECTOR3>();

  camera->aspect_ratio_ = reader.read_generic<float>();
  camera->horizontal_fov_ = reader.read_generic<float>();
  camera->vertical_fov_ = reader.read_generic<float>();
  camera->near_plane_ = reader.read_generic<float>();
  camera->far_plane_ = reader.read_generic<float>();

  camera->update();

  scene_->cameras_.push_back(CameraPtr(camera));
*/
}

void ReduxLoader::load_hierarchy_inner(ChunkIo& reader, AnimationNode* parent)
{
/*
  const string name(reader.read_string());
  AnimationNode* cur = new AnimationNode(name);

  if (parent == NULL) {
    // top level node
    animation_manager_->root_.push_back(AnimationNodeSPtr(cur));
  } else {
    parent->children_.push_back(AnimationNodeSPtr(cur));
  }

  const uint32_t num_children = reader.read_int();
  for (uint32_t i = 0; i < num_children; ++i) {
    load_hierarchy_inner(reader, cur);
  }
*/
}

void ReduxLoader::load_hierarchy(ChunkIo& reader) 
{
  // We don't want a single root node, so we just ignore the root
  const std::string root_name(reader.read_string());
  assert(root_name == "root");
  const uint32_t num_children = reader.read_int();
  // read children
  for (uint32_t i = 0; i < num_children; ++i) {
    load_hierarchy_inner(reader, NULL);
  }
}

uint32_t to_ms(const float time)
{
  SUPER_ASSERT(time >= 0.0f);
  return (uint32_t)(time * 1000.0f);
}

void ReduxLoader::load_animation(ChunkIo& reader)
{
/*
  animation_manager_->fps_ = reader.read_uint();
  animation_manager_->start_time_ = to_ms(reader.read_generic<float>());
  animation_manager_->end_time_ = to_ms(reader.read_generic<float>());

  const uint32_t num_tracks = reader.read_uint();
  for (uint32_t i = 0; i < num_tracks; ++i) {

    string node_name(reader.read_string());
    const uint32_t num_keys = reader.read_uint();
    AnimationKeys keys;
    for (uint32_t j = 0; j < num_keys; ++j) {
      const uint32_t time = to_ms(reader.read_generic<float>());
      const D3DXVECTOR3 pos = reader.read_generic<D3DXVECTOR3>();
      const D3DXQUATERNION rot = reader.read_generic<D3DXQUATERNION>();
      const D3DXVECTOR3 scale = reader.read_generic<D3DXVECTOR3>();
      keys.push_back(AnimationKey(time, pos, rot, scale));
    }

    if (AnimationNodeSPtr node = animation_manager_->find_node_by_name(node_name)) {
      node->keys_ = keys;
    } else {
      LOG_WARNING_LN("[%s] Unable to find node: %s", __FUNCTION__, node_name.c_str());
    }
  }
*/
}

void ReduxLoader::load_mesh(ChunkIo& reader)
{
  ID3D11Device* device = Graphics::instance().device();

	const std::string mesh_name(reader.read_string());
  Mesh* mesh = new Mesh(mesh_name);
  LOG_VERBOSE_LN("loading mesh: %s", mesh_name.c_str());
  mesh->_transform_name = reader.read_string();
  //mesh->animation_node_ = animation_manager_->find_node_by_name(mesh->transform_name_);

  const uint32_t input_desc_count = reader.read_int();
  D3D11_INPUT_ELEMENT_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  for (uint32_t i = 0; i < input_desc_count; ++i) {
    desc.SemanticName = _strdup(reader.read_cstring());     // memory is freed in the mesh's dtor
    desc.SemanticIndex = reader.read_int();
    desc.Format = static_cast<DXGI_FORMAT>(reader.read_int());
    desc.InputSlot = reader.read_int();
    desc.AlignedByteOffset = reader.read_int();
    mesh->_input_element_descs.push_back(desc);
  }

  const uint32_t vertex_count = reader.read_int();
  const uint32_t vertex_size = reader.read_int();
  uint8_t* vertex_data = reader.read_data(vertex_count * vertex_size);
	create_static_vertex_buffer(device, vertex_count, vertex_size, vertex_data, &mesh->_vertex_buffer);

  const uint32_t index_count = reader.read_int();
  const uint32_t index_size = reader.read_int();
  ENFORCE(index_size == 2 || index_size == 4)(index_size);
  uint8_t* index_data = reader.read_data(index_count * index_size);
  create_static_index_buffer(device, index_count, index_size, index_data, &mesh->_index_buffer);
  mesh->_index_buffer_format = index_size == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

  mesh->_index_count = index_count;
  mesh->_vertex_buffer_stride = vertex_size;

  mesh->_bounding_sphere_center = reader.read_generic<D3DXVECTOR3>();
  mesh->_bounding_sphere_radius = reader.read_generic<float>();

	scene_->_meshes.push_back(mesh);
}

