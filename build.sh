#bin/bash

if [ ! -d "/path/to/dir" ]; then
  mkdir "build"
fi

cmake --build build -j 20