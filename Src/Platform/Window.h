#pragma once
#include <GLFW/glfw3.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <sys/types.h>
#include <vulkan/vulkan.hpp>

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
            : m_Title(title), m_Width(width), m_Height(height)
        {
        }
    };

    class Window
    {
      public:
        Window(const vk::Instance& vkInstance, const WindowProps &props = WindowProps());

        ~Window()
        {
            glfwTerminate();
            glfwDestroyWindow(m_Window);
        }

        [[nodiscard]] GLFWwindow *GetGLFWWindow() const
        {
            return m_Window;
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

      private:
        GLFWwindow *m_Window;
        WindowProps m_Props;

        MouseState m_MouseState;
    };

} // namespace VkCore
