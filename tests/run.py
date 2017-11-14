import csv
import os
import platform
import argparse
import json

from testing import Compressor, ExecutableNotFoundError

METHOD = 'method'
FILE = 'file'
ORIGINAL_SIZE = 'original_size'
COMPRESSED_SIZE = 'compressed_size'
CONCLUSION = 'conclusion'

DIR_TESTS = os.path.split(os.path.abspath(__file__))[0]
DIR_ROOT = os.path.normpath(os.path.join(DIR_TESTS, os.pardir))
DIR_TEST_FILES = os.path.join(DIR_ROOT, 'test_files')
DIR_TEST_OUTPUT = os.path.join(DIR_TESTS, 'output')
FILE_RESULTS = os.path.join(DIR_ROOT, 'results.csv')
FILE_CONFIG = os.path.join(DIR_TESTS, 'config.cfg')

DIR_PROJECT = os.path.normpath(os.path.join(DIR_TESTS, os.path.pardir))
DIR_BUILD = os.path.join(DIR_PROJECT, 'build')


class TestDirectoryNotFoundError(FileNotFoundError):
    pass


def read_config(filename=FILE_CONFIG):
    if not os.path.isfile(filename):
        methods = ['ari']
        print('Failed to find config file: {}'.format(filename))
    else:
        with open(filename, 'r') as f:
            methods = json.load(f)
        assert type(methods) == list
        assert all(type(method) == str for method in methods)
        methods = [method.lower() for method in methods]

    return methods


def run_test(*, method, exe_path, test_dir, test_file, timeout, output_dir):
    test_path = os.path.join(test_dir, test_file)
    original_size = os.path.getsize(test_path)

    cmpr = Compressor(exe_path=exe_path,
                      test_file=test_file,
                      method=method,
                      timeout=timeout,
                      test_dir=test_dir,
                      output_dir=output_dir)

    conclusion, compressed_size = cmpr.run_test()

    return {ORIGINAL_SIZE: original_size,
            COMPRESSED_SIZE: compressed_size,
            CONCLUSION: conclusion}


def run_tests(methods, exe_path, test_dir, output_dir, timeout=180.0):
    results = []
    if not os.path.isdir(test_dir):
        raise TestDirectoryNotFoundError('Failed to find {}'.format(test_dir))

    if output_dir and not os.path.exists(output_dir):
        os.mkdir(output_dir)

    for method in methods:
        if output_dir:
            method_dir = os.path.join(output_dir, method)
            if not os.path.exists(method_dir):
                os.mkdir(method_dir)
        else:
            method_dir = None

        for test_file in sorted(os.listdir(test_dir)):
            test_path = os.path.join(test_dir, test_file)
            if not os.path.isfile(test_path):
                continue

            test_results = run_test(exe_path=exe_path,
                                    test_dir=test_dir,
                                    timeout=timeout,
                                    test_file=test_file,
                                    output_dir=method_dir,
                                    method=method)
            test_results.update({METHOD: method,
                                 FILE: test_file})
            results.append(test_results)
    return results


def save_results(results, filename=FILE_RESULTS):
    header = [METHOD,
              FILE,
              ORIGINAL_SIZE,
              COMPRESSED_SIZE,
              CONCLUSION]
    with open(filename, 'w', newline='') as resfile:
        writer = csv.writer(resfile)
        writer.writerow(header)
        resfile.flush()
        for row in results:
            writer.writerow([row[column] for column in header])
            resfile.flush()


def main():
    parser = argparse.ArgumentParser(description='Testing script', prog='test')
    parser.add_argument('--timeout', type=float, default=180.0)
    parser.add_argument('--testdir', type=str, default=DIR_TEST_FILES)

    parser.add_argument('--outputdir', type=str, default='')
    parser.add_argument('--output', action='store_true')

    cmp_exe = 'compress.exe' if platform.system() == 'Windows' else 'compress'
    exe_path = os.path.join(DIR_BUILD, cmp_exe)
    print('Executable: {}'.format(exe_path))

    args = parser.parse_args()
    if not args.outputdir:
        args.outputdir = DIR_TEST_OUTPUT
    else:
        args.output = True

    methods = read_config()
    print('Methods to test: {}'.format(methods))

    try:
        results = run_tests(methods,
                            exe_path=exe_path,
                            test_dir=args.testdir,
                            output_dir=args.outputdir if args.output else None,
                            timeout=args.timeout)
    except ExecutableNotFoundError as e:
        print('Failed to find executable')
        print(e)
        return
    except TestDirectoryNotFoundError as e:
        print('Failed to find test directory')
        print(e)
        return

    save_results(results, filename=FILE_RESULTS)
    print('Results saved to: {}'.format(FILE_RESULTS))

if __name__ == '__main__':
    main()
