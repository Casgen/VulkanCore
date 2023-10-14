
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

    BaseApplication::BaseApplication(const uint32_t width, const uint32_t height, const std::string& title)
        : m_WinWidth(width), m_WinHeight(height), m_Title(title)
    {
        Logger::SetSeverityFilter(ESeverity::Verbose);
    }

    BaseApplication::~BaseApplication()
    {
    }

    void BaseApplication::PreInitVulkan()
    {
    }

    void BaseApplication::InitVulkan()
    {
        // The window has to be created before the instance in order to obtain the required extensions
        // for the VkInstance!
        CreateWindow();
        CreateInstance();

        m_Window->CreateSurface(m_Instance);

        CreateDevices();

        CreateServices();

        m_Device->InitSwapChain(*m_PhysicalDevice, m_Window->GetSurface(), m_WinWidth, m_WinHeight);

        m_DescriptorBuilder = DescriptorBuilder(*m_Device);
    }

    void BaseApplication::PostInitVulkan()
    {
        // Render Pass
        m_RenderPass = VkCore::SwapchainRenderPass(*m_Device);

        CreateVertexBuffer();
        CreatePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void BaseApplication::CreateVertexBuffer()
    {
        // Buffer
        float vertices[15] = {
            -0.5f, -0.5f, 1.f, 0.f, 0.f, .0f, .5f, 0.f, 1.f, 0.f, .5f, -0.5f, 0.f, 0.f, 1.f,
        };

        m_VertexBuffer = Buffer(vk::BufferUsageFlagBits::eVertexBuffer);
        m_VertexBuffer.InitializeOnGpu(vertices, 60);


        m_AttributeBuilder.PushAttribute<float>(2).PushAttribute<float>(3).SetBinding(0);
    }

    void BaseApplication::CreatePipeline()
    {
        const std::vector<ShaderData> shaders = ShaderLoader::LoadClassicShaders("VulkanCore/Res/Shaders/");

        m_PipelineBuilder = GraphicsPipelineBuilder(*m_Device);

        m_Pipeline = m_PipelineBuilder.BindShaderModules(shaders)
                         .BindRenderPass(m_RenderPass.GetVkRenderPass())
                         .AddViewport(glm::uvec4(0, 0, m_WinWidth, m_WinHeight))
                         .AddDisabledBlendAttachment()
                         .BindVertexAttributes(m_AttributeBuilder)
                         .SetPrimitiveAssembly(vk::PrimitiveTopology::eTriangleList)
                         .Build();
    }

    void BaseApplication::CreateFramebuffers()
    {
        std::vector<vk::ImageView> imageViews = m_Device->GetSwapchain()->GetImageViews();

        TRY_CATCH_BEGIN()

        for (const vk::ImageView& view : imageViews)
        {

            vk::FramebufferCreateInfo createInfo{};
            createInfo.setWidth(m_WinWidth)
                .setHeight(m_WinHeight)
                .setLayers(1)
                .setRenderPass(m_RenderPass.GetVkRenderPass())
                .setAttachments(view);

            m_SwapchainFramebuffers.emplace_back(m_Device->CreateFrameBuffer(createInfo));
        }

        TRY_CATCH_END()
    }

    void BaseApplication::CreateCommandPool()
    {

        vk::CommandPoolCreateInfo createInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                             m_PhysicalDevice->GetQueueFamilyIndices().m_GraphicsFamily.value()};

        m_CommandPool = m_Device->CreateCommandPool(createInfo);
    }

    void BaseApplication::CreateCommandBuffer()
    {

        vk::CommandBufferAllocateInfo allocateInfo{};

        allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandPool(m_CommandPool)
            .setCommandBufferCount(m_Device->GetSwapchain()->GetImageViews().size());

        TRY_CATCH_BEGIN()
        m_CommandBuffers = m_Device->AllocateCommandBuffers(allocateInfo);
        TRY_CATCH_END()
    }

    void BaseApplication::CreateSyncObjects()
    {
        vk::FenceCreateInfo fenceCreateInfo{vk::FenceCreateFlagBits::eSignaled};

        vk::SemaphoreCreateInfo imageAvailableCreateInfo{};
        vk::SemaphoreCreateInfo renderFinishedCreateInfo{};

        TRY_CATCH_BEGIN()

        for (int i = 0; i < m_Device->GetSwapchain()->GetNumberOfSwapBuffers(); i++)
        {
            m_ImageAvailableSemaphores.emplace_back(m_Device->CreateSemaphore(imageAvailableCreateInfo));
            m_RenderFinishedSemaphores.emplace_back(m_Device->CreateSemaphore(renderFinishedCreateInfo));

            m_InFlightFences.emplace_back(m_Device->CreateFence(fenceCreateInfo));
        }

        TRY_CATCH_END()
    }

    void BaseApplication::Run()
    {
        PreInitVulkan();
        InitVulkan();
        PostInitVulkan();
        Loop();
        Shutdown();
    };

    void BaseApplication::Loop()
    {
        while (!m_Window->ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }
    }

    void BaseApplication::DrawFrame()
    {
        m_Device->WaitForFences(m_InFlightFences[m_CurrentFrame], false);

        uint32_t imageIndex;
        vk::ResultValue<uint32_t> result = m_Device->AcquireNextImageKHR(m_ImageAvailableSemaphores[m_CurrentFrame]);

        Utils::CheckVkResult(result.result);

        imageIndex = result.value;

        m_Device->ResetFences(m_InFlightFences[m_CurrentFrame]);
        m_CommandBuffers[m_CurrentFrame].reset();

        RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

        vk::SubmitInfo submitInfo{};
        submitInfo.setCommandBuffers(m_CommandBuffers[m_CurrentFrame])
            .setWaitSemaphores(m_ImageAvailableSemaphores[m_CurrentFrame])
            .setWaitDstStageMask(dstStageMask)
            .setSignalSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame]);

        TRY_CATCH_BEGIN()

        m_Device->GetGraphicsQueue().submit(submitInfo, m_InFlightFences[m_CurrentFrame]);

        TRY_CATCH_END()

        vk::SwapchainKHR swapchain = m_Device->GetSwapchain()->GetVkSwapchain();

        vk::PresentInfoKHR presentInfo{};
        presentInfo.setWaitSemaphores(m_RenderFinishedSemaphores[m_CurrentFrame])
            .setSwapchains(swapchain)
            .setImageIndices(imageIndex)
            .setPResults(nullptr);

        Utils::CheckVkResult(m_Device->GetPresentQueue().presentKHR(presentInfo));

        m_CurrentFrame = (m_CurrentFrame + 1) % m_Device->GetSwapchain()->GetNumberOfSwapBuffers();
    }

    void BaseApplication::RecordCommandBuffer(const vk::CommandBuffer& commandBuffer, const uint32_t imageIndex)
    {
        vk::CommandBufferBeginInfo cmdBufferBeginInfo{};

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

    void BaseApplication::Shutdown()
    {

        m_Device->Destroy();
        m_Instance.destroy();
    }

    void BaseApplication::CreateWindow()
    {
        VkCore::WindowProps windowProps{m_Title, m_WinWidth, m_WinHeight};

        m_Window = std::make_unique<VkCore::Window>(m_Instance, windowProps);
    }

    void BaseApplication::CreateInstance()
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

    void BaseApplication::CreateDevices()
    {
        m_PhysicalDevice = new VkCore::PhysicalDevice(m_Instance, m_Window->GetSurface(), m_DeviceExtensions);
        m_Device = new VkCore::Device(*m_PhysicalDevice, m_DeviceExtensions);
    }

    void BaseApplication::CreateServices()
    {
        VmaAllocatorService* allocationService = new VmaAllocatorService(*m_Device, *m_PhysicalDevice, m_Instance);
        ServiceLocator::ProvideAllocatorService(allocationService);
    }

} // namespace VkCore
