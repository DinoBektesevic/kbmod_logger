#ifndef KBMOD_LOGGER
#define KBMOD_LOGGER


#include <iostream>
#include <string>

#ifdef Py_PYTHON_H

#include <pybind11/pybind11.h>
namespace py = pybind11;


class Logging{
public:
  static Logging& logger(){
    static Logging instance;
    return instance;
  }

  void set_logger(py::handle logger){
    py::print("Setting the logger");
    handler_ = logger;
  }

  auto debug(std::string msg){
    return handler_.attr("debug")(msg);
  }

  auto info(std::string msg){
    return handler_.attr("info")(msg);
  }

  auto warning(std::string msg){
    return handler_.attr("warning")(msg);
  }

  auto error(std::string msg){
    return handler_.attr("error")(msg);
  }

  auto critical(std::string msg){
    return handler_.attr("critical")(msg);
  }

private:
  py::handle handler_;
  int set=0;

  Logging(){};
};

#else

#include <ctime>

class Logging{
private:

  std::ostream& handler_ = std::cout;
  int set=0;

  // make this a singleton
  Logging(){}

  // helper methods
  std::string fmt_time() const{
    time_t now;
    time(&now);
    char buf[sizeof "YYYY-MM-DDTHH:MM:SSZ"] = {0};
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return std::string(buf);
  };

public:
  static Logging& logger(){
    static Logging instance;
    return instance;
  }

  auto debug(std::string msg){
    handler_ << "[" << fmt_time() << " DEBUG] " << msg << std::endl;
  }

  auto info(std::string msg){
    handler_ << "[" << fmt_time() << " INFO] " << msg  << std::endl;;
  }

  auto warning(std::string msg){
    handler_ << "[" << fmt_time() << " WARNING] " << msg  << std::endl;;
  }

  auto error(std::string msg){
    handler_ << "[" << fmt_time() << " ERROR] " << msg  << std::endl;;
  }

  auto critical(std::string msg){
    handler_ << "[" << fmt_time() << " CRITICAL] " << msg  << std::endl;;
  }
};

#endif  // Py_PYTHON_H end


namespace core {
  void run(){
    Logging::logger().debug("Test debug in-C++ logger called from Python.");
    Logging::logger().info("Test info in-C++ logger called from Python.");
    Logging::logger().warning("Test warning in-C++ called logger from Python.");
    Logging::logger().error("Test error in-C++ logger called from Python.");
    Logging::logger().critical("Test critical in-C++ logger called from Python.");
  }
}
#endif // KBMOD_LOGGER
