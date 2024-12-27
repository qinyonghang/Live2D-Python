#include "Live2DModel.h"
#include "Live2DRegister.h"
#include "QLog.h"
#include "boost/python.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

BOOST_PYTHON_MODULE(PyLive2D) {
    using namespace boost::python;

    auto level_str = std::getenv("Live2DLogLevel");
    if (level_str != nullptr) {
        char* end_ptr = nullptr;
        auto level = std::strtol(level_str, &end_ptr, 10);
        if (*end_ptr != '\0') {
            qMWarn("Live2DLogLevel is not a number...");
        } else {
            Live2D::Register::set_log_level(level);
        }
    }

#ifdef WIN32
    SetConsoleOutputCP(65001);
#endif

    static auto& instance = QSingletonProductor<Live2D::Register>::get_instance();
    def("set_log_level", Live2D::Register::set_log_level);

    class_<std::vector<std::string>>("StringVector")
        .def(vector_indexing_suite<std::vector<std::string>>());

    class_<Live2D::Model, boost::noncopyable>("Model", init<std::string, size_t, size_t>())
        .def("expression_ids", &Live2D::Model::expression_ids)
        .def("motion_ids", &Live2D::Model::motion_ids)
        .def("set_background", &Live2D::Model::set_background, args("background"))
        .def("set_dragging", &Live2D::Model::set_dragging, args("x", "y"))
        .def("set_expression", &Live2D::Model::set_expression, args("id"))
        .def("set_motion", &Live2D::Model::set_motion,
             (args("id"), args("sound_file") = "", args("priority") = 3))
        .def("is_hit", &Live2D::Model::is_hit, args("hit_area", "x", "y"))
        .def("hit_area", &Live2D::Model::hit_area, args("x", "y"))
        .def("draw", &Live2D::Model::draw);
};
