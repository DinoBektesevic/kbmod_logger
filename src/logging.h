#ifndef KBMOD_LOGGER
#define KBMOD_LOGGER

#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <string>
#include <vector>


/*
 * The Logging class is a singleton that keeps a reference to all created
 * Loggers. The Loggers define the log format and IO method (stdout, file etc.).
 * Logging keeps references to Loggers in a registry. This registry is exposed
 * via the `getLogger` method, which doubles as a factory function for Loggers
 * This is modeled after Python's logging module. When `getLogger` is called from
 * Python (via the pybind11 bindings) it creates a new Python-side Logger object
 * and registers its reference. When called C++ side it creates a C++-side
 * Logger and registers its reference. Accessing a `getLogger` using a name that
 * was already registered - returns the reference from the registry (python or
 * internal).
 *
 * The obvious pitfall is the case when a user does not route through this cls,
 * and instead registers a Python-side Logger via Python's logging module. Then
 * these Python-side Loggers are not registered in the Logging's registry the
 * KBMOD Logging will default to using the C++ std::out logger. This can lead to
 * differing output formats if the Python Logger in question is re-configured.
 */
namespace logging {
  // There must be an easier way to establish mapping between log level and str
  // but I don't know it. Advice appreciated. Also switch statement? pattern
  // matching? Uppercase/lowercase? string isn't a type? what year are we in?

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

  static std::unordered_map<LogLevel, std::string> LogLevelToString {
    {LogLevel::DEBUG,    "DEBUG"},
    {LogLevel::INFO,     "INFO"},
    {LogLevel::WARNING,  "WARNING"},
    {LogLevel::ERROR,    "ERROR"},
    {LogLevel::CRITICAL, "CRITICAL"}
  };


  // dictionary of strings are used to configure Logger behavior such as minimal
  // log level allowed to print, time-format, log format etc.
  typedef std::unordered_map<std::string, std::string> sdict;

  // Logger is a base class that dispatches the logging mechanism (IO and
  // formatting) via the virtual method `log`
  class Logger {
  public:
    std::string name;
    sdict config;
    LogLevel level_threshold;

    Logger(const std::string logger_name) :
      name(logger_name), config(), level_threshold{LogLevel::WARNING}
    {}

    Logger(const std::string logger_name, const sdict conf) :
      name(logger_name), config(conf) {
      level_threshold = StringToLogLevel[config["level"]];
    }

    virtual ~Logger(){}

    std::string fmt_time(){
      std::time_t now = std::time(nullptr);
      std::tm timeinfo;

      if (config["converter"] == "gmtime"){
        timeinfo = *std::gmtime(&now);
      }
      else {
        timeinfo = *std::localtime(&now);
      }

      std::ostringstream timestamp;
      timestamp <<  std::put_time(&timeinfo, config["datefmt"].c_str());
      return timestamp.str();
    }

    std::string fmt_log(const std::string level, const std::string msg){
      std::string logfmt = config["format"];

      std::regex t("%\\(asctime\\)s");
      logfmt = std::regex_replace(logfmt, t, fmt_time());

      std::regex l("%\\(levelname\\)s");
      logfmt = std::regex_replace(logfmt, l, level);

      std::regex n("%\\(name\\)s");
      logfmt = std::regex_replace(logfmt, n, name);

      std::regex m("%\\(message\\)s");
      logfmt = std::regex_replace(logfmt, m, msg);

      return logfmt;
    }

    virtual void log(std::string level, std::string msg) = 0;
    void debug(std::string msg) { log("DEBUG", msg); }
    void info(std::string msg) { log("INFO", msg); }
    void warning(std::string msg) { log("WARNING", msg); }
    void error(std::string msg) { log("ERROR", msg); }
    void critical(std::string msg) { log("CRITICAL", msg); }
  };


#ifdef Py_PYTHON_H
  // Passes the logging to the Python-side Logger. The py::handle is the
  // pybind11 container holding the reference to the Python Logger. The actual
  // logging format and mechanism is handled Python-side by the logging module,
  // so just pass the message and level onward and let it be resolved in Python
  class PyLogger : public Logger {
  private:
    py::object pylogger;

  public:
    PyLogger(py::object logger) :
      Logger(logger.attr("name").cast<std::string>()),
      pylogger(logger)
    {}

    virtual void log(std::string level, const std::string msg){
      for (char& ch : level) ch = std::tolower(ch);
      pylogger.attr(level.c_str())(msg);
    }
  };
#endif // Py_PYTHON_H


  // Glorified std::cout. Unlike the PyLogger, CoutLogger is configurable.
  // Configuration should contain `level`, `datefmt` and `format` keys:
  // - `level`:  LogLevel enum value, the minimal level that is printed
  // - `datefmt`: timestamp template usable with `strftime`
  // - `format`: log format template, too much commitment atm, not supported
  class CoutLogger : public Logger{
  public:
    CoutLogger(std::string name, sdict config) :
      Logger(name, config)
    {}

    virtual void log(const std::string level, const std::string msg){
      if (level_threshold <= StringToLogLevel[level])
        std::cout << fmt_log(level, msg) << std::endl;
    }
  };


  // The singleton keeping the registry of all registered Loggers and their
  // default configuration. Use `logger` to access the singleton instance,
  // getLoger to get an existing logger or to create a new default logger.
  // No good examples, but let's say a non-stdout C++ only logger is required,
  // use getLogger<LoggerCls>(name, conf) to register and get the instance of it
  class Logging{
  private:
    sdict default_config = {
      {"level", "WARNING"},
      {"datefmt", "%Y-%m-%dT%H:%M:%SZ"},
      {"converter", "localtime"},
      {"format", "[%(asctime)s %(levelname)s %(name)s] %(message)s"}
    };
    std::unordered_map<std::string, Logger*> registry;

    Logging(){}
    ~Logging(){
      for (auto elem = registry.begin(); elem != registry.end(); elem++)
        delete elem->second;
    }

  public:
    Logging(Logging &other) = delete;
    void operator=(const Logging &) = delete;

    // get the singleton instance
    static Logging* logging(){
      static Logging* instance = new Logging();
      return instance;
    }

    void setConfig(sdict config){
      Logging::logging()->default_config = config;
    }

    sdict getConfig(){
      return Logging::logging()->default_config;
    }

    template<class LoggerCls>
    static Logger* getLogger(std::string name, sdict config={}){
      Logging* instance = Logging::logging();

      // if key not found use default setup
      if (instance->registry.find(name) == instance->registry.end()) {
        sdict tmpconf = config.size() != 0 ? config : instance->getConfig();
        instance->registry[name] = new LoggerCls(name, tmpconf);
      }
      return instance->registry[name];
    }

    static Logger* getLogger(std::string name, sdict config={}){
      return Logging::logging()->getLogger<CoutLogger>(name, config);
    }

    void register_logger(Logger* logger){
      Logging::logging()->registry[logger->name] = logger;
    }
  };


  // This is for convenience sake in C++ code to
  // shorten logging::Logging::logging()->getLogger(name) (what am I doing)
  //         namespc::class::singleton -> method
  // to Logging::getLogger(name)
  Logger* getLogger(std::string name, sdict config={}){
    return Logging::logging()->getLogger(name, config);
  }


#ifdef Py_PYTHON_H
  static void logging_bindings(py::module& m) {
    py::class_<Logging, std::unique_ptr<Logging, py::nodelete>>(m, "Logging")
      .def(py::init([](){ return std::unique_ptr<Logging, py::nodelete>(Logging::logging()); }))
      .def("setConfig", &Logging::setConfig)
      .def("getLogger", [](py::str name) -> py::object {
        py::module_ logging = py::module_::import("logging");
        py::object pylogger = logging.attr("getLogger")(name);
        Logging::logging()->register_logger(new PyLogger(pylogger));
        return pylogger;
      });
  }
#endif /* Py_PYTHON_H */
} // namespace logging


namespace core {
  // CPP logger, acting as a hook to a Python logger via name matching, will
  // follow Python config
  void run_hook(){
    auto logger = logging::Logging::getLogger("kbmod_logger.module1");
    logger->debug("Test debug logger hooked onto module 1 from C++");
    logger->info("Test info logger hooked onto module 1 from C++.");
    logger->warning("Test warning logger hooked onto module 1 from C++.");
    logger->error("Test error logger hooked onto module 1 from C++.");
    logger->critical("Test critical logger hooked onto module 1  from C++");
  }

  // CPP logger, unaware of Python's existence, would not follow Python config
  void run_pure(){
    auto logger = logging::Logging::getLogger("cpp.logger");
    logger->debug("Test debug new C++ logger from C++ or Python.");
    logger->info("Test info new C++ logger from C++ or Python.");
    logger->warning("Test warning new C++ logger from C++ or Python.");
    logger->error("Test error new C++ from C++ or Python.");
    logger->critical("Test critical new C++ from C++ or Python.");
  }
}
#endif // KBMOD_LOGGER
