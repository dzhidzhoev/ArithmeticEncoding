import filecmp
import os

from . import OK, WRONG_ANSWER
from .command import Command


class Compressor:
    def __init__(self, *, exe_path, test_file, method, timeout, test_dir, output_dir):
        self.exe_path = exe_path
        self.test_file = test_file
        self.method = method
        self.timeout = timeout
        self.test_dir = test_dir
        self.test_path = os.path.join(self.test_dir, self.test_file)
        self.output_dir = output_dir

    def execute(self, input_f, output_f, mode, name):
        args = [self.exe_path,
                '--input', input_f,
                '--output', output_f,
                '--mode', mode,
                '--method', self.method]

        cmd = Command(args)
        err_code = cmd.run(output_file=self.output_filename(name),
                           timeout=self.timeout,
                           working_directory=self.test_dir)
        return err_code

    def compress(self):
        return self.execute(input_f=self.test_file,
                            output_f=self.test_file + '.cmp',
                            mode='c',
                            name='compress')

    def decompress(self):
        return self.execute(input_f=self.test_file + '.cmp',
                            output_f=self.test_file + '.dcm',
                            mode='d',
                            name='decompress')

    def output_filename(self, mode):
        if self.output_dir:
            decompress_dir = os.path.join(self.output_dir, mode)
            if not os.path.exists(decompress_dir):
                os.mkdir(decompress_dir)
            return os.path.join(decompress_dir, self.test_file)
        else:
            return None

    def clean(self):
        # Use try-except to prevent async errors
        try:
            os.remove(self.test_path + '.cmp')
            os.remove(self.test_path + '.dcm')
        except FileNotFoundError:
            pass

    def run_test(self):
        err_code = self.compress()
        if err_code != OK:
            return err_code + '1', None

        err_code = self.decompress()
        if err_code != OK:
            return err_code + '2', None

        compressed_size = os.path.getsize(self.test_path + '.cmp')

        if filecmp.cmp(self.test_path,
                       self.test_path + '.dcm',
                       shallow=False):
            return OK, compressed_size
        return WRONG_ANSWER, compressed_size

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.clean()
