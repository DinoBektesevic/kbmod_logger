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
  virtual void info(std::string msg) = 0;
  virtual void warning(std::string msg) = 0;
  virtual void error(std::string msg) = 0;
  virtual void critical(std::string msg) = 0;
};


class PyLoggingHandler : public Handler {
private:
  py::handle handler_;

public:
  std::string name_;

  PyLoggingHandler(py::handle handle) :
    name_(handle.attr("name").cast<std::string>()),
    handler_(handle)
  {}

  virtual void debug(std::string msg){
    handler_.attr("debug")(msg);
  }

  virtual void info(std::string msg){
    handler_.attr("info")(msg);
  }

  virtual void warning(std::string msg){
    handler_.attr("warning")(msg);
  }

  virtual void error(std::string msg){
    handler_.attr("error")(msg);
  }

  virtual void critical(std::string msg){
    handler_.attr("critical")(msg);
  }
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

  virtual void info(std::string msg){
    std::cout << "[" << fmt_time() << " INFO] " << msg  << std::endl;;
  }

  virtual void warning(std::string msg){
    std::cout << "[" << fmt_time() << " WARNING] " << msg  << std::endl;;
  }

  virtual void error(std::string msg){
    std::cout << "[" << fmt_time() << " ERROR] " << msg  << std::endl;;
  }

  virtual void critical(std::string msg){
    std::cout << "[" << fmt_time() << " CRITICAL] " << msg  << std::endl;;
  }
};


class Logger{
private:
  Handler *handler_;

public:
  Logger(Handler *handler): handler_(handler) {}
  auto get_name() { return handler_ -> name; }
  virtual void debug(std::string msg){ handler_ -> debug(msg); }
  virtual void info(std::string msg){ handler_->info(msg); }
  virtual void warning(std::string msg){ handler_->warning(msg); }
  virtual void error(std::string msg){ handler_->error(msg); }
  virtual void critical(std::string msg){ handler_->critical(msg); }
};

class Logging{
private:

  std::map<std::string, Logger*> registry;
  std::ostream& handler_ = std::cout;
  static Logging* instance;

  // make this a singleton
  Logging(){}
  ~Logging(){}

public:
  // prevent copy and assignment ops
  Logging(Logging &other) = delete;
  void operator=(const Logging &) = delete;

  // get the singleton instance
  static Logging* logger(){
    if(instance == nullptr){
      instance = new Logging();
    }
    std::cout << "instance: " << instance << std::endl;
    return instance;
  }

  Logging* GetInstance();

  auto register_logger(py::handle handler){
    std::string name = handler.attr("name").cast<std::string>();
    instance->registry[name] = new Logger(new PyLoggingHandler(handler));
    return instance->registry[name];
  }

  auto register_logger(std::string name){
    std::cout << "Registering C++ Logger: " << name << std::endl;
    instance->registry[name] = new Logger(new CoutHandler(name));
    return instance->registry[name];
  }

  Logger* getLogger(std::string name){
    // if key not found
    if (instance->registry.find(name) == instance->registry.end()) {
      std::cout << "Logger: " << name << " not found." << std::endl;
      instance->register_logger(name);
    }
    return instance->registry[name];
  }
};


Logging* Logging::instance= nullptr;;

Logging *Logging::GetInstance()
{
  if(instance==nullptr){
    instance = new Logging();
  }
  return instance;
}



namespace core {
  void run(){
    auto logger = Logging::logger() -> getLogger("test");
    logger->debug("Test debug in-C++ logger from C++.");
    logger->info("Test info in-C++ logger from C++.");
    logger->warning("Test warning in-C++ from C++.");
    logger->error("Test error in-C++ from C++.");
    logger->critical("Test critical in-C++ from C++");

  }
}
#endif // KBMOD_LOGGER
