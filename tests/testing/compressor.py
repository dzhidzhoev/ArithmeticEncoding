import tempfile
import filecmp
import os
from contextlib import contextmanager, ExitStack

from . import OK, WRONG_ANSWER
from .command import Command


@contextmanager
def temp_filepaths(*args):
    with tempfile.TemporaryDirectory() as temp_dir_name:
        yield [
            os.path.join(temp_dir_name, filename)
            for filename in args
        ]


class CompressorError(Exception):
    def __init__(self, conclusion, *args, compressed_size=None):
        super().__init__(*args)
        self.conclusion = conclusion
        self.compressed_size = compressed_size


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

    def compress(self, compressed_filename):
        return self.execute(
            input_f=self.test_file,
            output_f=compressed_filename,
            mode='c',
            name='compress',
        )

    def decompress(self, compressed_filename, decompressed_filename):
        return self.execute(
            input_f=compressed_filename,
            output_f=decompressed_filename,
            mode='d',
            name='decompress',
        )

    def output_filename(self, mode):
        if self.output_dir:
            decompress_dir = os.path.join(self.output_dir, mode)
            if not os.path.exists(decompress_dir):
                os.mkdir(decompress_dir)
            return os.path.join(decompress_dir, self.test_file)
        else:
            return None

    def run_test(self):
        with temp_filepaths('compressed', 'decompressed') as (compressed, decompressed):
            compressed_filename = compressed
            decompressed_filename = decompressed
            try:
                compressed_size = self._run_test(
                    compressed_filename,
                    decompressed_filename,
                )
                return OK, compressed_size
            except CompressorError as e:
                return e.conclusion, e.compressed_size

    def _run_test(self, compressed_filename, decompressed_filename):
        self._run_compress(compressed_filename)
        self._run_decompress(compressed_filename, decompressed_filename)

        try:
            compressed_size = os.path.getsize(compressed_filename)
        except FileNotFoundError:
            raise CompressorError(WRONG_ANSWER + '2')

        if not filecmp.cmp(self.test_path,
                           decompressed_filename,
                           shallow=False):
            raise CompressorError(
                WRONG_ANSWER + '2',
                compressed_size=compressed_size,
            )

        return compressed_size

    def _run_compress(self, compressed_filename):
        err_code = self.compress(compressed_filename)
        if err_code != OK:
            raise CompressorError(err_code + '1')

        if not os.path.exists(compressed_filename):
            raise CompressorError(WRONG_ANSWER + '1')

    def _run_decompress(self, compressed_filename, decompressed_filename):

        err_code = self.decompress(compressed_filename, decompressed_filename)
        if err_code != OK:
            raise CompressorError(err_code + '2')

        if not os.path.exists(compressed_filename) \
                or not os.path.exists(decompressed_filename):
            raise CompressorError(WRONG_ANSWER + '2')
