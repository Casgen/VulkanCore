#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <sys/types.h>
#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "../Event/Event.h"
#include "../Model/MouseState.h"

namespace VkCore
{
    struct WindowProps
    {
        std::string m_Title;
        uint32_t m_Width, m_Height;
        std::function<void(Event&)> m_CbFunction;

        WindowProps(const std::string& title = "Hello, World", uint32_t width = 1280, uint32_t height = 720)
            : m_Title(title), m_Width(width), m_Height(height)
        {
        }
    };

    class Window
    {
      public:
        Window(vk::Instance& vkInstance, const WindowProps& props = WindowProps());

        ~Window()
        {
            glfwTerminate();
            glfwDestroyWindow(m_GlfwWindow);
        }

        [[nodiscard]] GLFWwindow* GetGLFWWindow() const;
        [[nodiscard]] MouseState GetMouseState() const;
        [[nodiscard]] WindowProps GetProps() const;
        [[nodiscard]] bool ShouldClose() const;
        [[nodiscard]] vk::SurfaceKHR& GetSurface();

        // ---------------- SETTERS ---------------------

        void SetEventCallback(const std::function<void(Event&)>& callback);
        void SetWindowSize(const int width, const int height);

        // ----------------------------------------------


        static void ErrorCallback(int error, const char* desc);

        static std::vector<const char*> GetRequiredInstanceExtensions();

      private:
        vk::SurfaceKHR m_Surface;
        GLFWwindow* m_GlfwWindow;
        WindowProps m_Props;

        MouseState m_MouseState;
    };

} // namespace VkCore
