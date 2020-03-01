FROM ubuntu:latest

RUN apt-get update && apt-get install -y nasm xorriso build-essential grub2-common grub-pc-bin git clang-format-9 valgrind

RUN update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-9 100

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
