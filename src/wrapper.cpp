#include "Model.h"
#include "Register.h"
#include "boost/python.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

namespace pylive2d {
void set_log_level(size_t level) {
    default_register().set_log_level(level);
}
};  // namespace pylive2d

BOOST_PYTHON_MODULE(pylive2d) {
    using namespace boost::python;
    using namespace pylive2d;

#ifdef WIN32
    SetConsoleOutputCP(65001);
#endif

    def("set_log_level", set_log_level);

    class_<std::vector<std::string>>("StringVector")
        .def(vector_indexing_suite<std::vector<std::string>>());

    class_<Model, boost::noncopyable>("Model", init<std::string, size_t, size_t>())
        .def("expression_ids", &Model::expression_ids)
        .def("motion_ids", &Model::motion_ids)
        // .def("set_background", &Model::set_background, args("background"))
        .def("set_dragging", &Model::set_dragging, args("x", "y"))
        .def("set_expression", &Model::set_expression, args("id"))
        .def("set_motion", &Model::set_motion,
             (args("id"), args("sound_file") = "", args("priority") = 3))
        .def("is_hit", &Model::is_hit, args("hit_area", "x", "y"))
        .def("hit_area", &Model::hit_area, args("x", "y"))
        .def("draw", static_cast<void (Model::*)(size_t, size_t)>(&Model::draw));
};
