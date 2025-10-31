#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <type_traits>

#include "qlib/argparse.h"

#include <GL/glew.h>
#ifdef WIN32
#include <GL/wglew.h>
#endif

#include "GLFW/glfw3.h"
#include "Model.h"
#include "Register.h"

namespace {
using namespace qlib;
using namespace pylive2d;

constexpr auto init_width = 550;
constexpr auto init_height = 500;

struct UserObject {
    Model* model;
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

        default_logger().info("Clicked: {} {} {}", mouse_x, mouse_y, action);
        if (action == GLFW_PRESS) {
        } else if (GLFW_RELEASE == action) {
            if (model != nullptr) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                model->set_dragging(width / 2.f, height / 2.f);
                if (model->is_hit("Head", mouse_x, mouse_y) && expression_ids.size() > 0) {
                    std::uniform_int_distribution<> dis_int(0, expression_ids.size() - 1);
                    default_logger().info("Hit head: expression={}", expression_ids[dis_int(gen)]);
                    model->set_expression(expression_ids[dis_int(gen)]);
                } else if (model->is_hit("Body", mouse_x, mouse_y) && motion_ids.size()) {
                    std::uniform_int_distribution<> dis_int(0, motion_ids.size() - 1);
                    default_logger().info("Hit body: motion={}", motion_ids[dis_int(gen)]);
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
        default_logger().trace("Initialize Start:");
        auto model = std::make_shared<Model>(model_path, width, height);
        default_logger().trace("Initialize End!");

        UserObject object;
        object.model = model.get();
        object.expression_ids = model->expression_ids();
        object.motion_ids = model->motion_ids();

        glfwSetWindowUserPointer(window, &object);
        glfwSetMouseButtonCallback(window, OnMouseCallBack);
        glfwSetCursorPosCallback(window, OnMouseCallBack);
        while (!glfwWindowShouldClose(window)) {
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            GLint fbo;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
            default_logger().debug("FBO: {}", fbo);
            model->draw(width, height, GLuint(fbo));

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } while (0);

    return result;
}
