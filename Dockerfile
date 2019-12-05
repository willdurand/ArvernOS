FROM ubuntu:latest

RUN apt-get update && apt-get install -y nasm xorriso build-essential grub2-common

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
