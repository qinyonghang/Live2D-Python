#include "Live2DModel.h"

#include <GL/glew.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <random>
#include <unordered_map>

#include "CubismFramework.hpp"
#include "GL/glew.h"
#include "LApp/LAppAllocator.hpp"
#include "LApp/LAppDefine.hpp"
#include "LApp/LAppModel.hpp"
#include "LApp/LAppPal.hpp"
#include "LApp/LAppView.hpp"
#include "LApp/TouchManager.hpp"
#include "LAppDefine.hpp"
#include "LAppModel.hpp"
#include "LAppPal.hpp"
#include "Live2DModel.h"
#include "Live2DRegister.h"
// #include "Live2DView.h"
#include "QException.h"
#include "QLog.h"

#ifdef WIN32
#include <GL/wglew.h>
#endif

using namespace Live2D::Cubism::Framework;

namespace {
struct ModelImpl : public QObject {
    size_t width;
    size_t height;
    std::shared_ptr<LAppModel> model;
    std::shared_ptr<LAppView> view;
    std::shared_ptr<CubismMatrix44> view_matrix;
    // std::shared_ptr<Live2D::View> view;
    // std::shared_ptr<CubismMatrix44> deviceToScreen;
};

};  // namespace

namespace Live2D {

int32_t Model::init(std::string model_path, size_t width, size_t height) {
    qCMInfo("model_path={}, width={}, height={}", model_path, width, height);
    int32_t result{0};

    do {
        result = glewInit();
        if (result != 0) {
            qCMCCritical("glewInit return {}", result);
            break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glViewport(0, 0, width, height);

        auto impl = std::make_shared<ModelImpl>();

        std::filesystem::path path(model_path);
        if (!std::filesystem::exists(path)) {
            qCError("model_path={} does not exist", path.string());
            result = -1;
            break;
        }

        auto view = std::make_shared<LAppView>();
        view->Initialize(width, height);

        auto _viewMatrix = std::make_shared<CubismMatrix44>();
        auto model = std::make_shared<LAppModel>();

        model->LoadAssets((path.parent_path().string() + "/").c_str(),
                          path.filename().string().c_str());

        // float clearColor[3] = {1.0f, 1.0f, 1.0f};
        // view->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);

        //default proj
        CubismMatrix44 projection;

        LAppPal::UpdateTime();

        view->InitializeSprite(width, height);

        impl->model = model;
        impl->view = view;
        impl->view_matrix = _viewMatrix;
        impl->width = width;
        impl->height = height;

        __impl = impl;
    } while (0);

    return result;
}

void Model::set_background(std::string background) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    // impl->view->set_background(background);
    qCMInfo("background={}", background);
}

void Model::draw(size_t width, size_t height) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    if ((impl->width != width || impl->height != height) && width > 0 && height > 0) {
        impl->view->Initialize(width, height);
        impl->view->ResizeSprite(width, height);
        impl->width = width;
        impl->height = height;

        glViewport(0, 0, width, height);
    }

    LAppPal::UpdateTime();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    impl->view->Render(impl->width, impl->height, impl->view_matrix.get(), impl->model.get(),
                       impl->view.get());
}

void Model::set_dragging(float x, float y) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    impl->view->OnTouchesMoved(x, y, impl->model.get());
}

bool Model::is_hit(std::string const& area, float x, float y) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    x = impl->view->_deviceToScreen->TransformX(x);
    y = impl->view->_deviceToScreen->TransformY(y);

    return impl->model->HitTest(area.c_str(), x, y);
}

std::string Model::hit_area(float x, float y) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    std::string result;

    x = impl->view->_deviceToScreen->TransformX(x);
    y = impl->view->_deviceToScreen->TransformY(y);

    if (impl->model->GetOpacity() > 0) {
        for (csmInt32 i = 0; i < impl->model->_modelSetting->GetHitAreasCount(); i++) {
            if (impl->model->IsHit(impl->model->_modelSetting->GetHitAreaId(i), x, y)) {
                result = impl->model->_modelSetting->GetHitAreaName(i);
                break;
            }
        }
    }

    return result;
}

std::vector<std::string> Model::expression_ids() const {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    std::vector<std::string> result;
    for (auto it = impl->model->_expressions.Begin(); it != impl->model->_expressions.End(); ++it) {
        result.emplace_back(it->First.GetRawString());
    }

    return result;
}

std::vector<std::string> Model::motion_ids() const {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    std::vector<std::string> result;
    for (auto it = impl->model->_motions.Begin(); it != impl->model->_motions.End(); ++it) {
        result.emplace_back(it->First.GetRawString());
    }

    return result;
}

void Model::set_expression(std::string const& id) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    qCTrace("set_expression: id={}", id);
    impl->model->SetExpression(id.c_str());
}

void Model::set_motion(std::string const& id, std::string sound_file, int32_t priority) {
    auto impl = std::dynamic_pointer_cast<ModelImpl>(__impl);
    if (impl == nullptr) {
        QCMTHROW_EXCEPTION("impl is nullptr");
    }

    qCMTrace("set_motion: id={} sound_file={} {}", id, sound_file, priority);
    impl->model->StartMotion(id, sound_file, priority, nullptr);
}

};  // namespace Live2D
