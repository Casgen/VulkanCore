#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "../Event/KeyEvent.h"
#include "../Event/MouseEvent.h"
#include "../Event/WindowEvent.h"
#include "../Log/Log.h"

#include "../Vk/Utils.h"
#include "GLFW/glfw3.h"
#include "Window.h"
#include "backends/imgui_impl_glfw.h"
#include "vulkan/vulkan_core.h"

namespace VkCore
{

    Window::Window(const std::string& title, uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
    {
        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit())
        {
            LOG(Window, Fatal, "GLFW couldn't be initialized!")
            throw std::runtime_error("GLFW couldn't be initialized!");
        }

        if (glfwVulkanSupported() != GLFW_TRUE)
        {
            LOG(Window, Fatal, "GLFW window can not be created! Vulkan is not supported!")
            throw std::runtime_error("GLFW window can not be created! Vulkan is not supported!");
        }

        LOG(Window, Info, "GLFW successfully initialized.")

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_GlfwWindow = glfwCreateWindow(m_Width, m_Height, title.c_str(), nullptr, nullptr);

        if (m_GlfwWindow == nullptr)
        {
            LOG(Window, Fatal, "Failed to create GLFW window! m_Window is nullptr!")
            throw std::runtime_error("Failed to create GLFW window! m_Window is nullptr!");
        }

        glfwSetWindowUserPointer(m_GlfwWindow, this);
        glfwMakeContextCurrent(m_GlfwWindow);

        RefreshResolution();

        LOG(Window, Verbose, "WindowSize callback set.")
    }

    Window::~Window()
    {
        glfwWaitEvents();

        glfwTerminate();
        glfwDestroyWindow(m_GlfwWindow);
    }

    void Window::SwapBuffers() const
    {
        glfwSwapBuffers(m_GlfwWindow);
    }

    void Window::WaitEvents() const
    {
        glfwWaitEvents();
    }

    // -------------- GETTERS ------------------

    GLFWwindow* Window::GetGLFWWindow() const
    {
        return m_GlfwWindow;
    }

    MouseState Window::GetMouseState() const
    {
        return m_MouseState;
    }

    void Window::SetEventCallback(const std::function<void(Event&)>& callback)
    {
        m_CbFunction = callback;

        glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
            const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS: {
                KeyPressedEvent event = KeyPressedEvent(key);
                return win->m_CbFunction(event);
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event = KeyReleasedEvent(key);
                return win->m_CbFunction(event);
            }
            case GLFW_REPEAT: {
                KeyRepeatedEvent event = KeyRepeatedEvent(key);
                return win->m_CbFunction(event);
            }
            default:
                std::cout << "Action couldn't be recognized!" << std::endl;
            }
        });

        LOG(Window, Verbose, "KeyCallback set.")

        glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow* window, int button, int action, int mods) -> void {
            Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event = MouseButtonPressedEvent(button);
                return usrWindow->m_CbFunction(event);
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event = MouseButtonReleasedEvent(button);
                return usrWindow->m_CbFunction(event);
            }
            case GLFW_REPEAT: {
                MouseButtonRepeatedEvent event = MouseButtonRepeatedEvent(button);
                return usrWindow->m_CbFunction(event);
            }
            default:
                std::cout << "Action couldn't be recognized!" << std::endl;
            }
        });

        LOG(Window, Verbose, "MouseButton callback set.")

        glfwSetWindowCloseCallback(m_GlfwWindow, [](GLFWwindow* window) -> void {
            const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

            WindowClosedEvent event;
            win->m_CbFunction(event);
        });

        LOG(Window, Verbose, "Close callback set.")

        glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) -> void {
            const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

            MouseScrolledEvent event(xoffset, yoffset);
            win->m_CbFunction(event);
        });

        LOG(Window, Verbose, "ScrollCallback callback set.")
        glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow* window, double xpos, double ypos) -> void {
            const Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(xpos, ypos);
            usrWindow->m_CbFunction(event);
        });
        LOG(Window, Verbose, "CursorPos callback set.")

        glfwSetWindowSizeCallback(m_GlfwWindow, [](GLFWwindow* window, int width, int height) {
            Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

            WindowResizedEvent event(width, height);
            usrWindow->m_CbFunction(event);
        });
    }

    // ------------------------------------------

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_GlfwWindow);
    }

    void Window::SetWindowSize(const int width, const int height)
    {
        m_Width = width;
        m_Height = height;
        glfwSetWindowSize(m_GlfwWindow, width, height);
    }

    void Window::ErrorCallback(int error, const char* desc)
    {
        std::cout << "GLFW Error has occured! (" << (error) << ") Desc: " << desc << std::endl;
    }

    std::vector<const char*> Window::GetRequiredInstanceExtensions()
    {
        uint32_t glfwExtensionsCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        return extensions;
    }
} // namespace VkCore
