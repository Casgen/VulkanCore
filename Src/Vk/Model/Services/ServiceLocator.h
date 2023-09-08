#pragma once

#include <memory>


#include "Allocator/NullAllocatorService.h"

namespace VkCore
{
    class IAllocatorService;

    class ServiceLocator
    {

      public:
        static void Initialize();

        /**
         * @brief obtains a AllocatorService object.
         *
         */

        static IAllocatorService& GetAllocatorService();

        /**
         * @brief Sets Allocator service to the service locator making it available globally. If the Service is a
         * nullptr, it will by default provide a NullAllocatorService!
         * @param service - pointer to an object implementing IAllocatorService.
         * @return - valid object implementing IAllocatorService or a NullAllocatorService if a service was not provided!
         */
        static void ProvideAllocatorService(IAllocatorService* service);

      private:
        inline static IAllocatorService* m_AllocatorService;
        inline static NullAllocatorService m_NullAllocatorService;
    };
} // namespace VkCore
