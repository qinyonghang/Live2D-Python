#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <type_traits>
#include <chrono>

#include "qlib/argparse.h"
#include "qlib/logger.h"

#include "GLFW/glfw3.h"
#include "Live2DModel.h"
#include "Live2DRegister.h"

namespace {
using namespace qlib;

constexpr auto init_width = 550;
constexpr auto init_height = 500;

using sink_type = std::shared_ptr<logger::ansicolor_stdout_sink_mt>;
auto __sink = std::make_shared<logger::ansicolor_stdout_sink_mt>();
auto __logger = logger::value<string_view_t, sink_type>("Live2D", __sink);

struct UserObject {
    Live2D::Model* model;
    std::vector<std::string> expression_ids;
    std::vector<std::string> motion_ids;
};

void OnMouseCallBack(GLFWwindow* window, int button, int action, int modify) {
    auto object = reinterpret_cast<UserObject*>(glfwGetWindowUserPointer(window));

    auto model = object->model;
    auto const& expression_ids = object->expression_ids;
    auto const& motion_ids = object->motion_ids;

    std::random_device rd;
    std::mt19937 gen(rd());
    if (GLFW_MOUSE_BUTTON_LEFT == button) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        __logger.info("Clicked: {} {} {}", mouse_x, mouse_y, action);
        if (action == GLFW_PRESS) {
        } else if (GLFW_RELEASE == action) {
            if (model != nullptr) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                model->set_dragging(width / 2.f, height / 2.f);
                if (model->is_hit("Head", mouse_x, mouse_y) && expression_ids.size() > 0) {
                    std::uniform_int_distribution<> dis_int(0, expression_ids.size() - 1);
                    __logger.info("Hit head: expression={}", expression_ids[dis_int(gen)]);
                    model->set_expression(expression_ids[dis_int(gen)]);
                } else if (model->is_hit("Body", mouse_x, mouse_y) && motion_ids.size()) {
                    std::uniform_int_distribution<> dis_int(0, motion_ids.size() - 1);
                    __logger.info("Hit body: motion={}", motion_ids[dis_int(gen)]);
                    model->set_motion(motion_ids[dis_int(gen)]);
                }
            }
        }
    }
}

void OnMouseCallBack(GLFWwindow* window, double x, double y) {
    auto object = reinterpret_cast<UserObject*>(glfwGetWindowUserPointer(window));
    auto model = object->model;

    // std::cout << "Move: " << x << ", " << y << std::endl;
    if (model != nullptr) {
        model->set_dragging(x, y);
        // mouse_x = x;
        // mouse_y = y;
    }
}

};  // namespace

int32_t main(int32_t argc, char** argv) {
    int32_t result{0};

    do {
        auto level_str = std::getenv("Live2D_LogLevel");
        if (level_str != nullptr) {
            char* end_ptr = nullptr;
            auto level = std::strtol(level_str, &end_ptr, 10);
            if (*end_ptr != '\0') {
                __logger.warn("Live2D_LogLevel is not a number...");
            } else {
                Live2D::Register::set_log_level(level);
                __sink->set_level(static_cast<logger::level>(level));
            }
        }

        argparse::parser<> parser("Live2D Viewer");

        parser.add_argument("model").help("Path to model.json file");

        result = parser.parse_args(argc, argv);
        if (0 != result) {
            std::cout << parser.help() << std::endl;
            break;
        }

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

        auto model_path = parser.get<std::string>("model");
        __logger.trace("Initialize Start:");
        auto model = std::make_shared<Live2D::Model>(model_path, width, height);
        __logger.trace("Initialize End!");

        UserObject object;
        object.model = model.get();
        object.expression_ids = model->expression_ids();
        object.motion_ids = model->motion_ids();

        // __logger.info("expression_ids={}, motion_ids={}", serialize(object.expression_ids),
        //             serialize(object.motion_ids));

        glfwSetWindowUserPointer(window, &object);
        glfwSetMouseButtonCallback(window, OnMouseCallBack);
        glfwSetCursorPosCallback(window, OnMouseCallBack);
        while (!glfwWindowShouldClose(window)) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            model->draw(width, height);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } while (0);

    return result;
}
