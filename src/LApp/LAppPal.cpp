/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <cstdio>
#include <stdarg.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <Model/CubismMoc.hpp>
#include "LAppDefine.hpp"
#ifdef WIN32
#include <Windows.h>
#else
#include <time.h>
#endif

using std::endl;
using namespace Csm;
using namespace std;
using namespace LAppDefine;



namespace {
#ifdef WIN32
    static LARGE_INTEGER s_frequency = {0};
#endif
}

double LAppPal::s_currentFrame = 0.0;
double LAppPal::s_lastFrame = 0.0;
double LAppPal::s_deltaTime = 0.0;

csmByte* LAppPal::LoadFileAsBytes(const string filePath, csmSizeInt* outSize)
{
    //filePath;//
    const char* path = filePath.c_str();

    int size = 0;
    struct stat statBuf;
    if (stat(path, &statBuf) == 0)
    {
        size = statBuf.st_size;

        if (size == 0)
        {
                PrintLogLn("Stat succeeded but file size is zero. path:%s", path);
            return NULL;
        }
    }
    else
    {
            PrintLogLn("Stat failed. errno:%d path:%s", errno, path);
        return NULL;
    }

    std::fstream file;
    file.open(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
            PrintLogLn("File open failed. path:%s", path);
        return NULL;
    }

    char* buf = new char[size];
    file.read(buf, size);
    file.close();

    *outSize = size;
    return reinterpret_cast<csmByte*>(buf);
}

void LAppPal::ReleaseBytes(csmByte* byteData)
{
    delete[] byteData;
}

csmFloat32  LAppPal::GetDeltaTime()
{
    return static_cast<csmFloat32>(s_deltaTime);
}

void LAppPal::UpdateTime()
{
#ifdef WIN32
    if (s_frequency.QuadPart == 0) {
        QueryPerformanceFrequency(&s_frequency);
    }

    LARGE_INTEGER currentCount;
    QueryPerformanceCounter(&currentCount);

    s_currentFrame = static_cast<double>(currentCount.QuadPart) / s_frequency.QuadPart;
    s_deltaTime = s_currentFrame - s_lastFrame;
    s_lastFrame = s_currentFrame;
#else
    struct timespec currentTimeSpec;
    clock_gettime(CLOCK_MONOTONIC, &currentTimeSpec);

    s_currentFrame = currentTimeSpec.tv_sec + static_cast<double>(currentTimeSpec.tv_nsec) / 1e9;
    s_deltaTime = s_currentFrame - s_lastFrame;
    s_lastFrame = s_currentFrame;
#endif
}

void LAppPal::PrintLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
#ifdef WIN32
    _vsnprintf_s(buf, sizeof(buf), format, args);
#else
    vsnprintf(buf, sizeof(buf), format, args);
#endif
#ifdef CSM_DEBUG_MEMORY_LEAKING
// メモリリークチェック時は大量の標準出力がはしり重いのでprintfを利用する
    std::printf(buf);
#else
    std::cout << buf;
#endif
    va_end(args);
}

void LAppPal::PrintLogLn(const Csm::csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
#ifdef WIN32
    _vsnprintf_s(buf, sizeof(buf), format, args);
#else
    vsnprintf(buf, sizeof(buf), format, args);
#endif
#ifdef CSM_DEBUG_MEMORY_LEAKING
    // メモリリークチェック時は大量の標準出力がはしり重いのでprintfを利用する
    std::printf("%s\n", buf);
#else
    std::cout << buf << std::endl;
#endif
    va_end(args);
}

void LAppPal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}

void LAppPal::PrintMessageLn(const csmChar* message)
{
    PrintLogLn("%s", message);
}
