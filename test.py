import os
import threading
import subprocess
import filecmp
import platform
import argparse

from json import load

TIME_LIMIT = 'time_limit'
RUNTIME_ERROR = 'runtime_error'
OK = 'ok'

class Command(object):
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


def read_config(filename='config.cfg'):
    if not os.path.isfile(filename):
        methods = ['ari']
    else:
        methods = load(open(filename))
        for i in range(len(methods)):
            methods[i] = methods[i].lower()

    return methods


def run_tests(methods, exe, testdir, timeout=180.0):
    res = {}
    for method in methods:
        res.update({method: []})
        for test_file in sorted(os.listdir(testdir)):
            if os.path.isfile(os.path.join(testdir, test_file)):
                conclusion = None

                size_before = os.path.getsize(os.path.join(testdir, test_file))

                # [RE1, TL1]
                cmpr_args = [os.path.abspath(exe),
                             '--input',  test_file,
                             '--output', test_file + '.cmp', 
                             '--mode',   'c',
                             '--method', method]

                command_compress = Command(cmpr_args)
                cmpr_err_code = command_compress.run(timeout, cwd=testdir)

                if cmpr_err_code == 'OK':
                    # [RE2, TL2]
                    dcmp_args = [os.path.abspath(exe),
                                 '--input',  test_file + '.cmp',
                                 '--output', test_file + '.dcm', 
                                 '--mode',   'd',
                                 '--method', method]

                    command_decompress = Command(dcmp_args)
                    dcmp_err_code = command_decompress.run(timeout, cwd=testdir)

                    if dcmp_err_code == 'OK':
                        size_after = os.path.getsize(os.path.join(testdir, test_file + '.cmp'))

                        # [OK, WA]
                        if conclusion is None:
                            if filecmp.cmp(os.path.join(testdir, test_file),
                                           os.path.join(testdir, test_file + '.dcm'),
                                           shallow=False):
                                conclusion = 'OK'
                            else:
                                conclusion = 'WA'

                        os.remove(os.path.join(testdir, test_file + '.cmp'))
                        os.remove(os.path.join(testdir, test_file + '.dcm'))

                    else:
                        if os.path.isfile(os.path.join(testdir, test_file + '.cmp')):
                            os.remove(os.path.join(testdir, test_file + '.cmp'))

                        if os.path.isfile(os.path.join(testdir, test_file + '.dcm')):
                            os.remove(os.path.join(testdir, test_file + '.dcm'))

                        size_after = '-'
                        conclusion = dcmp_err_code + '2'

                else:
                    if os.path.isfile(os.path.join(testdir, test_file + '.cmp')):
                        os.remove(os.path.join(testdir, test_file + '.cmp'))

                    size_after = '-'
                    conclusion = cmpr_err_code + '1'

                res[method].append({'file': test_file,
                                    'size': size_before,
                                    'compressed': size_after,
                                    'conclusion': conclusion})
    return res


def save_results(res, filename='res.csv'):
    with open(filename, 'w') as resfile:
        resfile.write('method,file,size,compressed,conclusion\n')
        for key, value in res.items():
            template = key + ',{},{},{},{}\n'
            for sample in value:
                resfile.write(template.format(sample['file'],
                                              sample['size'],
                                              sample['compressed'],
                                              sample['conclusion']))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Testing script', prog='test')
    parser.add_argument('--timeout', type=float, default=180.0)
    parser.add_argument('--testdir', type=str, default='./tests')

    if platform.system() == 'Windows':
        cmp_exe = './build/compress.exe'
    else:
        cmp_exe = './build/compress'

    args = parser.parse_args()

    methods = read_config()
    res = run_tests(methods, exe=cmp_exe, testdir=args.testdir, timeout=args.timeout)
    save_results(res)
