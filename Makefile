IMAGE=compressor_tests

all: build-and-run

build-and-run:
	docker build . -t $(IMAGE)
	make run

no-cache:
	docker build . -t $(IMAGE) --no-cache
	make run

run:
	docker run $(IMAGE)


# Base image stuff

base-build:
	docker build . \
		-f Dockerfile.base \
		-t grihabor/compressor_base:latest

base-push:
	docker push grihabor/compressor_base:latest

base-build-and-push: base-build base-push

sh:
	docker run -it $(IMAGE) /bin/sh


.PHONY: all build-and-run no-cache run base-build base-push base-build-and-push
