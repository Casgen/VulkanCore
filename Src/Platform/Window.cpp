#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "../Event/KeyEvent.h"
#include "../Event/MouseEvent.h"
#include "../Event/WindowEvent.h"
#include "../Log/Log.h"

#include "../Vk/Utils.h"
#include "Window.h"

using namespace Log;

namespace VkCore
{
    Window::Window(vk::Instance& vkInstance, const WindowProps &props) : m_Props(props)
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

        m_GlfwWindow = glfwCreateWindow(m_Props.m_Height, m_Props.m_Width, m_Props.m_Title.c_str(), nullptr, nullptr);

        if (m_GlfwWindow == nullptr)
        {
            LOG(ECategory::Window, ESeverity::Fatal, "Failed to create GLFW window! m_Window is nullptr!")
            throw std::runtime_error("Failed to create GLFW window! m_Window is nullptr!");
        }

        std::vector<std::string> instanceExtensions = GetRequiredInstanceExtensions();
        
        vkInstance = VkCore::CreateInstance(props.m_Title,VK_API_VERSION_1_3,instanceExtensions,true);

        glfwSetWindowUserPointer(m_GlfwWindow, this);

        vk::SurfaceKHR surface;

        VkResult err = glfwCreateWindowSurface(static_cast<VkInstance>(vkInstance), m_GlfwWindow, nullptr,
                                               reinterpret_cast<VkSurfaceKHR *>(&surface));

        VkCore::CheckVkResult(err);

        glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) -> void {
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

        glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow *window, int button, int action, int mods) -> void {
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

        glfwSetWindowCloseCallback(m_GlfwWindow, [](GLFWwindow *window) -> void {
            const WindowProps &data = static_cast<Window *>(glfwGetWindowUserPointer(window))->m_Props;

            WindowClosedEvent event;
            data.m_CbFunction(event);
        });

        LOG(ECategory::Window, ESeverity::Verbose, "Close callback set.")

        glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow *window, double xoffset, double yoffset) -> void {
            const WindowProps &data = static_cast<Window *>(glfwGetWindowUserPointer(window))->m_Props;

            MouseScrolledEvent event(xoffset, yoffset);
            data.m_CbFunction(event);
        });

        LOG(ECategory::Window, ESeverity::Verbose, "ScrollCallback callback set.")
        glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow *window, double xpos, double ypos) -> void {
            const Window *usrWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

            MouseMovedEvent event(xpos, ypos);
            usrWindow->m_Props.m_CbFunction(event);
        });
        LOG(ECategory::Window, ESeverity::Verbose, "CursorPos callback set.")

        glfwSetWindowSizeCallback(m_GlfwWindow, [](GLFWwindow *window, int width, int height) {
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
        glfwSetWindowSize(m_GlfwWindow, width, height);
    }

    void Window::ErrorCallback(int error, const char *desc)
    {
        std::cout << "GLFW Error has occured! (" << (error) << ") Desc: " << desc << std::endl;
    }

    std::vector<std::string> GetRequiredInstanceExtensions()
    {

        uint32_t glfwExtensionsCount;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

        std::vector<std::string> instanceExtensions;

        instanceExtensions.reserve(glfwExtensionsCount + 1);

        for (uint32_t i = 0; i < glfwExtensionsCount; i++)
        {
            instanceExtensions.push_back(glfwExtensions[i]);
        }
 
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        return std::move(instanceExtensions);
    }

} // namespace VkCore
