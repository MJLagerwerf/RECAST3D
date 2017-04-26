#include <sstream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "graphics/components/movie_component.hpp"
#include "graphics/components/reconstruction_component.hpp"
#include "graphics/interface/scene_switcher.hpp"
#include "graphics/interface/window.hpp"
#include "scene.hpp"
#include "scene_list.hpp"

namespace tomovis {

SceneSwitcher::SceneSwitcher(SceneList& scenes) : scenes_(scenes) {}

SceneSwitcher::~SceneSwitcher() {}

void SceneSwitcher::describe() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Scenes")) {
            if (ImGui::MenuItem("Next scene", "ctrl + n"))
                next_scene();
            if (ImGui::MenuItem("Add scene", "ctrl + a"))
                add_scene();
            if (ImGui::MenuItem("Add scene (3D)", "ctrl + b"))
                add_scene_3d();
            if (ImGui::MenuItem("Add movie", "ctrl + m"))
                show_movie_modal();

            if (ImGui::MenuItem("Delete scene", "ctrl + d"))
                delete_scene();

            if (scenes_.active_scene()) {

                ImGui::Separator();

                for (auto& scene : scenes_.scenes()) {
                    int index = scene.first;
                    if (ImGui::MenuItem(
                            scene.second.get()->name().c_str(), nullptr,
                            index == scenes_.active_scene_index())) {
                        scenes_.set_active_scene(index);
                    }
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (adding_movie_) {
        ImGui::OpenPopup("Model name");
        if (ImGui::BeginPopupModal("Model name", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Choose the model");
            ImGui::Separator();

            static std::string model_file = "../data/ape.obj";
            static char text_buffer[128];

            std::copy(model_file.data(),
                      model_file.data() + model_file.size() + 1,
                      text_buffer);

            if (ImGui::InputText("model", text_buffer, 128)) {
                model_file = std::string(text_buffer);
            }

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                add_movie_scene(std::string(text_buffer));
                adding_movie_ = false;
            }

            ImGui::EndPopup();
        }
    }
}

bool SceneSwitcher::handle_key(int key, bool down, int mods) {
    if (down && key == GLFW_KEY_N && (mods & GLFW_MOD_CONTROL)) {
        next_scene();
        return true;
    }
    if (down && key == GLFW_KEY_A && (mods & GLFW_MOD_CONTROL)) {
        add_scene();
        return true;
    }
    if (down && key == GLFW_KEY_B && (mods & GLFW_MOD_CONTROL)) {
        add_scene_3d();
        return true;
    }
    if (down && key == GLFW_KEY_M && (mods & GLFW_MOD_CONTROL)) {
        show_movie_modal();
        return true;
    }
    if (down && key == GLFW_KEY_D && (mods & GLFW_MOD_CONTROL)) {
        delete_scene();
        return true;
    }
    return false;
}

void SceneSwitcher::next_scene() {
    if (scenes_.scenes().empty())
        return;

    auto active_scene_it = scenes_.scenes().find(scenes_.active_scene_index());
    ++active_scene_it;
    if (active_scene_it == scenes_.scenes().end())
        active_scene_it = scenes_.scenes().begin();

    scenes_.set_active_scene((*active_scene_it).first);
}

void SceneSwitcher::add_scene() {
    std::stringstream ss;
    ss << "Scene #" << scenes_.scenes().size() + 1;
    scenes_.set_active_scene(scenes_.add_scene(ss.str()));
}

void SceneSwitcher::add_scene_3d() {
    std::stringstream ss;
    ss << "3D Scene #" << scenes_.scenes().size() + 1;
    scenes_.set_active_scene(scenes_.add_scene(ss.str(), -1, true, 3));
    auto& obj = scenes_.active_scene()->object();
    obj.add_component(
        std::make_unique<ReconstructionComponent>(obj, obj.scene_id()));
}

void SceneSwitcher::show_movie_modal() {
    adding_movie_ = true;
}

void SceneSwitcher::add_movie_scene(std::string file) {
    std::stringstream ss;
    ss << "Movie Scene #" << scenes_.scenes().size() + 1;
    scenes_.set_active_scene(scenes_.add_scene(ss.str(), -1, true, 3));
    auto& obj = scenes_.active_scene()->object();
    obj.add_component(std::make_unique<MovieComponent>(obj, obj.scene_id(), file));
}

void SceneSwitcher::delete_scene() {
    if (!scenes_.active_scene())
        return;

    scenes_.delete_scene(scenes_.active_scene_index());
}

} // namespace tomovis
