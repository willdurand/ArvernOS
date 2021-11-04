FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

COPY tools/install-linux-deps /tmp
RUN /tmp/install-linux-deps

RUN wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" | tar -xj
ENV PATH="${PATH}:/gcc-arm-none-eabi-10-2020-q4-major/bin"
ENV CROSS_COMPILE=/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
