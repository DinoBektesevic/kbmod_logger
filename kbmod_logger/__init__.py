import time
import logging

from logger import *


# Shared log configuration for C++ and Python logs
__log_config = {
    "level": "DEBUG",
    "format": "[%(asctime)s SHARED_CONF %(levelname)s %(name)s] %(message)s",
    "datefmt": "%Y-%m-%dT%H:%M:%SZ"

}
logging.Formatter.converter = time.gmtime
logging.basicConfig(**__log_config)
__log_config["converter"] = "gmtime"
Logging().setConfig(__log_config)


from . import module1
from . import module2
