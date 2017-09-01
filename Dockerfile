FROM grihabor/compressor_base:latest
MAINTAINER Borodin Gregory <grihabor@mail.ru>

ADD src /project/src

RUN mkdir /project/build \
 && cd /project/build \
 && CMAKE_MAKE_PROGRAM=make cmake /project/src \
 && make

ADD tests /project/tests

CMD python3 /project/tests/run.py && cat /project/tests/results.csv
