#include "ServiceLocator.h"

namespace VkCore
{
    void ServiceLocator::Initialize()
    {
        m_AllocatorService = &m_NullAllocatorService;
    }

    IAllocatorService& ServiceLocator::GetAllocatorService()
    {
        return *m_AllocatorService;
    }

    void ServiceLocator::ProvideAllocatorService(IAllocatorService* service)
    {
        if (service != nullptr)
        {
            m_AllocatorService = service;
            return;
        }
        m_AllocatorService = &m_NullAllocatorService;
    }
} // namespace VkCore
