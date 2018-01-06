import logging
from infra.run.common import *
from infra.core.ansi import Ansi


# LOGOR_LEVEL = logging.DEBUG
LOGOR_LEVEL = logging.INFO
LOGOR_FORMATS = ('%(asctime)s %(name)s %(levelname)s:\n%(message)s', '%d/%m/%y %H:%M:%S')
LOGOR_COLOR_MAP = {
    logging.CRITICAL: (Ansi.YELLOW, Ansi.BACKGROUND_RED,),
    logging.ERROR: (Ansi.RED, Ansi.BRIGHT),
    logging.WARNING: (Ansi.YELLOW, Ansi.BRIGHT),
    logging.INFO: (Ansi.CYAN, Ansi.FAINT),
    logging.DEBUG: (Ansi.YELLOW, Ansi.FAINT),
    'name': (Ansi.GREEN, Ansi.FAINT),
    'levelname': (Ansi.MAGENTA, Ansi.FAINT),
}
