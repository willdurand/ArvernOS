FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

COPY tools/install-linux-deps /tmp/

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:canonical-server/server-backports && \
    /tmp/install-linux-deps && \
    cd /usr/local/lib && \
    wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" | tar -xj

ENV PATH="${PATH}:/usr/local/lib/gcc-arm-none-eabi-10-2020-q4-major/bin"

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
