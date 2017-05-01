#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/scene_object.hpp"
#include "graphics/shader_program.hpp"
#include "object_component.hpp"

#include "graphics/model.hpp"
#include "movie/projection_object.hpp"
#include "movie/recorder.hpp"

namespace tomovis {

class MovieComponent : public ObjectComponent {
  public:
    MovieComponent(SceneObject& object, int scene_id, std::string file);
    ~MovieComponent();

    void draw(glm::mat4 world_to_screen) override;
    std::string identifier() const override { return "movie"; }

    void tick(float time_elapsed) override;
    void describe() override;

  private:
    SceneObject& object_;
    int scene_id_;
    Model model_;
    ProjectionObject projection_;
    Recorder recorder_;
    float time_ = 0.0f;
};

} // namespace tomovis
