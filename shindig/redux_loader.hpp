#ifndef REDUX_LOADER_HPP
#define REDUX_LOADER_HPP

class ChunkIo;
class MaterialManager;
class Scene;
class AnimationNode;
class AnimationManager;

class ReduxLoader
{
public:
  ReduxLoader(const std::string& filename, Scene* scene, AnimationManager* animation_manager);
  bool	load();
private:
  void	load_camera(ChunkIo& reader);
  void  load_mesh(ChunkIo& reader);
  void  load_hierarchy_inner(ChunkIo& reader, AnimationNode* parent);
  void  load_hierarchy(ChunkIo& reader);
  void  load_animation(ChunkIo& reader);

  std::string filename_;
  Scene* scene_;
  AnimationManager* animation_manager_;
};

#endif // #ifndef REDUX_LOADER_HPP
