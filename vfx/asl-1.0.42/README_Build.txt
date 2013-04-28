Building Asl:
-------------

install boost. ( system, threads, signals, filesystem, datetime)
install tbb.

cd asl-1.0.42/
mkdir build
cd build
cmake .. or cmake-gui ..
make
make install (by default on /usr/local, can be changed in cmake or cmake-gui)

