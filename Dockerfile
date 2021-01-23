FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install -y \
      build-essential \
      nasm \
      xorriso \
      grub2-common \
      grub-pc-bin \
      git \
      clang-format-9 \
      valgrind \
      gcc-10 \
      qemu-system \
      wget \
      bzip2 \
      bison \
      flex \
      bc && \
    apt-get clean

RUN update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-9 100
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10

RUN wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" | tar -xj
ENV PATH="${PATH}:/gcc-arm-none-eabi-10-2020-q4-major/bin"
ENV CROSS_COMPILE=/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
