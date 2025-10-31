#ifndef LIVE2D_REGISTER_H
#define LIVE2D_REGISTER_H

#include <chrono>
#include <iostream>
#include <mutex>

#include "qlib/logger.h"

#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif

#include "CubismFramework.hpp"
#include "ICubismAllocator.hpp"

#include "exports.h"

namespace pylive2d {

using namespace qlib;
namespace Csm = Live2D::Cubism::Framework;

class LIVE2D_API Register : public object {
public:
    using logger_type = typename logger::value<string_view_t, logger::ansicolor_stdout_sink_mt>;

    static Register& instance() {
        static Register instance;
        return instance;
    }

    ~Register() {
        CubismFramework::Dispose();
        _logger.trace("register deinited!");
    }

    void set_log_level(size_t level) {
        _option.LoggingLevel = static_cast<decltype(_option.LoggingLevel)>(level);
        _logger.sinks().set_level(static_cast<logger::level>(level));
    }

    logger_type& logger() { return _logger; }

protected:
    class Allocator : public Csm::ICubismAllocator {
        void* Allocate(const Csm::csmSizeType size) override { return malloc(size); }

        void Deallocate(void* memory) override { free(memory); }

        void* AllocateAligned(const Csm::csmSizeType size,
                              const Csm::csmUint32 alignment) override {
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

    Allocator _allocator;
    Csm::CubismFramework::Option _option;
    logger_type _logger;

    Register() : _logger("pylive2d", logger::ansicolor_stdout_sink_mt()) {
        size_t level = 2u;

        auto level_str = std::getenv("Live2D_LogLevel");
        if (level_str != nullptr) {
            char* end_ptr = nullptr;
            auto l = std::strtol(level_str, &end_ptr, 10);
            if (*end_ptr != '\0') {
                std::cout << "Live2D_LogLevel is not a number... " << level_str << std::endl;
            } else {
                level = l;
            }
        }
        _logger.sinks().set_level((logger::level)(level));

        _option.LogFunction = print;
        _option.LoggingLevel = decltype(_option.LoggingLevel)(level);

        CubismFramework::StartUp(&_allocator, &_option);
        CubismFramework::Initialize();

        _logger.trace("register inited!");
    }

    static void print(char const* message) { std::cout << message; }
};

ALWAYS_INLINE Register& default_register() {
    return Register::instance();
}

ALWAYS_INLINE Register::logger_type& default_logger() {
    return default_register().logger();
}

};  // namespace pylive2d

#endif
