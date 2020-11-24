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
      gcc-10

RUN update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-9 100

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
