#ifndef KBMOD_LOGGER
#define KBMOD_LOGGER

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
namespace py = pybind11;


typedef std::unordered_map<std::string, std::string> dict;


class Handler {
public:
  std::string name;
  dict config;

  Handler(std::string logger_name) : name(logger_name), config() {}
  Handler(std::string logger_name, dict conf) :
    name(logger_name),
    config(conf)
  {}

  virtual void debug(std::string msg) = 0;
  virtual void info(std::string msg) = 0;
  virtual void warning(std::string msg) = 0;
  virtual void error(std::string msg) = 0;
  virtual void critical(std::string msg) = 0;
};


class PyLoggingHandler : public Handler {
private:
  py::handle pylogger;

public:
  // it would not be possible for this handler to require a configuration, it's
  // just a pointer to the Python's logging singleton
  PyLoggingHandler(py::handle logger) :
    Handler(logger.attr("name").cast<std::string>()),
    pylogger(logger)
  {}

  virtual void debug(std::string msg){
    pylogger.attr("debug")(msg);
  }

  virtual void info(std::string msg){
    pylogger.attr("info")(msg);
  }

  virtual void warning(std::string msg){
    pylogger.attr("warning")(msg);
  }

  virtual void error(std::string msg){
    pylogger.attr("error")(msg);
  }

  virtual void critical(std::string msg){
    pylogger.attr("critical")(msg);
  }
};


class CoutHandler : public Handler{
private:
  // changing datefmt may require changing the size of char buf array. 2x sounds
  // safe enough (note %Y-%m expands to YYYY-MM etc) but I guess this could
  // overflow with abuse?
  std::string fmt_time(){
    time_t now;
    time(&now);
    std::string val = config["datefmt"];
    int len = sizeof(config["datefmt"]);
    char buf[2*len] = {0};
    strftime(buf, sizeof buf, config["datefmt"].c_str(), gmtime(&now));
    return std::string(buf);
  }

public:
  CoutHandler(std::string name, dict config) :
    Handler(name, config)
  {}

  // too lazy to resolve the Python-like log format string, todo some other time
  virtual void debug(std::string msg){
    std::cout << "[" << fmt_time() << " DEBUG " << name << "] " << msg << std::endl;
  }

  virtual void info(std::string msg){
    std::cout << "[" << fmt_time() << " INFO " << name << "] " << msg  << std::endl;;
  }

  virtual void warning(std::string msg){
    std::cout << "[" << fmt_time() << " WARNING " << name << "] " << msg  << std::endl;;
  }

  virtual void error(std::string msg){
    std::cout << "[" << fmt_time() << " ERROR " << name << "] " << msg  << std::endl;;
  }

  virtual void critical(std::string msg){
    std::cout << "[" << fmt_time() << " CRITICAL " << name << "] " << msg  << std::endl;;
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

  // could use a format, see todo above
  static dict default_config;
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
    return instance;
  }

  static void setConfig(dict config){
    default_config = config;
  }

  // The most general C++ use-case
  template<class LogHandler>
  auto register_logger(std::string name, dict config) {
    instance->registry[name] = new Logger(new LogHandler(name, config));
    return instance->registry[name];
  }

  Logger* getLogger(std::string name, dict config={}){
    // if key not found use default setup
    if (instance->registry.find(name) == instance->registry.end()) {
      dict tmpconf = config.size() != 0 ? config : instance->default_config;
      instance->register_logger<CoutHandler>(name, tmpconf);
    }
    return instance->registry[name];
  }

  // PyLoggingHandler gets its config straight from Python and requires a handle
  // so it's a special case
  auto register_pylogger(py::handle handler){
    std::string name = handler.attr("name").cast<std::string>();
    instance->registry[name] = new Logger(new PyLoggingHandler(handler));
    return instance->registry[name];
  }



};

Logging* Logging::instance = nullptr;
dict Logging::default_config = {
  {"level", "WARNING"},
  {"datefmt", "'%Y-%m-%dT%H:%M:%SZ"}
};



namespace core {
  void run(){
    auto logger = Logging::logger() -> getLogger("cpp.logger");
    logger->debug("Test debug in-C++ logger from C++.");
    logger->info("Test info in-C++ logger from C++.");
    logger->warning("Test warning in-C++ from C++.");
    logger->error("Test error in-C++ from C++.");
    logger->critical("Test critical in-C++ from C++");

  }
}
#endif // KBMOD_LOGGER
