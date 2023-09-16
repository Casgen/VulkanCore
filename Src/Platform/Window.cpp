#include <iostream>
#include <stdexcept>

#include "../Event/KeyEvent.h"
#include "../Event/MouseEvent.h"
#include "../Event/WindowEvent.h"
#include "../Log/Log.h"

#include "../Vk/Utils.h"
#include "GLFW/glfw3.h"
#include "Window.h"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{

    Window::Window(vk::Instance& vkInstance, const WindowProps& props) : m_Props(props)
    {
        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit())
        {
            LOG(Window, Fatal, "GLFW couldn't be initialized!")
            throw std::runtime_error("GLFW couldn't be initialized!");
        }

        if (!glfwVulkanSupported())
        {
            LOG(Window, Fatal, "GLFW window can not be created! Vulkan is not supported!")
            throw std::runtime_error("GLFW window can not be created! Vulkan is not supported!");
        }

        LOG(Window, Info, "GLFW successfully initialized.")

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_GlfwWindow = glfwCreateWindow(m_Props.m_Width, m_Props.m_Height, m_Props.m_Title.c_str(), nullptr, nullptr);

        if (m_GlfwWindow == nullptr)
        {
            LOG(Window, Fatal, "Failed to create GLFW window! m_Window is nullptr!")
            throw std::runtime_error("Failed to create GLFW window! m_Window is nullptr!");
        }

        glfwSetWindowUserPointer(m_GlfwWindow, this);

        // glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
        //     const WindowProps& data = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_Props;
        //
        //     switch (action)
        //     {
        //     case GLFW_PRESS: {
        //         KeyPressedEvent event = KeyPressedEvent(key);
        //         return data.m_CbFunction(event);
        //     }
        //     case GLFW_RELEASE: {
        //         KeyReleasedEvent event = KeyReleasedEvent(key);
        //         return data.m_CbFunction(event);
        //     }
        //     case GLFW_REPEAT: {
        //         KeyRepeatedEvent event = KeyRepeatedEvent(key);
        //         return data.m_CbFunction(event);
        //     }
        //     default:
        //         std::cout << "Action couldn't be recognized!" << std::endl;
        //     }
        // });
        //
        // LOG(Window, Verbose, "KeyCallback set.")
        //
        // glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow* window, int button, int action, int mods) -> void {
        //     Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        //
        //     switch (action)
        //     {
        //     case GLFW_PRESS: {
        //         MouseButtonPressedEvent event = MouseButtonPressedEvent(button);
        //         return usrWindow->m_Props.m_CbFunction(event);
        //     }
        //     case GLFW_RELEASE: {
        //         MouseButtonReleasedEvent event = MouseButtonReleasedEvent(button);
        //         return usrWindow->m_Props.m_CbFunction(event);
        //     }
        //     case GLFW_REPEAT: {
        //         MouseButtonRepeatedEvent event = MouseButtonRepeatedEvent(button);
        //         return usrWindow->m_Props.m_CbFunction(event);
        //     }
        //     }
        // });
        //
        // LOG(Window, Verbose, "MouseButton callback set.")
        //
        // glfwSetWindowCloseCallback(m_GlfwWindow, [](GLFWwindow* window) -> void {
        //     const WindowProps& data = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_Props;
        //
        //     WindowClosedEvent event;
        //     data.m_CbFunction(event);
        // });
        //
        // LOG(Window, Verbose, "Close callback set.")
        //
        // glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) -> void {
        //     const WindowProps& data = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_Props;
        //
        //     MouseScrolledEvent event(xoffset, yoffset);
        //     data.m_CbFunction(event);
        // });
        //
        // LOG(Window, Verbose, "ScrollCallback callback set.")
        // glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow* window, double xpos, double ypos) -> void {
        //     const Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        //
        //     MouseMovedEvent event(xpos, ypos);
        //     usrWindow->m_Props.m_CbFunction(event);
        // });
        // LOG(Window, Verbose, "CursorPos callback set.")
        //
        // glfwSetWindowSizeCallback(m_GlfwWindow, [](GLFWwindow* window, int width, int height) {
        //     const Window* usrWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        //
        //     WindowResizedEvent event(width, height);
        //     usrWindow->m_Props.m_CbFunction(event);
        // });
        //
        // LOG(Window, Verbose, "WindowSize callback set.")
    }

    Window::~Window()
    {
        glfwTerminate();
        glfwDestroyWindow(m_GlfwWindow);
    }

    void Window::CreateSurface(const vk::Instance& instance)
    {
        VkResult err = glfwCreateWindowSurface(static_cast<VkInstance>(instance), m_GlfwWindow, nullptr,
                                               reinterpret_cast<VkSurfaceKHR*>(&m_Surface));
        VkCore::Utils::CheckVkResult(err);
    }

    void Window::SwapBuffers() const
    {
        glfwSwapBuffers(m_GlfwWindow);
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

    WindowProps Window::GetProps() const
    {
        return m_Props;
    }

    vk::SurfaceKHR& Window::GetSurface()
    {
        return m_Surface;
    }

    void Window::SetEventCallback(const std::function<void(Event&)>& callback)
    {
        m_Props.m_CbFunction = callback;
    }

    // ------------------------------------------

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_GlfwWindow);
    }

    void Window::SetWindowSize(const int width, const int height)
    {
        m_Props.m_Width = width;
        m_Props.m_Height = height;
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
