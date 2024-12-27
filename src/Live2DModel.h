#pragma once

#include "QException.h"
#include "QObject.h"

namespace Live2D {

#ifdef _WIN32
#ifdef LIVE2D_EXPORTS
#define LIVE2D_API __declspec(dllexport)
#else
#define LIVE2D_API __declspec(dllimport)
#endif
#else
#define LIVE2D_API
#endif

class LIVE2D_API Model : public QObject {
public:
    template <typename String>
    Model(String&& model_path, size_t width, size_t height) : QObject(nullptr) {
        int32_t result{init(std::forward<String>(model_path), width, height)};
        if (result != 0) {
            QCMTHROW_EXCEPTION("init return {}... model_path={}, width={}, height={}", result,
                               model_path, width, height);
        }
    }

    int32_t init(std::string model_path, size_t width, size_t height);
    void set_background(std::string background);

    void draw(size_t width, size_t height);
    void set_dragging(float x, float y);
    bool is_hit(std::string const&, float x, float y);
    std::string hit_area(float x, float y);
    std::vector<std::string> expression_ids() const;
    std::vector<std::string> motion_ids() const;
    void set_expression(std::string const&);
    void set_motion(std::string const&, std::string sound_file = "", int32_t priority = 3);

protected:
    std::shared_ptr<QObject> __impl;
};

int32_t LIVE2D_API Live2DMain(int32_t argc, char** argv);

}  // namespace Live2D
