ARG VARIANT="ubuntu-22.04"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

RUN apt-get update && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends \
        gcc-arm-none-eabi \
        libstdc++-arm-none-eabi-newlib \
        cmake \
        build-essential \
        wget \
        ca-certificates \
        gdb-multiarch \
        binutils-multiarch \
        automake \
        autoconf \
        libtool \
        libftdi-dev \
        libusb-1.0-0-dev \
        libhidapi-dev \
        pkg-config \
        clang-format \
        python3 \
        python3-tk \
        erlang \
        rebar3

WORKDIR /apps

RUN cd /apps && \
    git clone -b master https://github.com/raspberrypi/pico-sdk.git --depth=1 && \
    cd pico-sdk && \
    git submodule update --init

ENV PICO_SDK_PATH=/apps/pico-sdk

RUN cd /apps && \
    git clone -b master https://github.com/raspberrypi/pico-extras.git --depth=1 \
    && cd pico-extras && \
    git submodule update --init 

ENV PICO_EXTRAS_PATH=/apps/pico-extras

RUN cd /apps && git clone https://github.com/raspberrypi/picotool.git --depth=1 && \
    cd picotool && mkdir build && cd build && cmake ../ && make -j 8 && cp picotool /usr/local/bin 
RUN cd /apps && git clone https://github.com/wtarreau/bootterm.git --depth=1 && \
    cd bootterm && make -j 8 install 
RUN git clone https://github.com/pguyot/uf2tool.git --depth=1 && \
    cd uf2tool && rebar3 escriptize && \
    cp _build/default/bin/uf2tool /usr/local/bin

RUN cd /usr/bin && ln -s /usr/bin/objdump objdump-multiarch && ln -s /usr/bin/nm nm-multiarch 

RUN usermod -aG plugdev vscode
