#ifndef KBMOD_LOGGER_BINDINGS
#define KBMOD_LOGGER_BINDINGS

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
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

  py::class_<Logger, std::unique_ptr<Logger, py::nodelete>>(m, "Logger")
    .def("debug", &Logger::debug)
    .def("info", &Logger::info)
    .def("warning", &Logger::warning)
    .def("error", &Logger::error)
    .def("critical", &Logger::critical);

  py::class_<Logging, std::unique_ptr<Logging, py::nodelete>>(m, "Logging")
    .def(py::init([](){ return std::unique_ptr<Logging, py::nodelete>(Logging::logger()); }))
    .def("setConfig", &Logging::setConfig)
    .def("getLogger", [](py::str name) -> py::handle {
      py::module_ logging = py::module_::import("logging");
      py::handle logger = logging.attr("getLogger")(name);
      Logging::logger() -> register_pylogger(logger);
      return logger;
    });

  m.def("run", &core::run);
}


#endif
