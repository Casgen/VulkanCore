#pragma once
#include <cstdint>
#include <functional>
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
        /**
         * Initializes GLFW and creates a new winow. Also Creates a new VkInstance.
         * @param vkInstance a reference to a VkInstance object. In the constructor, the instance will be created.
         * @param props
         */
        Window(vk::Instance& vkInstance, const WindowProps& props = WindowProps());

        ~Window();
        void CreateSurface(const vk::Instance& instance);
        void SwapBuffers() const;
        void WaitEvents() const;

        // ---------------- GETTERS ---------------------

        [[nodiscard]] GLFWwindow* GetGLFWWindow() const;
        [[nodiscard]] MouseState GetMouseState() const;
        [[nodiscard]] WindowProps GetProps() const;
        [[nodiscard]] bool ShouldClose() const;
        [[nodiscard]] vk::SurfaceKHR& GetSurface();

        [[nodiscard]] uint32_t GetWidth()
        {
            return m_Width;
        }
        [[nodiscard]] uint32_t GetHeight()
        {
            return m_Height;
        }

        // ---------------- SETTERS ---------------------

        void SetEventCallback(const std::function<void(Event&)>& callback);
        void SetWindowSize(const int width, const int height);

        void SetWidth(const unsigned int width)
        {
            m_Width = width;
        }

        void SetHeight(const unsigned int height)
        {
            m_Height = height;
        }

        void RefreshResolution()
        {
            int32_t width, height = 0;

            glfwGetFramebufferSize(m_GlfwWindow, &width, &height);

            m_Width = static_cast<uint32_t>(width);
            m_Height = static_cast<uint32_t>(height);
        }

        // ----------------------------------------------

        static void ErrorCallback(int error, const char* desc);

        static std::vector<const char*> GetRequiredInstanceExtensions();

      private:
        vk::SurfaceKHR m_Surface;
        GLFWwindow* m_GlfwWindow;
        WindowProps m_Props;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

        MouseState m_MouseState;
    };

} // namespace VkCore
