#ifndef KBMOD_LOGGER
#define KBMOD_LOGGER

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
namespace py = pybind11;


typedef std::unordered_map<std::string, std::string> dict;


// don't understand how there's not an easier way to map str to an enum or even
// a different string, no lower/upper, no pattern matching, no switch, string
// isn't a type, what year are we in?
// https://docs.python.org/3/library/logging.html#logging-levels
enum LogLevel {
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

static std::unordered_map<std::string, LogLevel> StringToLogLevel {
  {"DEBUG",    LogLevel::DEBUG},
  {"INFO",     LogLevel::INFO},
  {"WARNING",  LogLevel::WARNING},
  {"ERROR",    LogLevel::ERROR},
  {"CRITICAL", LogLevel::CRITICAL}
};


class Logger {
public:
  std::string name;
  dict config;
  LogLevel level_threshold;

  Logger(std::string logger_name) :
    name(logger_name), config(), level_threshold{LogLevel::WARNING}
  {}

  Logger(std::string logger_name, dict conf) :
    name(logger_name), config(conf) {
    level_threshold = StringToLogLevel[config["level"]];
  }

  virtual void log(std::string level, std::string msg) = 0;
  void debug(std::string msg) { log("DEBUG", msg); }
  void info(std::string msg) { log("INFO", msg); }
  void warning(std::string msg) { log("WARNING", msg); }
  void error(std::string msg) { log("ERROR", msg); }
  void critical(std::string msg) { log("CRITICAL", msg); }
};


class PyLoggingLogger : public Logger {
private:
  py::handle pylogger;

public:
  // it would not be possible for this handler to require a configuration, it's
  // just a pointer to the Python's logging singleton
  PyLoggingLogger(py::handle logger) :
    Logger(logger.attr("name").cast<std::string>()),
    pylogger(logger)
  {}

  // We have just a global logger config really, Py has per-logger. Whenever
  // possible, leave it to Python to deal with the logic.
  virtual voi
  d log(std::string level, std::string msg){
    for (char& ch : level) ch = std::tolower(ch);
    pylogger.attr(level.c_str())(msg);
  }
};


class CoutLogger : public Logger{
private:
  // changing datefmt may require changing the size of char buf array. 2x sounds
  // safe enough (note %Y-%m expands to YYYY-MM etc) but I guess this could
  // overflow with abuse?
  std::string fmt_time(){
    time_t now;
    time(&now);
    char buf[2*sizeof(config["datefmt"])] = {0};
    strftime(buf, sizeof buf, config["datefmt"].c_str(), gmtime(&now));
    return std::string(buf);
  }

public:
  CoutLogger(std::string name, dict config) :
    Logger(name, config)
  {}

  // too lazy to resolve the Python-like log format string, todo some other time
  virtual void log(std::string level, std::string msg){
    if (level_threshold <= StringToLogLevel[level])
      std::cout << "[" << fmt_time() << " " << level << " " << name << "] " << msg << std::endl;
  }
};


class Logging{
private:
  static dict default_config;
  std::map<std::string, Logger*> registry;
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
    if(instance == nullptr)
      instance = new Logging();
    return instance;
  }

  static void setConfig(dict config){
    default_config = config;
  }

  // The most general C++ use-case
  template<class Logger>
  auto register_logger(std::string name, dict config) {
    instance->registry[name] = new Logger(name, config);
    return instance->registry[name];
  }

  Logger* getLogger(std::string name, dict config={}){
    // if key not found use default setup
    if (instance->registry.find(name) == instance->registry.end()) {
      dict tmpconf = config.size() != 0 ? config : instance->default_config;
      instance->register_logger<CoutLogger>(name, tmpconf);
    }
    return instance->registry[name];
  }

  // PyLoggingLogger gets its config straight from Python and requires a handle
  // so it's a special case
  auto register_pylogger(py::handle handler){
    std::string name = handler.attr("name").cast<std::string>();
    instance->registry[name] = new PyLoggingLogger(handler);
    return instance->registry[name];
  }
};

Logging* Logging::instance = nullptr;
dict Logging::default_config = {
  {"level", "WARNING"},
  {"datefmt", "'%Y-%m-%dT%H:%M:%SZ"}
};



namespace core {
  // CPP logger, acting as a hook to a Python logger via name matching, will
  // follow Python config
  void run_hook(){
    auto logger = Logging::logger() -> getLogger("kbmod_logger.module1");
    logger->debug("Test debug in-C++ logger from C++.");
    logger->info("Test info in-C++ logger from C++.");
    logger->warning("Test warning in-C++ from C++.");
    logger->error("Test error in-C++ from C++.");
    logger->critical("Test critical in-C++ from C++");
  }

  // CPP logger, unaware of Python's existence, would not follow Python config
  void run_pure(){
    auto logger = Logging::logger() -> getLogger("cpp.logger");
    logger->debug("Test debug in-C++ logger from C++.");
    logger->info("Test info in-C++ logger from C++.");
    logger->warning("Test warning in-C++ from C++.");
    logger->error("Test error in-C++ from C++.");
    logger->critical("Test critical in-C++ from C++");
  }
}
#endif // KBMOD_LOGGER
