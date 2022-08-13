#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace rvk {

class VK_window {

    public:
        VK_window(int x, int y, std::string name);
        ~VK_window();  

        VK_window(const VK_window &) = delete;
        VK_window &operator=(const VK_window &) = delete;        

        bool should_close() {return glfwWindowShouldClose(window); }  
        VkExtent2D get_extend() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}

        bool was_window_resized() {return frame_buffer_resized;}
        void reset_resize_flag() {frame_buffer_resized = false;}

        GLFWwindow *getGLFWwindow() const { return window; }

        void create_window_surface(VkInstance instance, VkSurfaceKHR *surface);

    private:

        static void frame_buffer_resize_callback(GLFWwindow *window, int width, int height);

        void init_window();

        int width;
        int height;
        bool frame_buffer_resized = false;

        std::string window_name;
        GLFWwindow *window;
};
}   // namespace