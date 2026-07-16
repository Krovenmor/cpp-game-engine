#!/bin/bash

read -p "Путь назначения: " DEST_DIR
read -p "Имя проекта: " PROJ_NAME

DATA_DIR="scripts/default_game_data"
OLD_H="default.h"
OLD_CPP="default.cpp"
OLD_CPP_MAIN="main.cpp"
OLD_CMAKE="CMakeLists.txt"

NEW_H="${PROJ_NAME}.h"
NEW_CPP="${PROJ_NAME}.cpp"

if [ ! -d "$DATA_DIR" ]; then
    echo "Ошибка: Директория $DATA_DIR не найдена!"
    exit 1
fi

replace_text() {
    sed -i '' "s/$1/$2/g" "$3" 2>/dev/null || sed -i "s/$1/$2/g" "$3"
}

cp "$DATA_DIR/$OLD_H" "$DATA_DIR/$NEW_H"
cp "$DATA_DIR/$OLD_CPP" "$DATA_DIR/$NEW_CPP"

if [ -f "$DATA_DIR/$OLD_CPP_MAIN" ]; then
    cp "$DATA_DIR/$OLD_CPP_MAIN" "$DATA_DIR/temp_main.cpp"
    replace_text "$OLD_H" "$NEW_H" "$DATA_DIR/temp_main.cpp"
fi

if [ -f "$DATA_DIR/$OLD_CMAKE" ]; then
    cp "$DATA_DIR/$OLD_CMAKE" "$DATA_DIR/temp_cmake.txt"
    replace_text "Default" "$PROJ_NAME" "$DATA_DIR/temp_cmake.txt"
fi

replace_text "$OLD_H" "$NEW_H" "$DATA_DIR/$NEW_H"
replace_text "$OLD_H" "$NEW_H" "$DATA_DIR/$NEW_CPP"

mkdir -p "$DEST_DIR/src"
mkdir -p "$DEST_DIR/build"

cp -R "$DATA_DIR/"* "$DEST_DIR/"

rm -f "$DEST_DIR/$OLD_H" "$DEST_DIR/$OLD_CPP" "$DEST_DIR/$OLD_CMAKE" "$DEST_DIR/$OLD_CPP_MAIN"


mv "$DEST_DIR"/*.h "$DEST_DIR/src/" 2>/dev/null
mv "$DEST_DIR"/*.cpp "$DEST_DIR/src/" 2>/dev/null

if [ -f "$DEST_DIR/temp_cmake.txt" ]; then
    mv "$DEST_DIR/temp_cmake.txt" "$DEST_DIR/$OLD_CMAKE"
fi

if [ -f "$DEST_DIR/src/temp_main.cpp" ]; then
    mv "$DEST_DIR/src/temp_main.cpp" "$DEST_DIR/src/$OLD_CPP_MAIN"
fi

rm -f "$DATA_DIR/$NEW_H" "$DATA_DIR/$NEW_CPP" "$DATA_DIR/temp_cmake.txt" "$DATA_DIR/temp_main.cpp"

echo "------------------------------------------"
echo "Проект '$PROJ_NAME' создан успешно!"
echo "Структура:"
echo "$DEST_DIR/"
echo "├── vcpkg.json"
echo "├── CMakeLists.txt"
echo "└── src/"
echo "    ├── $NEW_H"
echo "    ├── $NEW_CPP"
echo "    └── $OLD_CPP_MAIN"
echo "└── materials/"
echo "    └── Fonts/"
echo "        └── test.ttf"
echo "    └── Textures/"
echo "        └── test.png"
echo "├── build/"
echo "└── assets/"
echo "    └── config/"
echo "        └── tilesets_config.json"
echo "    └── logs/"
echo "    └── tilesets/"
echo "        └── tilemap_test.json"
echo "------------------------------------------"