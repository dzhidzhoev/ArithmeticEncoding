from .command import Command


class Compressor:
    def __init__(self, exe_path, test_file, method, timeout, test_dir):
        self.exe_path = exe_path
        self.test_file = test_file
        self.method = method
        self.timeout = timeout
        self.test_dir = test_dir

    def compress(self):
        args = [self.exe_path,
                '--input', self.test_file,
                '--output', self.test_file + '.cmp',
                '--mode', 'c',
                '--method', self.method]

        cmd = Command(args)
        err_code = cmd.run(self.timeout, cwd=self.test_dir)
        return err_code

    def decompress(self):
        args = [self.exe_path,
                '--input', self.test_file + '.cmp',
                '--output', self.test_file + '.dcm',
                '--mode', 'd',
                '--method', self.method]

        cmd = Command(args)
        err_code = cmd.run(self.timeout, cwd=self.test_dir)
        return err_code
