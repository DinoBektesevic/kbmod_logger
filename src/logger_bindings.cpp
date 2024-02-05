#ifndef KBMOD_LOGGER_BINDINGS
#define KBMOD_LOGGER_BINDINGS

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

#include "logging.h"

PYBIND11_MODULE(logger, m) {
  /*  m.doc() = R"pbdoc(
        KBMOD Logger
        ------------

        .. currentmodule:: kbmod_logger

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

  py::class_<Logging, std::unique_ptr<Logging, py::nodelete>>(m, "Logging")
    .def(py::init([](){ return std::unique_ptr<Logging, py::nodelete>(Logging::logger()); }))
    .def("setConfig", &Logging::setConfig)
    .def("getLogger", [](py::str name) -> py::handle {
      py::module_ logging = py::module_::import("logging");
      py::handle logger = logging.attr("getLogger")(name);
      Logging::logger() -> register_pylogger(logger);
      return logger;
      });*/

  logging::logging_bindings(m);
  m.def("run_pure", &core::run_pure);
  m.def("run_hook", &core::run_hook);
}


#endif
