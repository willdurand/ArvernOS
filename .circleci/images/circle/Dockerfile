# This image should only be used for ArvernOS on Circle CI.
FROM cimg/base:2022.02

COPY tools/install-linux-deps /tmp/

RUN sudo add-apt-repository ppa:canonical-server/server-backports && \
    sudo /tmp/install-linux-deps && \
    cd /usr/local/lib && \
    wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2" | sudo tar -xj

ENV PATH="${PATH}:/usr/local/lib/gcc-arm-none-eabi-10-2020-q4-major/bin"

# Clean-up
RUN sudo apt-get clean && sudo rm -rf /var/lib/apt/lists/*
