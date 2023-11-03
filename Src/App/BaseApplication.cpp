
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fenv.h>
#include <memory>
#include "glm/fwd.hpp"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

#define VMA_IMPLEMENTATION
#include "BaseApplication.h"
#include "GLFW/glfw3.h"
#include "../Log/Log.h"
#include "../Vk/Model/SwapchainRenderPass.h"
#include "../Vk/Utils.h"
#include "../Vk/Model/Services/Allocator/VmaAllocatorService.h"
#include "../Vk/Model/Services/ServiceLocator.h"
#include "../FileUtils.h"
#include "../Model/Shaders/ShaderLoader.h"

namespace VkCore
{

    Application::Application(const uint32_t width, const uint32_t height, const std::string& title)
        : m_WinWidth(width), m_WinHeight(height), m_Title(title)
    {
        s_Instance = this;
        Logger::SetSeverityFilter(ESeverity::Verbose);
    }

    Application::~Application()
    {
    }

    void Application::PreInitVulkan()
    {
    }

    void Application::InitVulkan()
    {
        // The window has to be created before the instance in order to obtain the required extensions
        // for the VkInstance!
        CreateWindow();
        m_Running = true;

        CreateInstance();

        m_Window->CreateSurface(m_Instance);

        CreateDevices();

        CreateServices();

        m_Device.InitSwapChain(*m_PhysicalDevice, m_Window->GetSurface(), m_WinWidth, m_WinHeight);

        m_DescriptorBuilder = DescriptorBuilder(m_Device);
    }

    void Application::PostInitVulkan()
    {
        // Render Pass
        m_RenderPass = VkCore::SwapchainRenderPass(m_Device);

        m_Camera = Camera({0.f, 0.f, -1.f}, {0.f, 0.f, 0.f}, (float)m_WinWidth / m_WinHeight);

        CreateBuffers();
        CreateDescriptorSets();
        CreatePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void Application::CreateBuffers()
    {
        // Buffer
        float vertices[15] = {
            -0.5f, -0.5f, 1.f, 0.f, 0.f, .0f, .5f, 0.f, 1.f, 0.f, .5f, -0.5f, 0.f, 0.f, 1.f,
        };

        m_VertexBuffer = Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
        m_VertexBuffer.InitializeOnGpu(vertices, 60);

        m_AttributeBuilder.PushAttribute<float>(2).PushAttribute<float>(3).SetBinding(0);

        for (int i = 0; i < m_Device.GetSwapchain()->GetImageCount(); i++)
        {
            Buffer matBuffer = Buffer(vk::BufferUsageFlagBits::eUniformBuffer);
            matBuffer.InitializeOnCpu(sizeof(MatrixBuffer));

            m_MatBuffers.emplace_back(std::move(matBuffer));
        }
    }
    void Application::CreateDescriptorSets()
    {
        m_DescriptorBuilder = DescriptorBuilder(m_Device);

        vk::DescriptorSet tempSet;

        for (const auto& buffer : m_MatBuffers)
        {
            m_DescriptorBuilder.BindBuffer(0, buffer, vk::DescriptorType::eUniformBuffer,
                                           vk::ShaderStageFlagBits::eVertex);
            m_DescriptorBuilder.Build(tempSet, m_DescriptorSetLayout);
            m_DescriptorBuilder.Clear();

            m_DescriptorSets.emplace_back(std::move(tempSet));
        }
    }

    void Application::CreatePipeline()
    {
        const std::vector<ShaderData> shaders = ShaderLoader::LoadClassicShaders("VulkanCore/Res/Shaders/");

        m_PipelineBuilder = GraphicsPipelineBuilder(m_Device);

        m_Pipeline = m_PipelineBuilder.BindShaderModules(shaders)
                         .BindRenderPass(m_RenderPass.GetVkRenderPass())
                         .AddViewport(glm::uvec4(0, 0, m_WinWidth, m_WinHeight))
                         .FrontFaceDirection(vk::FrontFace::eCounterClockwise)
                         .AddDisabledBlendAttachment()
                         .AddDescriptorLayout(m_DescriptorSetLayout)
                         .BindVertexAttributes(m_AttributeBuilder)
                         .SetPrimitiveAssembly(vk::PrimitiveTopology::eTriangleList)
                         .Build(m_PipelineLayout);
    }

    void Application::CreateFramebuffers()
    {
        std::vector<vk::ImageView> imageViews = m_Device.GetSwapchain()->GetImageViews();

        TRY_CATCH_BEGIN()

        for (const vk::ImageView& view : imageViews)
        {

            vk::FramebufferCreateInfo createInfo{};
            createInfo.setWidth(m_WinWidth)
                .setHeight(m_WinHeight)
                .setLayers(1)
                .setRenderPass(m_RenderPass.GetVkRenderPass())
                .setAttachments(view);

            m_SwapchainFramebuffers.emplace_back(m_Device.CreateFrameBuffer(createInfo));
        }

        TRY_CATCH_END()
    }

    void Application::CreateCommandPool()
    {

        vk::CommandPoolCreateInfo createInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                             m_PhysicalDevice->GetQueueFamilyIndices().m_GraphicsFamily.value()};

        m_CommandPool = m_Device.CreateCommandPool(createInfo);
    }

    void Application::CreateCommandBuffer()
    {

        vk::CommandBufferAllocateInfo allocateInfo{};

        allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(m_CommandPool)
            .setCommandBufferCount(m_Device.GetSwapchain()->GetImageViews().size());

        TRY_CATCH_BEGIN()
        m_CommandBuffers = m_Device.AllocateCommandBuffers(allocateInfo);
        TRY_CATCH_END()
    }

    void Application::CreateSyncObjects()
    {
        vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};

        vk::SemaphoreCreateInfo imageAvailableCreateInfo{};
        vk::SemaphoreCreateInfo renderFinishedCreateInfo{};

        TRY_CATCH_BEGIN()

        for (int i = 0; i < m_Device.GetSwapchain()->GetNumberOfSwapBuffers(); i++)
        {
            m_ImageAvailableSemaphores.emplace_back(m_Device.CreateSemaphore(imageAvailableCreateInfo));
            m_RenderFinishedSemaphores.emplace_back(m_Device.CreateSemaphore(renderFinishedCreateInfo));

            m_InFlightFences.emplace_back(m_Device.CreateFence(fenceCreateInfo));
        }

        TRY_CATCH_END()
    }

    void Application::Run()
    {
        PreInitVulkan();
        InitVulkan();
        PostInitVulkan();
        Loop();
        Shutdown();
    };

    void Application::Loop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }
    }

    void Application::DrawFrame()
    {
        m_Device.WaitForFences(m_InFlightFences[m_CurrentFrame], false);

        uint32_t imageIndex;
        vk::ResultValue<uint32_t> result = m_Device.AcquireNextImageKHR(m_ImageAvailableSemaphores[m_CurrentFrame]);

        Utils::CheckVkResult(result.result);

        imageIndex = result.value;

        m_Device.ResetFences(m_InFlightFences[m_CurrentFrame]);
        m_CommandBuffers[m_CurrentFrame].reset();

        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_CommandBuffers[m_CurrentFrame])
            .setWaitSemaphores(m_ImageAvailableSemaphores[m_CurrentFrame])
            .setWaitDstStageMask(dstStageMask)
            .setSignalSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame]);

        TRY_CATCH_BEGIN()

        m_Device.GetGraphicsQueue().submit(submitInfo, m_InFlightFences[m_CurrentFrame]);

        TRY_CATCH_END()

        vk::SwapchainKHR swapchain = m_Device.GetSwapchain()->GetVkSwapchain();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame])
            .setSwapchains(swapchain)
            .setImageIndices(imageIndex)
            .setPResults(nullptr);

        Utils::CheckVkResult(m_Device.GetPresentQueue().presentKHR(presentInfo));

        m_CurrentFrame = (m_CurrentFrame + 1) % m_Device.GetSwapchain()->GetNumberOfSwapBuffers();
    }

    void Application::RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex)
    {
        m_Camera.Update();

        MatrixBuffer ubo{};
        ubo.m_Proj = m_Camera.GetProjMatrix();
        ubo.m_View = m_Camera.GetViewMatrix();

        vk::CommandBufferBeginInfo cmdBufferBeginInfo{};

        m_MatBuffers[imageIndex].UpdateData(&ubo);

        vk::ClearValue clearValue{};
        clearValue.setColor({.0f, .0f, 0.f, 1.f});

        vk::RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.setRenderPass(m_RenderPass.GetVkRenderPass())
            .setRenderArea(vk::Rect2D({0, 0}, {m_WinWidth, m_WinHeight}))
            .setFramebuffer(m_SwapchainFramebuffers[imageIndex])
            .setClearValues(clearValue);

        TRY_CATCH_BEGIN()

        commandBuffer.begin(cmdBufferBeginInfo);

        {
            commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

            // Bind the necessary resource
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, 1,
                                             &m_DescriptorSets[imageIndex], 0, nullptr);

            const VkBuffer* buffer = reinterpret_cast<const VkBuffer*>(&m_VertexBuffer);

            if (buffer == VK_NULL_HANDLE)
            {
                LOG(Vulkan, Fatal, "Failed to create a Destination buffer!")
            }

            // TODO fix the bugged creationg of VkBuffers
            commandBuffer.bindVertexBuffers(0, {m_VertexBuffer.GetVkBuffer()}, {0});

            commandBuffer.draw(3, 1, 0, 0);

            commandBuffer.endRenderPass();
        }

        commandBuffer.end();

        TRY_CATCH_END()
    }

    void Application::Shutdown()
    {
        m_Device.WaitIdle();

        for (auto& buffer : m_MatBuffers)
        {
            buffer.Destroy();
        }

        m_VertexBuffer.Destroy();

        m_DescriptorBuilder.Clear();

        m_Device.Destroy();
        m_Instance.destroy();
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher = EventDispatcher(event);

        switch (event.GetEventType())
        {
        case EventType::MouseBtnPressed:
            dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Application::OnMousePress));
            break;
        case EventType::MouseMoved:
            dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Application::OnMouseMoved));
            break;
        case EventType::MouseBtnReleased:
            dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Application::OnMouseRelease));
            break;
        case EventType::KeyPressed:
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Application::OnKeyPressed));
            break;
        case EventType::KeyReleased:
            dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Application::OnKeyReleased));
            break;
        case EventType::MouseScrolled:
            dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Application::OnMouseScrolled));
            break;
        case EventType::WindowResized:
            dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowResize));
            break;
        }
    }

    bool Application::OnMousePress(MouseButtonEvent& event)
    {
        // Later for ImGui
        // if (m_AppWindow->GetProps().m_ImGuiIO->WantCaptureMouse)
        // {
        //     ImGui_ImplGlfw_MouseButtonCallback(m_AppWindow->GetGLFWWindow(), event.GetKeyCode(), GLFW_PRESS, 0);
        //     return true;
        // }

        switch (event.GetKeyCode())
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            m_MouseState.m_IsLMBPressed = true;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            m_MouseState.m_IsMMBPressed = true;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            m_MouseState.m_IsRMBPressed = true;
            break;
        }

        LOG(Application, Verbose, "Mouse Pressed")
        return true;
    }

    bool Application::OnMouseMoved(MouseMovedEvent& event)
    {
        // ImGui_ImplGlfw_CursorPosCallback(m_AppWindow->GetGLFWWindow(), event.GetPos().x, event.GetPos().y);

        const glm::ivec2 diff = m_MouseState.m_LastPosition - event.GetPos();
        LOGF(Application, Info, "Mouse last position X: %d, Y: %d", m_MouseState.m_LastPosition.x, m_MouseState.m_LastPosition.y)
        LOGF(Application, Info, "Mouse moved with diff X: %d, Y: %d", diff.x, diff.y)

        if (m_MouseState.m_IsRMBPressed)
        {
            m_Camera.Yaw(-diff.x);
            m_Camera.Pitch(-diff.y);
        }

        m_MouseState.UpdatePosition(event.GetPos());

        LOGF(Application, Info, "Mouse moved with position X: %d, Y: %d", event.GetPos().x, event.GetPos().y)

    
        return true;
    }

    bool Application::OnMouseScrolled(MouseScrolledEvent& event)
    {

        if (event.GetYOffset() > 0)
        {
            m_Camera.AddMovementSpeed(.005f);
            return true;
        }
        m_Camera.AddMovementSpeed(-.005f);
        return true;

        LOGF(Application, Info, "Mouse scrolled - X Offset: %d, Y Offset: %d", event.GetXOffset(), event.GetYOffset())
        return true;
    }

    bool Application::OnMouseRelease(MouseButtonEvent& event)
    {

        // Later for ImGui
        // if (m_AppWindow->GetProps().m_ImGuiIO->WantCaptureMouse)
        // {
        //     ImGui_ImplGlfw_MouseButtonCallback(m_AppWindow->GetGLFWWindow(), event.GetKeyCode(), GLFW_RELEASE, 0);
        //     return true;
        // }

        switch (event.GetKeyCode())
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            m_MouseState.m_IsLMBPressed = false;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            m_MouseState.m_IsMMBPressed = false;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            m_MouseState.m_IsRMBPressed = false;
            break;
        }

        return true;
        LOG(Application, Info, "Mouse released")
        return true;
    }

    bool Application::OnKeyPressed(KeyPressedEvent& event)
    {
        switch (event.GetKeyCode())
        {
        case GLFW_KEY_W:
            m_Camera.SetIsMovingForward(true);
            return true;
        case GLFW_KEY_A:
            m_Camera.SetIsMovingLeft(true);
            return true;
        case GLFW_KEY_S:
            m_Camera.SetIsMovingBackward(true);
            return true;
        case GLFW_KEY_D:
            m_Camera.SetIsMovingRight(true);
            return true;
        case GLFW_KEY_E:
            m_Camera.SetIsMovingUp(true);
            return true;
        case GLFW_KEY_Q:
            m_Camera.SetIsMovingDown(true);
            return true;
        default:
            return false;
        }
    }

    bool Application::OnKeyReleased(KeyReleasedEvent& event)
    {
        switch (event.GetKeyCode())
        {
        case GLFW_KEY_W:
            m_Camera.SetIsMovingForward(false);
            return true;
        case GLFW_KEY_A:
            m_Camera.SetIsMovingLeft(false);
            return true;
        case GLFW_KEY_S:
            m_Camera.SetIsMovingBackward(false);
            return true;
        case GLFW_KEY_D:
            m_Camera.SetIsMovingRight(false);
            return true;
        case GLFW_KEY_E:
            m_Camera.SetIsMovingUp(false);
            return true;
        case GLFW_KEY_Q:
            m_Camera.SetIsMovingDown(false);
            return true;
        default:
            return false;
        }
        LOG(Application, Info, "Key released")
        return true;
    }

    bool Application::OnWindowResize(WindowResizedEvent& event)
    {
        LOG(Application, Info, "Window resized")
        return true;
    }

    void Application::CreateWindow()
    {
        VkCore::WindowProps windowProps{m_Title, m_WinWidth, m_WinHeight};

        m_Window = std::make_unique<VkCore::Window>(m_Instance, windowProps);
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
    }

    void Application::CreateInstance()
    {

        std::vector<const char*> requiredExtensions(Window::GetRequiredInstanceExtensions()), layers;

#ifdef DEBUG
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        m_Instance = VkCore::Utils::CreateInstance(m_Title, VK_API_VERSION_1_3, requiredExtensions, layers, true);

#ifdef DEBUG
        // Debug messenger has to be created after creating the VkInstance!
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(VkCore::Utils::PopulateDebugMessengerCreateInfo());
#endif
    }

    void Application::CreateDevices()
    {
        m_PhysicalDevice = new VkCore::PhysicalDevice(m_Instance, m_Window->GetSurface(), m_DeviceExtensions);
        m_Device = VkCore::Device(*m_PhysicalDevice, m_DeviceExtensions);
    }

    void Application::CreateServices()
    {
        VmaAllocatorService* allocationService = new VmaAllocatorService(m_Device, *m_PhysicalDevice, m_Instance);
        ServiceLocator::ProvideAllocatorService(allocationService);
    }

} // namespace VkCore
