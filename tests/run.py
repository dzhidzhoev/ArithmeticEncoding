from collections import defaultdict
import os
import filecmp
import platform
import argparse
import json


class Compressor:
    def __init__(self):
        pass


def read_config(filename='config.cfg'):
    if not os.path.isfile(filename):
        methods = ['ari']
    else:
        with open(filename, 'r') as f:
            methods = json.load(f)
        assert type(methods) == list
        assert all(type(method) == str for method in methods)
        methods = [method.lower() for method in methods]

    return methods


def run_test(method, exe, testdir, test_file, timeout):
    conclusion = None
    test_path = os.path.join(testdir, test_file)
    size_before = os.path.getsize(test_path)

    # [RE1, TL1]
    cmpr_args = [os.path.abspath(exe),
                 '--input',  test_file,
                 '--output', test_file + '.cmp',
                 '--mode',   'c',
                 '--method', method]

    command_compress = Command(cmpr_args)
    cmpr_err_code = command_compress.run(timeout, cwd=testdir)

    if cmpr_err_code == OK:
        # [RE2, TL2]
        dcmp_args = [os.path.abspath(exe),
                     '--input',  test_file + '.cmp',
                     '--output', test_file + '.dcm',
                     '--mode',   'd',
                     '--method', method]

        command_decompress = Command(dcmp_args)
        dcmp_err_code = command_decompress.run(timeout, cwd=testdir)

        if dcmp_err_code == OK:
            size_after = os.path.getsize(test_path + '.cmp')

            # [OK, WA]
            if conclusion is None:
                if filecmp.cmp(test_path,
                               test_path + '.dcm',
                               shallow=False):
                    conclusion = OK
                else:
                    conclusion = WRONG_ANSWER

            os.remove(test_path + '.cmp')
            os.remove(test_path + '.dcm')

        else:
            if os.path.isfile(test_path + '.cmp'):
                os.remove(test_path + '.cmp')

            if os.path.isfile(test_path + '.dcm'):
                os.remove(test_path + '.dcm')

            size_after = '-'
            conclusion = dcmp_err_code + '2'

    else:
        if os.path.isfile(test_path + '.cmp'):
            os.remove(test_path + '.cmp')

        size_after = '-'
        conclusion = cmpr_err_code + '1'

    return {'file': test_file,
            'size': size_before,
            'compressed': size_after,
            'conclusion': conclusion}


def run_tests(methods, exe, testdir, timeout=180.0):
    res = defaultdict(list)
    for method in methods:
        for test_file in sorted(os.listdir(testdir)):
            path = os.path.join(testdir, test_file)
            if os.path.isfile(path):
                res[method].append(run_test(exe=exe,
                                            testdir=testdir,
                                            timeout=timeout,
                                            test_file=test_file,
                                            method=method))
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
