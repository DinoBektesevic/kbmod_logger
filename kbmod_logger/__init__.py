import logging

logging.basicConfig(level=logging.DEBUG,
                    format='[%(asctime)s %(levelname)s %(name)s] %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S')

from logger import *
from . import module1
from . import module2
