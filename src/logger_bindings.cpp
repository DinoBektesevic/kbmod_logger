#ifndef KBMOD_LOGGER_BINDINGS
#define KBMOD_LOGGER_BINDINGS

#include <pybind11/pybind11.h>
#include <string>
#include "logger.cpp"


namespace py = pybind11;


PYBIND11_MODULE(logger, m) {
  m.doc() = R"pbdoc(
        KBMOD Logger
        ------------

        .. currentmodule:: kbmod_logger

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

  py::class_<Handler>(m, "Handler")
    .def_readonly("name", &Handler::name)
    .def("debug", &Handler::debug)
    /*.def("info", &Handler::info)
    .def("warning", &Handler::warning)
    .def("error", &Handler::error)
    .def("critical", &Handler::critical)*/;

  py::class_<Logger>(m, "Logger")
    .def("debug", &Logger::debug)
    /*.def("info", &Logger::info)
    .def("warning", &Logger::warning)
    .def("error", &Logger::error)
    .def("critical", &Logger::critical)*/;

  py::class_<Logging, std::unique_ptr<Logging, py::nodelete>>(m, "Logging")
    .def(py::init([](){
      return std::unique_ptr<Logging, py::nodelete>(&Logging::logger());
    }))
    .def("logger", &Logging::logger)
    .def("register_logger", py::overload_cast<py::handle>(&Logging::register_logger))
    .def("register_logger", py::overload_cast<std::string>(&Logging::register_logger))
    .def("getLogger", &Logging::getLogger);

  m.def("run", &core::run);
}


#endif
