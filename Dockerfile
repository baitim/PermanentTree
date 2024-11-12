FROM ubuntu:latest
WORKDIR /app
COPY . .

RUN apt-get -y update
RUN apt-get -y upgrade
RUN apt-get -y install sudo
RUN apt-get -y install cmake
RUN apt-get -y install pkg-config
RUN apt-get -y install python3
RUN apt-get -y install python3-pip

RUN pip install conan --break-system-packages
RUN conan profile detect --force

RUN conan install . --build=missing -c tools.system.package_manager:mode=install \
    -c tools.system.package_manager:sudo=True -s compiler.cppstd=gnu20 -s build_type=Release
RUN cmake . -B build -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release; cmake --build build
RUN ctest --test-dir build --rerun-failed --output-on-failure

RUN rm -rf build

RUN conan install . --build=missing -c tools.system.package_manager:mode=install \
    -c tools.system.package_manager:sudo=True -s compiler.cppstd=gnu20 -s build_type=Debug
RUN cmake . -B build -DCMAKE_TOOLCHAIN_FILE=build/Debug/generators/conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug; cmake --build build
RUN ctest --test-dir build --rerun-failed --output-on-failure

ENTRYPOINT ["/app"]