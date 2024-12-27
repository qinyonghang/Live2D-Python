#pragma once

#include "QException.h"
#include "QObject.h"

namespace Live2D {

#ifdef WIN32
#ifdef LIVE2D_EXPORTS
#define LIVE2D_API __declspec(dllexport)
#else
#define LIVE2D_API __declspec(dllimport)
#endif
#else
#define LIVE2D_API
#endif

class LIVE2D_API Register : public QObject {
public:
    Register(QObject* parent = nullptr) : QObject(parent) {
        int32_t result{init()};
        if (result != 0) {
            QCMTHROW_EXCEPTION("init return {}...", result);
        }
    }
    static void set_log_level(size_t level);

protected:
    std::shared_ptr<QObject> __impl;

    int32_t init();
};

};  // namespace Live2D
