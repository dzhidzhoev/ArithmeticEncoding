all: tests

tests:
	docker build . -t compressor_tests
	docker run compressor_tests

build-base:
	docker build . -f Dockerfile.base -t grihabor/compressor_base

.PHONY: all tests
