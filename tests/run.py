import csv
from collections import defaultdict
import os
import platform
import argparse
import json

from tests.testing.compressor import Compressor

METHOD = 'method'
FILE = 'file'
ORIGINAL_SIZE = 'original_size'
COMPRESSED_SIZE = 'compressed_size'
CONCLUSION = 'conclusion'


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


def save_results(results, filename='res.csv'):
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
    res = run_tests(methods,
                    exe_path=os.path.abspath(cmp_exe),
                    test_dir=args.testdir,
                    timeout=args.timeout)
    save_results(res)
