#ifndef KBMOD_LOGGER_BINDINGS
#define KBMOD_LOGGER_BINDINGS

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

#include "logging.h"

PYBIND11_MODULE(logger, m) {
  logging::logging_bindings(m);
  m.def("run_pure", &core::run_pure);
  m.def("run_hook", &core::run_hook);
}


#endif
