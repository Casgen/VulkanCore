#pragma once

#include "Device.h"
#include "PhysicalDevice.h"
#include "vulkan/vulkan_handles.hpp"

namespace VkCore
{

    class DeviceManager
    {

      public:
        DeviceManager() = delete;
        DeviceManager(const DeviceManager& other) = delete;
        DeviceManager(DeviceManager&& other) = delete;
        DeviceManager operator=(DeviceManager&& other) = delete;
        DeviceManager operator=(const DeviceManager& other) = delete;

        static void Initialize(const vk::Instance& instance, const vk::SurfaceKHR& surface, const bool isMeshShadingEnabled = true)
        {
            m_PhysicalDevice = VkCore::PhysicalDevice(instance, surface, m_DeviceExtensions);
            m_Device = VkCore::Device(m_PhysicalDevice, m_DeviceExtensions, isMeshShadingEnabled);
        }

        static Device& GetDevice()
        {
            return m_Device;
        };

        static PhysicalDevice& GetPhysicalDevice()
        {
            return m_PhysicalDevice;
        };

        static std::vector<const char*> GetDeviceExtensions()
        {
            return m_DeviceExtensions;
        };

        static void AddDeviceExtension(const char* ext)
        {
            m_DeviceExtensions.emplace_back(ext);
        };

      private:
        inline static Device m_Device;
        inline static PhysicalDevice m_PhysicalDevice;

        inline static std::vector<const char*> m_DeviceExtensions;
    };
} // namespace VkCore
