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
    class Window
    {
      public:
        /**
         * Initializes GLFW and creates a new winow. Also Creates a new VkInstance.
         * @param title
         * @param width - Width of the window
         * @param height - Height of the window
         */
        Window(const std::string& title = "Hello, World", uint32_t width = 1280, uint32_t height = 720);
        ~Window();

        void InitSurface(const vk::Instance& instance);
        void SwapBuffers() const;
        void WaitEvents() const;

        // ---------------- GETTERS ---------------------
        //
        [[nodiscard]] GLFWwindow* GetGLFWWindow() const;
        [[nodiscard]] MouseState GetMouseState() const;
        [[nodiscard]] bool ShouldClose() const;

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

		void DisableCursor()
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		void EnabledCursor()
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
        GLFWwindow* m_GlfwWindow;

        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

        std::function<void(Event&)> m_CbFunction;

        MouseState m_MouseState;
    };

} // namespace VkCore
