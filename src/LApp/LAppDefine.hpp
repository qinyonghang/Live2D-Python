#pragma once

#include <CubismFramework.hpp>

namespace LAppDefine {

using namespace Csm;

constexpr auto ViewScale = 1.0f;
constexpr auto ViewMaxScale = 2.0f;
constexpr auto ViewMinScale = 0.8f;

constexpr auto ViewLogicalLeft = -1.0f;
constexpr auto ViewLogicalRight = 1.0f;
constexpr auto ViewLogicalBottom = -1.0f;
constexpr auto ViewLogicalTop = 1.0f;

constexpr auto ViewLogicalMaxLeft = -2.0f;
constexpr auto ViewLogicalMaxRight = 2.0f;
constexpr auto ViewLogicalMaxBottom = -2.0f;
constexpr auto ViewLogicalMaxTop = 2.0f;

constexpr auto ResourcesPath = "../resources/";

constexpr auto BackImageName = "back_class_normal.png";

constexpr auto GearImageName = "icon_gear.png";

constexpr auto PowerImageName = "close.png";

constexpr auto MotionGroupIdle = "Idle";
constexpr auto MotionGroupTapBody = "TapBody";

constexpr auto HitAreaNameHead = "Head";
constexpr auto HitAreaNameBody = "Body";

constexpr auto PriorityNone = 0;
constexpr auto PriorityIdle = 1;
constexpr auto PriorityNormal = 2;
constexpr auto PriorityForce = 3;

constexpr auto DebugTouchLogEnable = false;

constexpr auto CubismLoggingLevel = CubismFramework::Option::LogLevel_Verbose;

constexpr auto RenderTargetWidth = 1900;
constexpr auto RenderTargetHeight = 1000;
}  // namespace LAppDefine
