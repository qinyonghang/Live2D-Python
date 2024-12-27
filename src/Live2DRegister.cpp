#include "Live2DRegister.h"

#include <atomic>
#include <iostream>

#include "CubismFramework.hpp"
#include "GL/glew.h"
#include "ICubismAllocator.hpp"
#include "Math/CubismMatrix44.hpp"
#include "QException.h"
#include "QLog.h"

using namespace Live2D::Cubism::Framework;

namespace {
constexpr auto default_level = 2u;
std::atomic_bool __register{false};

class Allocator : public Csm::ICubismAllocator {
    void* Allocate(const Csm::csmSizeType size) override { return malloc(size); }

    void Deallocate(void* memory) override { free(memory); }

    void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment) override {
        size_t offset, shift, alignedAddress;
        void* allocation;
        void** preamble;

        offset = alignment - 1 + sizeof(void*);

        allocation = Allocate(size + static_cast<Csm::csmSizeType>(offset));

        alignedAddress = reinterpret_cast<size_t>(allocation) + sizeof(void*);

        shift = alignedAddress % alignment;

        if (shift) {
            alignedAddress += (alignment - shift);
        }

        preamble = reinterpret_cast<void**>(alignedAddress);
        preamble[-1] = allocation;

        return reinterpret_cast<void*>(alignedAddress);
    }

    void DeallocateAligned(void* alignedMemory) override {
        void** preamble;

        preamble = static_cast<void**>(alignedMemory);

        Deallocate(preamble[-1]);
    }
};

struct RegisterImpl : public QObject {
    Allocator allocator;
    Csm::CubismFramework::Option option;
    std::shared_ptr<void> destroy;
};

void print(char const* message) {
    std::cout << message;
    // qLogger().log(spdlog::level::trace, message);
}
};  // namespace

namespace Live2D {
int32_t Register::init() {
    int32_t result{0};

    do {
        if (__register) {
            result = 0;
            break;
        }

        auto impl = std::make_shared<RegisterImpl>();

        impl->option.LogFunction = print;
        impl->option.LoggingLevel = static_cast<decltype(impl->option.LoggingLevel)>(default_level);
        qLogger().set_level(default_level);

        CubismFramework::StartUp(&impl->allocator, &impl->option);
        CubismFramework::Initialize();
        qCMDebug("Live2D Register End!");

        impl->destroy = std::shared_ptr<void>(nullptr, [](void*) {
            if (__register) {
                CubismFramework::Dispose();
                qMDebug("Live2D UnRegister End!");
                __register = false;
            }
        });

        __impl = impl;
        __register = true;
    } while (0);

    return result;
}

void Register::set_log_level(size_t level) {
    auto instance = QSingletonProductor<Register>::get_instance();
    auto impl = std::dynamic_pointer_cast<RegisterImpl>(instance.__impl);
    if (impl != nullptr) {
        qLogger().set_level(level);
        impl->option.LoggingLevel = static_cast<decltype(impl->option.LoggingLevel)>(level);
        qMInfo("level={}", level);
    }
}
};  // namespace Live2D
