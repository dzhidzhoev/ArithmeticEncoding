IMAGE=compressor_tests:latest

all: run

# Testing

build:
	docker build . -t $(IMAGE)

no-cache:
	docker build . -t $(IMAGE) --no-cache

run: build
	docker run $(IMAGE)

sh: build
	docker run -it $(IMAGE) /bin/sh

# Base image stuff

base-build:
	docker build . \
		-f Dockerfile.base \
		-t grihabor/compressor_base:latest

base-push:
	docker push grihabor/compressor_base:latest

base-build-and-push: base-build base-push


.PHONY: all build run no-cache sh base-build base-push base-build-and-push
