#include "vk_window.hpp"

#include <stdexcept>

namespace rvk {

    VK_window::VK_window(int x, int y, std::string name) : width{x}, height{y}, window_name{name} {
        init_window();
    }

    VK_window::~VK_window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VK_window::init_window() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frame_buffer_resize_callback);
    }

    void VK_window::create_window_surface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("Failed to create windows surface.");
        }
    }

    void VK_window::frame_buffer_resize_callback(GLFWwindow *window, int width, int height) {
        auto vk_window = reinterpret_cast<VK_window *>(glfwGetWindowUserPointer(window));
        vk_window->frame_buffer_resized = true;
        vk_window->width = width;
        vk_window->height = height;
    }
}