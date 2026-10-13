FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

COPY tools/install-linux-deps /tmp/

# The GNU Arm Embedded toolchain is published per host architecture, so pick
# the tarball that matches the platform we are building for: `amd64` hosts use
# the `x86_64` build and `arm64` hosts use the `aarch64` one.
RUN /tmp/install-linux-deps && \
    cd /usr/local/lib && \
    case "$(dpkg --print-architecture)" in \
      amd64) toolchain_host=x86_64 ;; \
      arm64) toolchain_host=aarch64 ;; \
      *) echo >&2 "unsupported architecture: $(dpkg --print-architecture)"; exit 1 ;; \
    esac && \
    wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-${toolchain_host}-linux.tar.bz2" | bsdtar -xj

ENV PATH="${PATH}:/usr/local/lib/gcc-arm-none-eabi-10-2020-q4-major/bin"

RUN groupadd -r simpleuser && useradd --no-log-init -r -g simpleuser simpleuser

USER simpleuser

WORKDIR /app
