#include "Live2DRegister.h"

#include <atomic>
#include <iostream>
#include <memory>

#include "CubismFramework.hpp"
#include "GL/glew.h"
#include "ICubismAllocator.hpp"
#include "Math/CubismMatrix44.hpp"

using namespace Live2D::Cubism::Framework;

namespace Live2D {
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

Allocator allocator;
Csm::CubismFramework::Option option;
std::shared_ptr<void> destroy;

void print(char const* message) {
    std::cout << message;
}

int32_t Register::init() {
    int32_t result{0};

    do {
        if (__register) {
            result = 0;
            break;
        }

        option.LogFunction = print;
        option.LoggingLevel = static_cast<decltype(option.LoggingLevel)>(default_level);

        CubismFramework::StartUp(&allocator, &option);
        CubismFramework::Initialize();

        destroy = std::shared_ptr<void>(nullptr, [](void*) {
            if (__register) {
                CubismFramework::Dispose();
                __register = false;
            }
        });

        __register = true;
    } while (0);

    return result;
}

void Register::set_log_level(size_t level) {
    option.LoggingLevel = static_cast<decltype(option.LoggingLevel)>(level);
}
};  // namespace Live2D
