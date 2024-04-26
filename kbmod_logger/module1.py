import kbmod_logger
import logging


__all__ = ["run", ]


logger = logging.getLogger(__name__)
logger.setLevel(logging.CRITICAL)
cpplogger = kbmod_logger.Logging.getLogger(__name__)

def run():
    print()
    print("       MODULE 1    ")
    print("Expectation is that the first 2 print only CRITICAL level warnings.")
    print("Third one formatting of the message may differ. Module of origin should be")
    print("kbmod_logger.module1 for the first 2 and cpp.logger for the last outputs")
    print()
    logger.debug("Test debug Python logger from Python.")
    logger.info("Test info Python logger from Python.")
    logger.warning("Test warning Python logger from Python.")
    logger.error("Test error Python logger from Python.")
    logger.critical("Test critical Python logger from Python.")
    print()
    kbmod_logger.run_hook()
    print()
    kbmod_logger.run_pure()
