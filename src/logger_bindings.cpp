#ifndef KBMOD_LOGGER_BINDINGS
#define KBMOD_LOGGER_BINDINGS

#include <pybind11/pybind11.h>
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

  py::class_<Logging, std::unique_ptr<Logging, py::nodelete>>(m, "Logging")
    .def(py::init([](){
      return std::unique_ptr<Logging, py::nodelete>(&Logging::logger());
    }))
    .def("set_logger", &Logging::set_logger)
    .def("debug", &Logging::debug)
    .def("info", &Logging::info)
    .def("warning", &Logging::warning)
    .def("error", &Logging::error)
    .def("critical", &Logging::critical);


  m.def("run", &core::run);
}


#endif
