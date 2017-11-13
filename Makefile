IMAGE=compressor_tests:latest

all: build-and-run

build-and-run:
	docker build . -t $(IMAGE)
	make run

no-cache:
	docker build . -t $(IMAGE) --no-cache
	make run

run:
	docker run $(IMAGE)

sh:
	docker run -it $(IMAGE) /bin/sh

# Base image stuff

base-build:
	docker build . \
		-f Dockerfile.base \
		-t grihabor/compressor_base:latest

base-push:
	docker push grihabor/compressor_base:latest

base-build-and-push: base-build base-push


.PHONY: all build-and-run no-cache run base-build base-push base-build-and-push
