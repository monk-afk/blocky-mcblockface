FROM alpine:3.14

ENV MINETEST_GAME_VERSION master

COPY .git /usr/src/blocky-mcblockface/.git
COPY CMakeLists.txt /usr/src/blocky-mcblockface/CMakeLists.txt
COPY README.md /usr/src/blocky-mcblockface/README.md
COPY mcblockface.conf.example /usr/src/blocky-mcblockface/mcblockface.conf.example
COPY builtin /usr/src/blocky-mcblockface/builtin
COPY cmake /usr/src/blocky-mcblockface/cmake
COPY doc /usr/src/blocky-mcblockface/doc
COPY lib /usr/src/blocky-mcblockface/lib
COPY po /usr/src/blocky-mcblockface/po
COPY src /usr/src/blocky-mcblockface/src

WORKDIR /usr/src/blocky-mcblockface

RUN apk add --no-cache git build-base irrlicht-dev cmake sqlite-dev curl-dev \
		zlib-dev gmp-dev jsoncpp-dev postgresql-dev luajit-dev ca-certificates && \
	git clone --depth=1 -b ${MINETEST_GAME_VERSION} https://github.com/luanti-org/minetest_game.git ./games/minetest_game && \
	rm -fr ./games/minetest_game/.git

WORKDIR /usr/src/

RUN git clone --branch v1.2.4 --depth 1 --recursive https://github.com/jupp0r/prometheus-cpp/ && \
	mkdir prometheus-cpp/build && \
	cd prometheus-cpp/build && \
	cmake .. \
		-DCMAKE_INSTALL_PREFIX=/usr/local \
		-DCMAKE_BUILD_TYPE=Release \
		-DENABLE_TESTING=0 && \
	make -j2 && \
	make install

WORKDIR /usr/src/blocky-mcblockface
RUN mkdir build && \
	cd build && \
	cmake .. \
		-DCMAKE_INSTALL_PREFIX=/usr/local \
		-DCMAKE_BUILD_TYPE=Release \
		-DENABLE_PROMETHEUS=TRUE \
		-DBUILD_UNITTESTS=FALSE && \
	make -j2 && \
	make install

FROM alpine:3.14

RUN apk add --no-cache sqlite-libs curl gmp libstdc++ libgcc libpq luajit && \
	adduser -D mcblockface --uid 30000 -h /var/lib/mcblockface && \
	chown -R mcblockface:mcblockface /var/lib/mcblockface

WORKDIR /var/lib/mcblockface

COPY --from=0 /usr/local/share/mcblockface /usr/local/share/mcblockface
COPY --from=0 /usr/local/bin/mcblockface /usr/local/bin/mcblockface
COPY --from=0 /usr/local/share/doc/mcblockface/mcblockface.conf.example /etc/mcblockface/mcblockface.conf

USER mcblockface:mcblockface

EXPOSE 30000/udp 30000/tcp

CMD ["/usr/local/bin/mcblockface", "--config", "/etc/mcblockface/mcblockface.conf"]
