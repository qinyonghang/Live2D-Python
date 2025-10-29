#include "Live2DModel.h"

#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif

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
#include "Live2DRegister.h"

using namespace Live2D::Cubism::Framework;

namespace Live2D {

struct Model::Impl : public object {
    Live2D::Register _register;
    size_t width;
    size_t height;
    unique_ptr_t<LAppModel> model;
    unique_ptr_t<LAppView> view;
    unique_ptr_t<CubismMatrix44> view_matrix;
};

int32_t Model::init(std::string model_path, size_t width, size_t height) {
    int32_t result{0};

    do {
        std::filesystem::path path(model_path);
        if (!std::filesystem::exists(path)) {
            result = ERR_MODEL_NOT_EXIST;
            break;
        }

        result = glewInit();
        if (result != 0) {
            result = ERR_OPENGL_INIT;
            break;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        auto impl = std::make_shared<Impl>();

        auto view = make_unique<LAppView>();
        view->Initialize(width, height);

        auto _viewMatrix = make_unique<CubismMatrix44>();
        auto model = make_unique<LAppModel>();

        model->LoadAssets((path.parent_path().string() + "/").c_str(),
                          path.filename().string().c_str());

        LAppPal::UpdateTime();

        view->InitializeSprite(width, height);

        impl->model = move(model);
        impl->view = move(view);
        impl->view_matrix = move(_viewMatrix);
        impl->width = width;
        impl->height = height;

        __impl = std::move(impl);
    } while (0);

    return result;
}

// void Model::set_background(std::string background) {
//     auto impl = __impl;

//     // impl->view->set_background(background);
// }

void Model::draw(size_t width, size_t height) {
    auto impl = __impl.get();

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
    auto impl = __impl.get();

    impl->view->OnTouchesMoved(x, y, impl->model.get());
}

bool Model::is_hit(std::string const& area, float x, float y) {
    auto impl = __impl.get();

    x = impl->view->_deviceToScreen->TransformX(x);
    y = impl->view->_deviceToScreen->TransformY(y);

    return impl->model->HitTest(area.c_str(), x, y);
}

std::string Model::hit_area(float x, float y) {
    auto impl = __impl.get();

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
    auto impl = __impl.get();

    std::vector<std::string> result;
    for (auto it = impl->model->_expressions.Begin(); it != impl->model->_expressions.End(); ++it) {
        result.emplace_back(it->First.GetRawString());
    }

    return result;
}

std::vector<std::string> Model::motion_ids() const {
    auto impl = __impl.get();

    std::vector<std::string> result;
    for (auto it = impl->model->_motions.Begin(); it != impl->model->_motions.End(); ++it) {
        result.emplace_back(it->First.GetRawString());
    }

    return result;
}

void Model::set_expression(std::string const& id) {
    auto impl = __impl.get();

    impl->model->SetExpression(id.c_str());
}

void Model::set_motion(std::string const& id, std::string sound_file, int32_t priority) {
    auto impl = __impl.get();

    impl->model->StartMotion(id, sound_file, priority, nullptr);
}

};  // namespace Live2D
