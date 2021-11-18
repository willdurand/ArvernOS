FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

COPY tools/install-linux-deps /tmp/
RUN  /tmp/install-linux-deps

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
