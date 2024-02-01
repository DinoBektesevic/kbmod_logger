#ifndef KBMOD_LOGGER
#define KBMOD_LOGGER


#include <iostream>
#include<map>
#include <string>

#include <pybind11/pybind11.h>
namespace py = pybind11;


class Handler {
public:
  std::string name;
  virtual void debug(std::string msg) = 0;
  /* void info(std::string msg);
     void warning(std::string msg);
  void error(std::string msg);
  void critical(std::string msg);*/
};



class PyLoggingHandler : public Handler {
private:
  py::handle handler_;

public:
  std::string name_;

  PyLoggingHandler(py::handle handle) :
    name_(handle.attr("name").cast<std::string>()), handler_(handle)
  {}

   virtual void debug(std::string msg){
     handler_.attr("debug")(msg).cast<std::string>();
  }

  /*
   void info(std::string msg){
     handler_.attr("info")(msg).cast<std::string>();
  }

   void warning(std::string msg){
     handler_.attr("warning")(msg).cast<std::string>();
     }

   void error(std::string msg){
     handler_.attr("error")(msg).cast<std::string>();
  }

   void critical(std::string msg){
     handler_.attr("critical")(msg).cast<std::string>();
     }*/
};


class CoutHandler : public Handler{
private:
  // helper methods
  std::string fmt_time() const{
    time_t now;
    time(&now);
    char buf[sizeof "YYYY-MM-DDTHH:MM:SSZ"] = {0};
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return std::string(buf);
  }

public:
  std::string name_;

  CoutHandler(std::string name) : name_(name) {}

  virtual void debug(std::string msg){
    std::cout << "[" << fmt_time() << " DEBUG] " << msg << std::endl;
  }
  /*
   void info(std::string msg){
    std::cout << "[" << fmt_time() << " INFO] " << msg  << std::endl;;
  }

   void warning(std::string msg){
    std::cout << "[" << fmt_time() << " WARNING] " << msg  << std::endl;;
    }

   void error(std::string msg){
    std::cout << "[" << fmt_time() << " ERROR] " << msg  << std::endl;;
  }

   void critical(std::string msg){
    std::cout << "[" << fmt_time() << " CRITICAL] " << msg  << std::endl;;
    }*/
};


class Logger{
private:
  Handler *handler_;

public:
  Logger(Handler *handler): handler_(handler) {}
  auto get_name() { return handler_ -> name; }
  virtual void debug(std::string msg){ handler_ -> debug(msg); }
  /* auto info(std::string msg){ handler_->info(msg); }
  auto warning(std::string msg){ handler_->warning(msg); }
  auto error(std::string msg){ handler_->error(msg); }
  auto critical(std::string msg){ handler_->critical(msg); }*/
};

class Logging{
private:
  std::map<std::string, Logger*> registry;
  std::ostream& handler_ = std::cout;
  int set=0;

  // make this a singleton
  Logging(){}

public:
  static Logging& logger(){
    static Logging instance;
    return instance;
  }

  auto register_logger(py::handle handler){
    std::string name = handler.attr("name").cast<std::string>();
    registry[name] = new Logger(new PyLoggingHandler(handler));
    return registry[name];
  }

  auto register_logger(std::string name){
    registry[name] = new Logger(new CoutHandler(name));
    return registry[name];
  }

  Logger* getLogger(std::string name){
    // if key not found
    if (registry.find("f") == registry.end()) {
      register_logger(name);
    }
    return registry[name];
  }
};

namespace core {
  void run(){
    auto logger = Logging::logger().getLogger("test");
    logger->debug("Test debug in-C++ logger called from Python.");
    //logger->info("Test info in-C++ logger called from Python.");
    //logger->warning("Test warning in-C++ called logger from Python.");
    //logger->error("Test error in-C++ logger called from Python.");
    //logger->critical("Test critical in-C++ logger called from Python.");
  }
}
#endif // KBMOD_LOGGER
