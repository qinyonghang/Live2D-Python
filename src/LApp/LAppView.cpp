﻿/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppView.hpp"

#include <math.h>

#include <string>

#include "LAppPal.hpp"
// #include "LAppDelegate.hpp"
// #include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "LAppModel.hpp"
#include "LAppSprite.hpp"
#include "LAppTextureManager.hpp"
#include "TouchManager.hpp"

using namespace std;
using namespace LAppDefine;

LAppView::LAppView()
        : _programId(0),
          // _back(NULL),
          // _gear(NULL),
          // _power(NULL),
          _renderSprite(NULL), _renderTarget(SelectTarget_None) {
    _clearColor[0] = 1.0f;
    _clearColor[1] = 1.0f;
    _clearColor[2] = 1.0f;
    _clearColor[3] = 0.0f;

    // タッチ関係のイベント管理
    _touchManager = new TouchManager();

    // デバイス座標からスクリーン座標に変換するための
    _deviceToScreen = new CubismMatrix44();

    // 画面の表示の拡大縮小や移動の変換を行う行列
    _viewMatrix = new CubismViewMatrix();
}

LAppView::~LAppView() {
    _renderBuffer.DestroyOffscreenSurface();
    delete _renderSprite;
    delete _viewMatrix;
    delete _deviceToScreen;
    delete _touchManager;
    // delete _back;
    // delete _gear;
    // delete _power;
}

void LAppView::Initialize(int width, int height) {
    // 縦サイズを基準とする
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    float left = -ratio;
    float right = ratio;
    float bottom = ViewLogicalLeft;
    float top = ViewLogicalRight;

    _viewMatrix->SetScreenRect(
        left, right, bottom,
        top);  // デバイスに対応する画面の範囲。 Xの左端, Xの右端, Yの下端, Yの上端
    _viewMatrix->Scale(ViewScale, ViewScale);

    _deviceToScreen->LoadIdentity();  // サイズが変わった際などリセット必須
    if (width > height) {
        float screenW = fabsf(right - left);
        _deviceToScreen->ScaleRelative(screenW / width, -screenW / width);
    } else {
        float screenH = fabsf(top - bottom);
        _deviceToScreen->ScaleRelative(screenH / height, -screenH / height);
    }
    _deviceToScreen->TranslateRelative(-width * 0.5f, -height * 0.5f);

    // 表示範囲の設定
    _viewMatrix->SetMaxScale(ViewMaxScale);  // 限界拡大率
    _viewMatrix->SetMinScale(ViewMinScale);  // 限界縮小率

    // 表示できる最大範囲
    _viewMatrix->SetMaxScreenRect(ViewLogicalMaxLeft, ViewLogicalMaxRight, ViewLogicalMaxBottom,
                                  ViewLogicalMaxTop);
}

void LAppView::Render(
    int width, int height, CubismMatrix44* viewMatrix, LAppModel* model, LAppView* view) {
    // 画面サイズを取得する
    // _back->SetWindowSize(maxWidth, maxHeight);
    // _gear->SetWindowSize(maxWidth, maxHeight);
    // _power->SetWindowSize(maxWidth, maxHeight);

    // _back->Render();
    // _gear->Render();
    // _power->Render();

    for (int i = 0; i < 16; i++) {
        viewMatrix->GetArray()[i] = _viewMatrix->GetArray()[i];
    }

    CubismMatrix44 projection;

    if (model->GetModel()->GetCanvasWidth() > 1.0f && width < height) {
        // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
        model->GetModelMatrix()->SetWidth(2.0f);
        projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
    } else {
        projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
    }

    // 必要があればここで乗算
    if (viewMatrix != NULL) {
        projection.MultiplyByMatrix(viewMatrix);
    }

    // モデル1体描画前コール
    view->PreModelDraw(*model);

    model->Update();
    model->Draw(projection);  ///< 参照渡しなのでprojectionは変質する

    // モデル1体描画後コール
    view->PostModelDraw(*model);

    // // 各モデルが持つ描画ターゲットをテクスチャとする場合
    // if (_renderTarget == SelectTarget_ModelFrameBuffer && _renderSprite)
    // {
    //     const GLfloat uvVertex[] =
    //     {
    //         1.0f, 1.0f,
    //         0.0f, 1.0f,
    //         0.0f, 0.0f,
    //         1.0f, 0.0f,
    //     };

    //     for (csmUint32 i = 0; i < Live2DManager->GetModelNum(); i++)
    //     {
    //         LAppModel* model = Live2DManager->GetModel(i);
    //         float alpha = i < 1 ? 1.0f : model->GetOpacity(); // 片方のみ不透明度を取得できるようにする
    //         _renderSprite->SetColor(1.0f, 1.0f, 1.0f, alpha);

    //         if (model)
    //         {
    //             _renderSprite->SetWindowSize(maxWidth, maxHeight);
    //             _renderSprite->RenderImmidiate( model->GetRenderBuffer().GetColorBuffer(), uvVertex);
    //         }
    //     }
    // }
}

bool CheckShader(GLuint shaderId) {
    GLint status;
    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
        glGetShaderInfoLog(shaderId, logLength, &logLength, log);
        CubismLogError("Shader compile log: %s", log);
        CSM_FREE(log);
    }

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glDeleteShader(shaderId);
        return false;
    }

    return true;
}

GLuint CreateShader() {
    //バーテックスシェーダのコンパイル
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader = "#version 120\n"
                               "attribute vec3 position;"
                               "attribute vec2 uv;"
                               "varying vec2 vuv;"
                               "void main(void){"
                               "    gl_Position = vec4(position, 1.0);"
                               "    vuv = uv;"
                               "}";
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);
    if (!CheckShader(vertexShaderId)) {
        return 0;
    }

    //フラグメントシェーダのコンパイル
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader = "#version 120\n"
                                 "varying vec2 vuv;"
                                 "uniform sampler2D texture;"
                                 "uniform vec4 baseColor;"
                                 "void main(void){"
                                 "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
                                 "}";
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);
    if (!CheckShader(fragmentShaderId)) {
        return 0;
    }

    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

void LAppView::InitializeSprite(int width, int height) {
    _programId = CreateShader();

    const string resourcesPath = ResourcesPath;

    // string imageName = BackImageName;
    // LAppTextureManager::TextureInfo* backgroundTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName);

    // float x = width * 0.5f;
    // float y = height * 0.5f;
    // float fWidth = static_cast<float>(backgroundTexture->width * 2.0f);
    // float fHeight = static_cast<float>(height * 0.95f);
    // _back = new LAppSprite(x, y, fWidth, fHeight, backgroundTexture->id, _programId);

    // imageName = GearImageName;
    // LAppTextureManager::TextureInfo* gearTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName);

    // x = static_cast<float>(width - gearTexture->width * 0.5f);
    // y = static_cast<float>(height - gearTexture->height * 0.5f);
    // fWidth = static_cast<float>(gearTexture->width);
    // fHeight = static_cast<float>(gearTexture->height);
    // _gear = new LAppSprite(x, y, fWidth, fHeight, gearTexture->id, _programId);

    // imageName = PowerImageName;
    // LAppTextureManager::TextureInfo* powerTexture = textureManager->CreateTextureFromPngFile(resourcesPath + imageName);

    // x = static_cast<float>(width - powerTexture->width * 0.5f);
    // y = static_cast<float>(powerTexture->height * 0.5f);
    // fWidth = static_cast<float>(powerTexture->width);
    // fHeight = static_cast<float>(powerTexture->height);
    // _power = new LAppSprite(x, y, fWidth, fHeight, powerTexture->id, _programId);

    {
        // 画面全体を覆うサイズ
        auto x = width * 0.5f;
        auto y = height * 0.5f;
        _renderSprite = new LAppSprite(x, y, static_cast<float>(width), static_cast<float>(height),
                                       0, _programId);
    }
}

void LAppView::OnTouchesBegan(float px, float py) const {
    _touchManager->TouchesBegan(px, py);
}

void LAppView::OnTouchesMoved(float px, float py, LAppModel* model) const {
    float viewX = this->TransformViewX(_touchManager->GetX());
    float viewY = this->TransformViewY(_touchManager->GetY());

    _touchManager->TouchesMoved(px, py);

    model->SetDragging(viewX, viewY);
}

void LAppView::OnTouchesEnded(float px, float py, LAppModel* model) const {
    // タッチ終了
    model->SetDragging(0.0, 0.0);
    {
        // シングルタップ
        float x =
            _deviceToScreen->TransformX(_touchManager->GetX());  // 論理座標変換した座標を取得。
        float y =
            _deviceToScreen->TransformY(_touchManager->GetY());  // 論理座標変換した座標を取得。
        if (DebugTouchLogEnable) {
            LAppPal::PrintLogLn("[APP]touchesEnded x:%.2f y:%.2f", x, y);
        }

        if (model->HitTest(HitAreaNameHead, x, y)) {
            model->SetRandomExpression();
        } else if (model->HitTest(HitAreaNameBody, x, y)) {
            model->StartRandomMotion(MotionGroupTapBody, PriorityNormal, nullptr);
        }

        // // 歯車にタップしたか
        // if (_gear->IsHit(px, py))
        // {
        //     live2DManager->NextScene();
        // }

        // // 電源ボタンにタップしたか
        // if (_power->IsHit(px, py))
        // {
        //     LAppDelegate::GetInstance()->AppEnd();
        // }
    }
}

float LAppView::TransformViewX(float deviceX) const {
    float screenX = _deviceToScreen->TransformX(deviceX);  // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformX(screenX);         // 拡大、縮小、移動後の値。
}

float LAppView::TransformViewY(float deviceY) const {
    float screenY = _deviceToScreen->TransformY(deviceY);  // 論理座標変換した座標を取得。
    return _viewMatrix->InvertTransformY(screenY);         // 拡大、縮小、移動後の値。
}

float LAppView::TransformScreenX(float deviceX) const {
    return _deviceToScreen->TransformX(deviceX);
}

float LAppView::TransformScreenY(float deviceY) const {
    return _deviceToScreen->TransformY(deviceY);
}

void LAppView::PreModelDraw(LAppModel& refModel) {
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_OpenGLES2* useTarget = NULL;

    // if (_renderTarget != SelectTarget_None)
    // {// 別のレンダリングターゲットへ向けて描画する場合

    //     // 使用するターゲット
    //     useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

    //     if (!useTarget->IsValid())
    //     {// 描画ターゲット内部未作成の場合はここで作成
    //         int width, height;
    //         glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);
    //         if (width != 0 && height != 0)
    //         {
    //             // モデル描画キャンバス
    //             useTarget->CreateOffscreenSurface(static_cast<csmUint32>(width), static_cast<csmUint32>(height));
    //         }
    //     }

    //     // レンダリング開始
    //     useTarget->BeginDraw();
    //     useTarget->Clear(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]); // 背景クリアカラー
    // }
}

void LAppView::PostModelDraw(LAppModel& refModel) {
    // 別のレンダリングターゲットへ向けて描画する場合の使用するフレームバッファ
    Csm::Rendering::CubismOffscreenSurface_OpenGLES2* useTarget = NULL;

    // if (_renderTarget != SelectTarget_None)
    // {// 別のレンダリングターゲットへ向けて描画する場合

    //     // 使用するターゲット
    //     useTarget = (_renderTarget == SelectTarget_ViewFrameBuffer) ? &_renderBuffer : &refModel.GetRenderBuffer();

    //     // レンダリング終了
    //     useTarget->EndDraw();

    //     // LAppViewの持つフレームバッファを使うなら、スプライトへの描画はここ
    //     if (_renderTarget == SelectTarget_ViewFrameBuffer && _renderSprite)
    //     {
    //         const GLfloat uvVertex[] =
    //         {
    //             1.0f, 1.0f,
    //             0.0f, 1.0f,
    //             0.0f, 0.0f,
    //             1.0f, 0.0f,
    //         };

    //         _renderSprite->SetColor(1.0f, 1.0f, 1.0f, GetSpriteAlpha(0));

    //         // 画面サイズを取得する
    //         int maxWidth, maxHeight;
    //         glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &maxWidth, &maxHeight);
    //         _renderSprite->SetWindowSize(maxWidth, maxHeight);

    //         _renderSprite->RenderImmidiate(useTarget->GetColorBuffer(), uvVertex);
    //     }
    // }
}

void LAppView::SwitchRenderingTarget(SelectTarget targetType) {
    _renderTarget = targetType;
}

void LAppView::SetRenderTargetClearColor(float r, float g, float b) {
    _clearColor[0] = r;
    _clearColor[1] = g;
    _clearColor[2] = b;
}

float LAppView::GetSpriteAlpha(int assign) const {
    // assignの数値に応じて適当に決定
    float alpha = 0.25f + static_cast<float>(assign) * 0.5f;  // サンプルとしてαに適当な差をつける
    if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    if (alpha < 0.1f) {
        alpha = 0.1f;
    }

    return alpha;
}

void LAppView::ResizeSprite(int width, int height) {
    // LAppTextureManager* textureManager = LAppDelegate::GetInstance()->GetTextureManager();
    // if (!textureManager)
    // {
    //     return;
    // }

    // // 描画領域サイズ
    // int width, height;
    // glfwGetWindowSize(LAppDelegate::GetInstance()->GetWindow(), &width, &height);

    float x = 0.0f;
    float y = 0.0f;
    float fWidth = 0.0f;
    float fHeight = 0.0f;

    // if (_back)
    // {
    //     GLuint id = _back->GetTextureId();
    //     LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
    //     if (texInfo)
    //     {
    //         x = width * 0.5f;
    //         y = height * 0.5f;
    //         fWidth = static_cast<float>(texInfo->width * 2);
    //         fHeight = static_cast<float>(height) * 0.95f;
    //         _back->ResetRect(x, y, fWidth, fHeight);
    //     }
    // }

    // if (_power)
    // {
    //     GLuint id = _power->GetTextureId();
    //     LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
    //     if (texInfo)
    //     {
    //         x = static_cast<float>(width - texInfo->width * 0.5f);
    //         y = static_cast<float>(texInfo->height * 0.5f);
    //         fWidth = static_cast<float>(texInfo->width);
    //         fHeight = static_cast<float>(texInfo->height);
    //         _power->ResetRect(x, y, fWidth, fHeight);
    //     }
    // }

    // if (_gear)
    // {
    //     GLuint id = _gear->GetTextureId();
    //     LAppTextureManager::TextureInfo* texInfo = textureManager->GetTextureInfoById(id);
    //     if (texInfo)
    //     {
    //         x = static_cast<float>(width - texInfo->width * 0.5f);
    //         y = static_cast<float>(height - texInfo->height * 0.5f);
    //         fWidth = static_cast<float>(texInfo->width);
    //         fHeight = static_cast<float>(texInfo->height);
    //         _gear->ResetRect(x, y, fWidth, fHeight);
    //     }
    // }
}
