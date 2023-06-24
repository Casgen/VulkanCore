#include <iostream>
#include <stdexcept>

#include "../Event/KeyEvent.h"
#include "../Event/MouseEvent.h"
#include "../Event/WindowEvent.h"
#include "../Log/Log.h"

#include "GLFW/glfw3.h"
#include "Window.h"
#include "vulkan/vulkan.hpp"

using namespace Log;

namespace VkCore
{
    Window::Window(const vk::Instance& vkInstance, const WindowProps &props) : m_Props(props)
    {
        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit())
        {
            LOG(ECategory::Window, ESeverity::Fatal, "GLFW couldn't be initialized!")
            throw std::runtime_error("GLFW couldn't be initialized!");
        }

        if (!glfwVulkanSupported())
        {
            LOG(ECategory::Window, ESeverity::Fatal, "GLFW window can not be created! Vulkan is not supported!")
            throw std::runtime_error("GLFW window can not be created! Vulkan is not supported!");
        }

        LOG(ECategory::Window, ESeverity::Info, "GLFW successfully initialized.")

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_Window = glfwCreateWindow(m_Props.m_Height, m_Props.m_Width, m_Props.m_Title.c_str(), nullptr, nullptr);

        if (m_Window == nullptr)
        {
            LOG(ECategory::Window, ESeverity::Fatal, "Failed to create GLFW window! m_Window is nullptr!")
            throw std::runtime_error("Failed to create GLFW window! m_Window is nullptr!");
        }

        glfwSetWindowUserPointer(m_Window, this);

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) -> void {
            const WindowProps &data = static_cast<Window *>(glfwGetWindowUserPointer(window))->m_Props;

            switch (action)
            {
            case GLFW_PRESS: {
                KeyPressedEvent event = KeyPressedEvent(key);
                return data.m_CbFunction(event);
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event = KeyReleasedEvent(key);
                return data.m_CbFunction(event);
            }
            case GLFW_REPEAT: {
                KeyRepeatedEvent event = KeyRepeatedEvent(key);
                return data.m_CbFunction(event);
            }
            default:
                std::cout << "Action couldn't be recognized!" << std::endl;
            }
        });

        LOG(ECategory::Window, ESeverity::Verbose, "KeyCallback set.")

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) -> void {
            Window *usrWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event = MouseButtonPressedEvent(button);
                return usrWindow->m_Props.m_CbFunction(event);
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event = MouseButtonReleasedEvent(button);
                return usrWindow->m_Props.m_CbFunction(event);
            }
            case GLFW_REPEAT: {
                MouseButtonRepeatedEvent event = MouseButtonRepeatedEvent(button);
                return usrWindow->m_Props.m_CbFunction(event);
            }
            }
        });

        LOG(ECategory::Window, ESeverity::Verbose, "MouseButton callback set.")

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) -> void {
            const WindowProps &data = static_cast<Window *>(glfwGetWindowUserPointer(window))->m_Props;

            WindowClosedEvent event;
            data.m_CbFunction(event);
        });

        LOG(ECategory::Window, ESeverity::Verbose, "Close callback set.")

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xoffset, double yoffset) -> void {
            const WindowProps &data = static_cast<Window *>(glfwGetWindowUserPointer(window))->m_Props;

            MouseScrolledEvent event(xoffset, yoffset);
            data.m_CbFunction(event);
        });

        LOG(ECategory::Window, ESeverity::Verbose, "ScrollCallback callback set.")
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos) -> void {
            const Window *usrWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(xpos, ypos);
            usrWindow->m_Props.m_CbFunction(event);
        });
        LOG(ECategory::Window, ESeverity::Verbose, "CursorPos callback set.")

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
            const Window *usrWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

            WindowResizedEvent event(width, height);
            usrWindow->m_Props.m_CbFunction(event);
        });

        LOG(ECategory::Window, ESeverity::Verbose, "WindowSize callback set.")
    }

    void Window::SetWindowSize(const int width, const int height)
    {
        m_Props.m_Width = width;
        m_Props.m_Height = height;
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::ErrorCallback(int error, const char *desc)
    {
        std::cout << "GLFW Error has occured! (" << (error) << ") Desc: " << desc << std::endl;
    }

} // namespace VkCore
