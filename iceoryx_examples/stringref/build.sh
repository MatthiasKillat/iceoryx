WORKSPACE=$(git rev-parse --show-toplevel)
ICEORYX_INSTALL_PREFIX=$WORKSPACE/build/install/prefix/

mkdir -p build
cd build
cmake -DCMAKE_PREFIX_PATH=$ICEORYX_INSTALL_PREFIX $WORKSPACE/iceoryx_examples/stringref
cmake --build .