#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <sys/types.h>
#include "vulkan/vulkan.hpp"
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
        std::function<void(Event &)> m_CbFunction;

        WindowProps(const std::string &title = "Hello, World", uint32_t width = 1280, uint32_t height = 720)
            : m_Title(title), m_Width(width), m_Height(height) {}
    };

    class Window
    {
      public:
        Window(vk::Instance& vkInstance, const WindowProps &props = WindowProps());

        ~Window()
        {
            glfwTerminate();
            glfwDestroyWindow(m_GlfwWindow);
        }

        [[nodiscard]] GLFWwindow *GetGLFWWindow() const
        {
            return m_GlfwWindow;
        }

        void SetEventCallback(const std::function<void(Event &)> &callback)
        {
            m_Props.m_CbFunction = callback;
        }

        [[nodiscard]] MouseState &GetMouseState()
        {
            return m_MouseState;
        }

        [[nodiscard]] WindowProps GetProps() const
        {
            return m_Props;
        }

        void SetWindowSize(const int width, const int height);

        static void ErrorCallback(int error, const char *desc);

        static std::vector<std::string> GetRequiredInstanceExtensions();

      private:
        GLFWwindow *m_GlfwWindow;
        WindowProps m_Props;

        MouseState m_MouseState;
    };

} // namespace VkCore
