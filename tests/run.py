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
DIR_TEST_FILES = os.path.join(DIR_TESTS, 'test_files')
FILE_RESULTS = os.path.join(DIR_TESTS, 'results.csv')
FILE_CONFIG = os.path.join(DIR_TESTS, 'config.cfg')

DIR_PROJECT = os.path.normpath(os.path.join(DIR_TESTS, os.path.pardir))
DIR_BUILD = os.path.join(DIR_PROJECT, 'build')


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


def run_test(method, exe_path, test_dir, test_file, timeout):
    test_path = os.path.join(test_dir, test_file)
    original_size = os.path.getsize(test_path)

    cmpr = Compressor(exe_path,
                      test_file,
                      method,
                      timeout,
                      test_dir)

    # Use context manager to clean up files
    with cmpr:
        conclusion, compressed_size = cmpr.run_test()

    return {ORIGINAL_SIZE: original_size,
            COMPRESSED_SIZE: compressed_size,
            CONCLUSION: conclusion}


def run_tests(methods, exe_path, test_dir, timeout=180.0):
    results = []
    for method in methods:
        for test_file in sorted(os.listdir(test_dir)):
            path = os.path.join(test_dir, test_file)
            if not os.path.isfile(path):
                continue

            test_results = run_test(exe_path=exe_path,
                                    test_dir=test_dir,
                                    timeout=timeout,
                                    test_file=test_file,
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
    with open(filename, 'w') as resfile:
        writer = csv.writer(resfile)
        writer.writerow(header)
        for row in results:
            writer.writerow([row[column] for column in header])


def main():
    parser = argparse.ArgumentParser(description='Testing script', prog='test')
    parser.add_argument('--timeout', type=float, default=180.0)
    parser.add_argument('--testdir', type=str, default=DIR_TEST_FILES)

    cmp_exe = 'compress.exe' if platform.system() == 'Windows' else 'compress'
    exe_path = os.path.join(DIR_BUILD, cmp_exe)
    print('Executable: {}'.format(exe_path))

    args = parser.parse_args()

    methods = read_config()
    print('Methods to test: {}'.format(methods))
    print()

    try:
        results = run_tests(methods,
                            exe_path=exe_path,
                            test_dir=args.testdir,
                            timeout=args.timeout)
    except ExecutableNotFoundError as e:
        print('Failed to find executable')
        print(e)
        return

    save_results(results, filename=FILE_RESULTS)


if __name__ == '__main__':
    main()
