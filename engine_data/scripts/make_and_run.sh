#!/bin/bash

read -p "Введите путь к проекту: " path
read -p "Введите имя исполняемого файла: " filename

cd "$path" || { echo "Ошибка: путь не найден"; exit 1; }

MAC_VCPKG="$HOME/.vcpkg/scripts/buildsystems/vcpkg.cmake"
LINUX_VCPKG="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake"

if [[ "$(uname)" == "Darwin" ]]; then
    VCPKG_PATH="$MAC_VCPKG"
    echo "--- Building on macOS ---"
else
    VCPKG_PATH="$LINUX_VCPKG"
    echo "--- Building on Linux ---"
fi

if [ ! -f "$VCPKG_PATH" ]; then
    echo "Ошибка: vcpkg не найден по пути $VCPKG_PATH"
    exit 1
fi

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH"

cmake --build build --parallel > build/build.txt 2> build/build_errors.txt

if [ $? -eq 0 ]; then
    echo "Сборка успешна! Запускаю..."
    ./build/$filename
else
    echo "Ошибка сборки! Проверь build/build_errors.txt"
fi
