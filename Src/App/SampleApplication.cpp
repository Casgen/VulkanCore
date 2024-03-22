#include <cstdint>
#include "vulkan/vulkan.hpp"

#include "SampleApplication.h"
#include "GLFW/glfw3.h"
#include "../Log/Log.h"
#include "../Vk/Utils.h"
#include "../Vk/Devices/DeviceManager.h"
#include "../Vk/Swapchain.h"
#include "../Model/MatrixBuffer.h"
#include "../Model/Shaders/ShaderLoader.h"
#include "vulkan/vulkan_enums.hpp"

namespace VkCore
{

    SampleApplication::SampleApplication(const uint32_t width, const uint32_t height, const std::string& title)
        : BaseApplication(width, height, title)
    {
    }

    void SampleApplication::PostInitVulkan()
    {

        m_Camera = Camera({0.f, 0.f, -2.f}, {0.f, 0.f, 0.f}, (float)m_WinWidth / m_WinHeight);

        CreateBuffers();
        CreateDescriptorSets();
        CreatePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
   }

    void SampleApplication::CreateBuffers()
    {
        // Buffer
        float vertices[15] = {
            -0.5f, -0.5f, 1.f, 0.f, 0.f, .0f, .5f, 0.f, 1.f, 0.f, .5f, -0.5f, 0.f, 0.f, 1.f,
        };

        float cubeVertices[56] = {
        -1, -1, -1, 1,  1, 0, 0,
        1, -1, -1, 1,   1, 0, 0,
        1, -1, 1, 1,    1, 0, 0,
        -1, -1, 1, 1,   1, 0, 0,

        -1, 1, -1, 1,   0, 1, 0,
        1, 1, -1, 1,    0, 1, 0,
        1, 1, 1, 1,     0, 1, 0,
        -1, 1, 1, 1,    0, 1, 0,
    };
        

        m_VertexBuffer = Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
        m_VertexBuffer.InitializeOnGpu(cubeVertices, 56 * sizeof(float));

        m_AttributeBuilder.PushAttribute<float>(4).PushAttribute<float>(3).SetBinding(0);

        m_IndexBuffer = Buffer(vk::BufferUsageFlagBits::eIndexBuffer);
        m_IndexBuffer.InitializeOnGpu(m_CubeIndices.data(), m_CubeIndices.size()*sizeof(uint32_t));


        for (int i = 0; i < DeviceManager::GetDevice().GetSwapchain()->GetImageCount(); i++)
        {
            Buffer matBuffer = Buffer(vk::BufferUsageFlagBits::eUniformBuffer);
            matBuffer.InitializeOnCpu(sizeof(MatrixBuffer));

            m_MatBuffers.emplace_back(std::move(matBuffer));
        }
    }

    void SampleApplication::CreateDescriptorSets()
    {
        m_DescriptorBuilder = DescriptorBuilder(DeviceManager::GetDevice());

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

    void SampleApplication::CreatePipeline()
    {
        const std::vector<ShaderData> shaders = ShaderLoader::LoadClassicShaders("VulkanCore/Res/Shaders/");

        m_PipelineBuilder = GraphicsPipelineBuilder(DeviceManager::GetDevice());

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

    void SampleApplication::CreateFramebuffers()
    {
        std::vector<vk::ImageView> imageViews = DeviceManager::GetDevice().GetSwapchain()->GetImageViews();

        TRY_CATCH_BEGIN()

        for (const vk::ImageView& view : imageViews)
        {

            vk::FramebufferCreateInfo createInfo{};
            createInfo.setWidth(m_WinWidth)
                .setHeight(m_WinHeight)
                .setLayers(1)
                .setRenderPass(m_RenderPass.GetVkRenderPass())
                .setAttachments(view);

            m_SwapchainFramebuffers.emplace_back(DeviceManager::GetDevice().CreateFrameBuffer(createInfo));
        }

        TRY_CATCH_END()
    }

    void SampleApplication::CreateCommandPool()
    {

        vk::CommandPoolCreateInfo createInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                             DeviceManager::GetPhysicalDevice().GetQueueFamilyIndices().m_GraphicsFamily.value()};

        m_CommandPool = DeviceManager::GetDevice().CreateCommandPool(createInfo);
    }

    void SampleApplication::CreateCommandBuffer()
    {

        vk::CommandBufferAllocateInfo allocateInfo{};

        allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(m_CommandPool)
            .setCommandBufferCount(DeviceManager::GetDevice().GetSwapchain()->GetImageViews().size());

        TRY_CATCH_BEGIN()
        m_CommandBuffers = DeviceManager::GetDevice().AllocateCommandBuffers(allocateInfo);
        TRY_CATCH_END()
    }

    void SampleApplication::CreateSyncObjects()
    {
        vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};

        vk::SemaphoreCreateInfo imageAvailableCreateInfo{};
        vk::SemaphoreCreateInfo renderFinishedCreateInfo{};

        TRY_CATCH_BEGIN()

        for (int i = 0; i < DeviceManager::GetDevice().GetSwapchain()->GetNumberOfSwapBuffers(); i++)
        {
            m_ImageAvailableSemaphores.emplace_back(DeviceManager::GetDevice().CreateSemaphore(imageAvailableCreateInfo));
            m_RenderFinishedSemaphores.emplace_back(DeviceManager::GetDevice().CreateSemaphore(renderFinishedCreateInfo));

            m_InFlightFences.emplace_back(DeviceManager::GetDevice().CreateFence(fenceCreateInfo));
        }

        TRY_CATCH_END()
    }

    void SampleApplication::DrawFrame()
    {
    DeviceManager::GetDevice().WaitForFences(m_InFlightFences[m_CurrentFrame], false);

        uint32_t imageIndex;
        vk::ResultValue<uint32_t> result = DeviceManager::GetDevice().AcquireNextImageKHR(m_ImageAvailableSemaphores[m_CurrentFrame]);

        Utils::CheckVkResult(result.result);

        imageIndex = result.value;

    DeviceManager::GetDevice().ResetFences(m_InFlightFences[m_CurrentFrame]);
        m_CommandBuffers[m_CurrentFrame].reset();

        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_CommandBuffers[m_CurrentFrame])
            .setWaitSemaphores(m_ImageAvailableSemaphores[m_CurrentFrame])
            .setWaitDstStageMask(dstStageMask)
            .setSignalSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame]);

        TRY_CATCH_BEGIN()

        DeviceManager::GetDevice().GetGraphicsQueue().submit(submitInfo, m_InFlightFences[m_CurrentFrame]);

        TRY_CATCH_END()

        vk::SwapchainKHR swapchain = DeviceManager::GetDevice().GetSwapchain()->GetVkSwapchain();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame])
            .setSwapchains(swapchain)
            .setImageIndices(imageIndex)
            .setPResults(nullptr);

        Utils::CheckVkResult(DeviceManager::GetDevice().GetPresentQueue().presentKHR(presentInfo));

        m_CurrentFrame = (m_CurrentFrame + 1) % DeviceManager::GetDevice().GetSwapchain()->GetNumberOfSwapBuffers();
    }

    void SampleApplication::RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex)
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

            commandBuffer.bindIndexBuffer(m_IndexBuffer.GetVkBuffer(), 0, vk::IndexType::eUint32);

            // TODO fix the bugged creationg of VkBuffers
            commandBuffer.bindVertexBuffers(0, {m_VertexBuffer.GetVkBuffer()}, {0});

            commandBuffer.drawIndexed(m_CubeIndices.size(), 1, 0, 0, 0);

            commandBuffer.endRenderPass();
        }

        commandBuffer.end();

        TRY_CATCH_END()
    }

    bool SampleApplication::OnMousePress(MouseButtonEvent& event)
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

    bool SampleApplication::OnMouseMoved(MouseMovedEvent& event)
    {
        // ImGui_ImplGlfw_CursorPosCallback(m_AppWindow->GetGLFWWindow(), event.GetPos().x, event.GetPos().y);

        const glm::ivec2 diff = m_MouseState.m_LastPosition - event.GetPos();
        LOGF(Application, Info, "Mouse last position X: %d, Y: %d", m_MouseState.m_LastPosition.x,
             m_MouseState.m_LastPosition.y)
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

    bool SampleApplication::OnMouseScrolled(MouseScrolledEvent& event)
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

    bool SampleApplication::OnMouseRelease(MouseButtonEvent& event)
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

    bool SampleApplication::OnKeyPressed(KeyPressedEvent& event)
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

    bool SampleApplication::OnKeyReleased(KeyReleasedEvent& event)
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

    bool SampleApplication::OnWindowResize(WindowResizedEvent& event)
    {
        LOG(Application, Info, "Window resized")
        return true;
    }

    void SampleApplication::Shutdown()
    {
        for (auto& buffer : m_MatBuffers)
        {
            buffer.Destroy();
        }

        m_VertexBuffer.Destroy();

        m_DescriptorBuilder.Clear();
    }
} // namespace VkCore
