FROM alpine:3.14

ENV MINETEST_GAME_VERSION master

COPY .git /usr/src/squareone-core/.git
COPY CMakeLists.txt /usr/src/squareone-core/CMakeLists.txt
COPY README.md /usr/src/squareone-core/README.md
COPY squareone.conf.example /usr/src/squareone-core/squareone.conf.example
COPY builtin /usr/src/squareone-core/builtin
COPY cmake /usr/src/squareone-core/cmake
COPY doc /usr/src/squareone-core/doc
COPY fonts /usr/src/squareone-core/fonts
COPY lib /usr/src/squareone-core/lib
COPY misc /usr/src/squareone-core/misc
COPY po /usr/src/squareone-core/po
COPY src /usr/src/squareone-core/src
COPY textures /usr/src/squareone-core/textures

WORKDIR /usr/src/squareone-core

RUN apk add --no-cache git build-base irrlicht-dev cmake bzip2-dev libpng-dev \
		jpeg-dev libxxf86vm-dev mesa-dev sqlite-dev libogg-dev \
		libvorbis-dev openal-soft-dev curl-dev freetype-dev zlib-dev \
		gmp-dev jsoncpp-dev postgresql-dev luajit-dev ca-certificates && \
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

WORKDIR /usr/src/squareone-core
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
	adduser -D squareone --uid 30000 -h /var/lib/squareone && \
	chown -R squareone:squareone /var/lib/squareone

WORKDIR /var/lib/squareone

COPY --from=0 /usr/local/share/squareone /usr/local/share/squareone
COPY --from=0 /usr/local/bin/squareoneserver /usr/local/bin/squareoneserver
COPY --from=0 /usr/local/share/doc/squareone/squareone.conf.example /etc/squareone/squareone.conf

USER squareone:squareone

EXPOSE 30000/udp 30000/tcp

CMD ["/usr/local/bin/squareoneserver", "--config", "/etc/squareone/squareone.conf"]
