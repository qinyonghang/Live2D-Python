#ifndef LIVE2DMODEL_H
#define LIVE2DMODEL_H

#include <memory>
#include <string>
#include <vector>
#include <type_traits>

#include "qlib/memory.h"

namespace Live2D {

using namespace qlib;

#ifdef _WIN32
#ifdef LIVE2D_EXPORTS
#define LIVE2D_API __declspec(dllexport)
#else
#define LIVE2D_API __declspec(dllimport)
#endif
#else
#define LIVE2D_API
#endif

class LIVE2D_API Model final : public object {
public:
    enum : int32_t {
        OK = 0,
        ERR_MODEL_NOT_EXIST = -1,
        ERR_OPENGL_INIT = -2,
    };

    template <class String>
    Model(String&& model_path, size_t width, size_t height) {
        int32_t result{init(std::forward<String>(model_path), width, height)};
        throw_if(result != 0, "exception");
    }

    int32_t init(std::string model_path, size_t width, size_t height);
    // void set_background(std::string background);

    void draw(size_t width, size_t height);
    void set_dragging(float x, float y);
    bool is_hit(std::string const&, float x, float y);
    std::string hit_area(float x, float y);
    std::vector<std::string> expression_ids() const;
    std::vector<std::string> motion_ids() const;
    void set_expression(std::string const&);
    void set_motion(std::string const&, std::string sound_file = "", int32_t priority = 3);

protected:
    class Impl;
    std::shared_ptr<Impl> __impl;
};

}  // namespace Live2D

#endif
