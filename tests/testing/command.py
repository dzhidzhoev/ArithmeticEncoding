import threading
import subprocess

from . import TIME_LIMIT, OK, RUNTIME_ERROR


class ExecutableNotFoundError(FileNotFoundError):
    pass


class Command:
    def __init__(self, args):
        self.args = args
        self.return_code = None
        self.process = None

    def run(self, timeout, working_directory):
        try:
            process = subprocess.Popen(self.args,
                                       cwd=working_directory,
                                       stdout=subprocess.DEVNULL,
                                       stderr=subprocess.DEVNULL)
        except FileNotFoundError as e:
            raise ExecutableNotFoundError(str(e))
        except OSError as e:
            raise e

        try:
            output = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired as e:
            process.kill()
            return TIME_LIMIT

        if process.returncode == 0:
            return OK
        else:
            return RUNTIME_ERROR
