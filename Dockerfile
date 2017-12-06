FROM grihabor/compressor_base:latest AS builder_image
MAINTAINER Borodin Gregory <grihabor@mail.ru>

ADD src /project/src

RUN mkdir /project/build \
 && cd /project/build \
 && CMAKE_MAKE_PROGRAM=make cmake /project/src \
 && make


FROM library/python:3.6-alpine
MAINTAINER Borodin Gregory <grihabor@mail.ru>

RUN apk update \
 && apk add libstdc++

ADD tests /project/tests
ADD test_files /project/test_files

ADD src /project/src
COPY --from=builder_image /project/build/compress /project/build/compress

CMD python3 /project/tests/run.py && cat /project/results.csv
