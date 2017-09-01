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

    def run(self, output_file, *, timeout, working_directory):
        if output_file is None:
            return self._run(subprocess.DEVNULL, timeout, working_directory)

        with open(output_file, 'w') as f:
            return self._run(f, timeout, working_directory)

    def _run(self, file_obj, timeout, working_directory):
        try:
            process = subprocess.Popen(self.args,
                                       cwd=working_directory,
                                       stdout=file_obj,
                                       stderr=file_obj)
        except FileNotFoundError as e:
            raise ExecutableNotFoundError(str(e))
        except OSError as e:
            raise e

        #
        # communicate() example
        # https://docs.python.org/3/library/subprocess.html#subprocess.Popen.communicate
        #

        try:
            process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            process.kill()
            process.communicate()
            return TIME_LIMIT

        if process.returncode == 0:
            return OK
        else:
            return RUNTIME_ERROR
