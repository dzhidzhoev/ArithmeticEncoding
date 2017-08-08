import threading
import subprocess

from . import TIME_LIMIT, OK, RUNTIME_ERROR


class Command:
    def __init__(self, args):
        self.args = args
        self.process = None

    def run(self, timeout, cwd):
        def target():
            try:
                self.process = subprocess.Popen(self.args, cwd=cwd)
                self.process.communicate()
            except OSError as e:
                self.process.returncode = e

        thread = threading.Thread(target=target)
        thread.start()
        thread.join(timeout)

        if thread.is_alive():
            self.process.kill()
            thread.join()
            return TIME_LIMIT

        if self.process.returncode == 0:
            return OK
        else:
            return RUNTIME_ERROR
