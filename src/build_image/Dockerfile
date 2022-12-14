FROM gcc:11

MAINTAINER artiomn

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
  && gpg --fetch-keys https://neilalexander.s3.eu-west-2.amazonaws.com/deb/key.txt \
  && gpg --export 569130E8CA20FBC4CB3FDE555898470A764B32C9 | apt-key add - \
  && echo 'deb http://neilalexander.s3.eu-west-2.amazonaws.com/deb/ debian yggdrasil' >> /etc/apt/sources.list.d/yggdrasil.list \
  && apt-get update \
  && apt-get -y --no-install-recommends --show-progress install \
    capnproto \
    clang \
    cmake \
    gdb \
    gdbserver \
    gosu \
    inetutils-ping \
    iproute2 \
    libboost-all-dev \
    libgtest-dev \
    libglib2.0-dev \
    libgtk2.0-dev \
    libgtk-3-dev \
    libpcap-dev \
    libevent-2\* libevent-dev libev-dev libuv1-dev \
    libc-ares-dev \
    libpoco-dev \
    libtorrent-dev \
    libtorrent-rasterbar-dev \
    less \
    meson \
    mpi-default-dev \
    netcat \
    net-tools \
    protobuf-compiler \
    python \
    qtbase5-dev \
    rapidjson-dev \
    rsync \
    sudo \
    ssh \
    tar \
    telnet \
    tcpdump \
    valgrind \
    vim \
    yggdrasil \
  && git clone --recursive --depth 1 https://github.com/artiomn/pistache \
  && cd pistache \
  && cmake \
    -DPISTACHE_USE_SSL=true \
    -DPISTACHE_BUILD_EXAMPLES=true \
    -DPISTACHE_BUILD_TESTS=false \
    -DPISTACHE_BUILD_DOCS=false \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    . \
  && meson setup build \
    --buildtype=debug \
    -DPISTACHE_USE_SSL=true \
    -DPISTACHE_BUILD_EXAMPLES=false \
    -DPISTACHE_BUILD_TESTS=false \
    -DPISTACHE_BUILD_DOCS=false \
    --prefix="/usr/local" \
  && meson compile -C build \
  && meson install -C build \
  && cd / \
  && rm -rf pistache \
  && git clone --recursive --depth 1 https://github.com/corvusoft/restbed.git \
  && cd restbed \
  && cmake -B build -DBUILD_SSL=YES -DBUILD_TESTS=NO -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf restbed \
  && git clone --recursive --depth 1 https://github.com/oatpp/oatpp.git \
  && cd oatpp \
  && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf oatpp \
  && git clone --recursive --depth 1 https://github.com/oatpp/oatpp-websocket.git \
  && cd oatpp-websocket \
  && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf oatpp-websocket \
  && git clone --recursive --depth 1 https://github.com/oatpp/oatpp-swagger.git \
  && cd oatpp-swagger \
  && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf oatpp-swagger \
  && git clone --recursive --depth 1 https://github.com/oatpp/oatpp-curl.git \
  && cd oatpp-curl \
  && cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf oatpp-curl \
  && git clone --recursive --depth 1 https://github.com/Thalhammer/jwt-cpp \
  && cd jwt-cpp \
  && cmake -B build -DBUILD_SSL=YES -DBUILD_TESTS=NO -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && rm -rf jwt-cpp \
  && cd / \
  && git clone --recursive --depth 1 https://github.com/reo7sp/tgbot-cpp \
  && cd tgbot-cpp \
  && cmake . \
  && cmake --build . --parallel \
  && cmake --install . --prefix "/usr/local" \
  && cd / \
  && rm -rf tgbot-cpp \
  && ldconfig \
  && apt-get clean

COPY ./entry.sh /
RUN chmod +x /entry.sh
COPY ./bashrc /etc/skel/.bashrc
COPY ./yggdrasil.conf /etc/yggdrasil.conf

ENTRYPOINT ["/entry.sh"]
CMD ["/bin/bash"]
