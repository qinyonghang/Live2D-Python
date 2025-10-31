#ifndef LIVE2DMODEL_H
#define LIVE2DMODEL_H

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif

#include "CubismFramework.hpp"
#include "LApp/LAppAllocator.hpp"
#include "LApp/LAppDefine.hpp"
#include "LApp/LAppModel.hpp"
#include "LApp/LAppPal.hpp"
#include "LApp/LAppView.hpp"
#include "LApp/TouchManager.hpp"
#include "Register.h"
#include "exports.h"

namespace pylive2d {

using namespace qlib;
namespace Csm = Live2D::Cubism::Framework;

class LIVE2D_API Model final : public object {
public:
    enum : int32_t {
        OK = 0,
        ERR_MODEL_NOT_EXIST = -1,
        ERR_OPENGL_INIT = -2,
    };

    template <class String>
    Model(String&& model_path, size_t width, size_t height) : _register(default_register()) {
        int32_t result{_init_(std::forward<String>(model_path), width, height)};
        throw_if(result != 0, "exception");
    }

    void draw(size_t width, size_t height) { return _draw_(width, height); }

    void draw(size_t width, size_t height, GLuint fbo) {
        GLint old_fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        _draw_(width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, GLuint(old_fbo));
    }

    void set_dragging(float x, float y) { _view->OnTouchesMoved(x, y, _model.get()); }

    bool is_hit(std::string const& area, float x, float y) {
        x = _view->_deviceToScreen->TransformX(x);
        y = _view->_deviceToScreen->TransformY(y);

        return _model->HitTest(area.c_str(), x, y);
    }

    std::string hit_area(float x, float y) {
        std::string result;

        x = _view->_deviceToScreen->TransformX(x);
        y = _view->_deviceToScreen->TransformY(y);

        if (_model->GetOpacity() > 0) {
            for (csmInt32 i = 0; i < _model->_modelSetting->GetHitAreasCount(); i++) {
                if (_model->IsHit(_model->_modelSetting->GetHitAreaId(i), x, y)) {
                    result = _model->_modelSetting->GetHitAreaName(i);
                    break;
                }
            }
        }

        return result;
    }

    std::vector<std::string> expression_ids() const {
        std::vector<std::string> result;
        for (auto it = _model->_expressions.Begin(); it != _model->_expressions.End(); ++it) {
            result.emplace_back(it->First.GetRawString());
        }

        return result;
    }

    std::vector<std::string> motion_ids() const {
        std::vector<std::string> result;
        for (auto it = _model->_motions.Begin(); it != _model->_motions.End(); ++it) {
            result.emplace_back(it->First.GetRawString());
        }

        return result;
    }

    void set_expression(std::string const& id) { _model->SetExpression(id.c_str()); }

    void set_motion(std::string const& id, std::string sound_file = "", int32_t priority = 3) {
        _model->StartMotion(id, sound_file, priority, nullptr);
    }

protected:
    pylive2d::Register& _register;
    size_t _width;
    size_t _height;
    unique_ptr_t<LAppModel> _model;
    unique_ptr_t<LAppView> _view;
    unique_ptr_t<Csm::CubismMatrix44> _view_matrix;

    int32_t _init_(std::string const& path, size_t width, size_t height) {
        int32_t result{0};

        do {
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

            auto view = make_unique<LAppView>();
            view->Initialize(width, height);

            auto _viewMatrix = make_unique<CubismMatrix44>();
            auto model = make_unique<LAppModel>();

            auto dir = std::filesystem::path(path).parent_path().string() + "/";
            auto filename = std::filesystem::path(path).filename().string();
            model->LoadAssets(dir.c_str(), filename.c_str());

            LAppPal::UpdateTime();

            view->InitializeSprite(width, height);

            _model = move(model);
            _view = move(view);
            _view_matrix = move(_viewMatrix);
            _width = width;
            _height = height;
        } while (0);

        return result;
    }

    void _draw_(size_t width, size_t height) {
        if ((_width != width || _height != height) && width > 0 && height > 0) {
            _view->Initialize(width, height);
            _view->ResizeSprite(width, height);
            _width = width;
            _height = height;

            glViewport(0, 0, width, height);
        }

        LAppPal::UpdateTime();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);

        _view->Render(_width, _height, _view_matrix.get(), _model.get(), _view.get());
    }
};

}  // namespace pylive2d

#endif
