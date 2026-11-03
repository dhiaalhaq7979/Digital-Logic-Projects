#!/usr/bin/env bash
set -e
mkdir -p build
for src in */*.cpp; do
  name=$(basename "$src" .cpp)
  g++ -std=c++17 -Wall -Wextra -O2 "$src" -o "build/$name"
  echo "built build/$name"
done
