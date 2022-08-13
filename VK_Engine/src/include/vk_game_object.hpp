#pragma once

#include "vk_model.hpp"

//glm
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

namespace rvk {
    struct transform_comp {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMAtrix();
    };

    struct pointLightComponent {
        float lightIntesity = 1.0f;
    };

    class VK_GameObject {
        public:
            using id_t = unsigned int;
            using Map = std::unordered_map<id_t, VK_GameObject>;

            static VK_GameObject create_game_object() {
                static id_t current_id = 0;
                return VK_GameObject(current_id++);
            }

            static VK_GameObject makePointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f, 1.f, 1.f));

            VK_GameObject(const VK_GameObject &) = delete;
            VK_GameObject &operator=(const VK_GameObject &) = delete;
            VK_GameObject(VK_GameObject &&) = default;
            VK_GameObject &operator=(VK_GameObject &&) = default;

            id_t get_id() {return id;};

            glm::vec3 color{};
            transform_comp transform{};
            //transform_2d_comp transform2d{};

            //optional
            std::shared_ptr<VK_model> model{};
            std::unique_ptr<pointLightComponent> pointLight = nullptr;
        
        private:
            VK_GameObject(id_t obj_id) : id{obj_id} {}

            id_t id;
    };
} // namespace rvk
