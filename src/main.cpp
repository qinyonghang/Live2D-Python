#include <iostream>
#include <random>

#include "GLFW/glfw3.h"
#include "Live2DModel.h"
#include "Live2DRegister.h"
#include "QLog.h"

constexpr auto init_width = 550;
constexpr auto init_height = 500;
constexpr auto model_path = "../resources/Haru/Haru.model3.json";
constexpr auto background_path = "../resources/back_class_normal.png";
// double mouse_x, mouse_y;
std::shared_ptr<Live2D::Model> __model;
std::vector<std::string> __expression_ids;
std::vector<std::string> __motion_ids;

void OnMouseCallBack(GLFWwindow* window, int button, int action, int modify) {
    // std::cout << "button=" << button << ", action=" << action << ", modify=" << modify << std::endl;
    std::random_device rd;
    std::mt19937 gen(rd());
    if (GLFW_MOUSE_BUTTON_LEFT == button) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        qMInfo("Clicked: {} {} {}", mouse_x, mouse_y, action);
        if (action == GLFW_PRESS) {
        } else if (GLFW_RELEASE == action) {
            if (__model != nullptr) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                __model->set_dragging(width / 2.f, height / 2.f);
                if (__model->is_hit("Head", mouse_x, mouse_y) && __expression_ids.size() > 0) {
                    std::uniform_int_distribution<> dis_int(0, __expression_ids.size() - 1);
                    qMInfo("Hit head: expression={}", __expression_ids[dis_int(gen)]);
                    __model->set_expression(__expression_ids[dis_int(gen)]);
                } else if (__model->is_hit("Body", mouse_x, mouse_y) && __motion_ids.size()) {
                    std::uniform_int_distribution<> dis_int(0, __motion_ids.size() - 1);
                    qMInfo("Hit body: motion={}", __motion_ids[dis_int(gen)]);
                    __model->set_motion(__motion_ids[dis_int(gen)]);
                }
            }
        }
    }
}

void OnMouseCallBack(GLFWwindow* window, double x, double y) {
    // std::cout << "Move: " << x << ", " << y << std::endl;
    if (__model != nullptr) {
        __model->set_dragging(x, y);
        // mouse_x = x;
        // mouse_y = y;
    }
}

int32_t main(int32_t argc, char** argv) {
    int32_t result{0};

    do {
        glfwSetErrorCallback([](int error, const char* description) {
            std::cout << "error=" << error << ", description=" << description << std::endl;
            // qCritical("error={}, description={}", error, description);
        });

        result = glfwInit();
        if (result == GLFW_FALSE) {
            std::cout << "result=" << result << std::endl;
            // qMCritical("Failed to initialize GLFW...");
            break;
        }
        auto glfw_destroy = std::shared_ptr<void>(nullptr, [](void* ptr) { glfwTerminate(); });

        auto window = glfwCreateWindow(init_width, init_height, "SAMPLE", nullptr, nullptr);
        if (window == nullptr) {
            // qMCritical("Can't create GLFW window.");
            std::cout << "window=" << window << std::endl;
            break;
        }
        auto window_destroy =
            std::shared_ptr<void>(nullptr, [&window](void* ptr) { glfwDestroyWindow(window); });

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        qMTrace("Initialize Start:");
        auto live2d = std::make_shared<Live2D::Register>();
        auto model = std::make_shared<Live2D::Model>(model_path, width, height);
        qMTrace("Initialize End!");
        auto expression_ids = model->expression_ids();
        auto motion_ids = model->motion_ids();
        qMInfo("expression_ids={}, motion_ids={}", serialize(expression_ids),
               serialize(motion_ids));

        __model = model;
        __expression_ids = expression_ids;
        __motion_ids = motion_ids;
        glfwSetMouseButtonCallback(window, OnMouseCallBack);
        glfwSetCursorPosCallback(window, OnMouseCallBack);
        while (!glfwWindowShouldClose(window)) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            model->draw(width, height);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        __model = nullptr;
    } while (0);

    return result;
}
