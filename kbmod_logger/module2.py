import kbmod_logger
import logging


__all__ = ["run", ]


logger = logging.getLogger(__name__)
cpplogger = kbmod_logger.Logging.getLogger(__name__)


def run():
    print()
    print("       MODULE 2    ")
    print("Expectation is that they will all print everything and ")
    print("that the module of origin will be kbmod_logger.module2.")
    print()
    logger.debug("Test debug Python logger from Python.")
    logger.info("Test info Python logger from Python.")
    logger.warning("Test warning Python logger from Python.")
    logger.error("Test error Python logger from Python.")
    logger.critical("Test critical Python logger from Python.")
    print()
    cpplogger.debug("Test debug C++ logger obj from Python.")
    cpplogger.info("Test info C++ logger obj from Python.")
    cpplogger.warning("Test warning C++ logger obj from Python.")
    cpplogger.error("Test error C++ logger obj from Python.")
    cpplogger.critical("Test critical C++ logger obj from Python.")
