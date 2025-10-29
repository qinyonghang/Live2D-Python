#ifndef LIVE2D_REGISTER_H
#define LIVE2D_REGISTER_H

#include "qlib/memory.h"

namespace Live2D {

using namespace qlib;

#ifdef WIN32
#ifdef LIVE2D_EXPORTS
#define LIVE2D_API __declspec(dllexport)
#else
#define LIVE2D_API __declspec(dllimport)
#endif
#else
#define LIVE2D_API
#endif

class LIVE2D_API Register : public object {
public:
    Register() {
        int32_t result{init()};
        throw_if(result != 0, "exception");
    }
    static void set_log_level(size_t level);

protected:
    int32_t init();
};

};  // namespace Live2D

#endif
