#!/bin/zsh

# 1. Setup paths
GLFW_PATH=$(brew --prefix glfw)
INCLUDE_DIR="./include"
SRC_DIR="./src"
OUTPUT="grav_sim"

MACOS_VER="26.0";

echo "🚀 Building for macOS Tahoe..."

# 2. Compile glad.c (C only)
# We compile to an object file (.o) so it doesn't care about C++17 flags
clang -c "$SRC_DIR/glad.c" -I"$INCLUDE_DIR" -o glad.o -mmacosx-version-min=$MACOS_VER

# 3. Compile main.cpp and link everything
# We pass glad.o into the final link step
clang++ -std=c++17 \
    "$SRC_DIR/main.cpp" glad.o \
    -I"$INCLUDE_DIR" \
    -I"$GLFW_PATH/include" \
    -L"$GLFW_PATH/lib" \
    -lglfw \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -mmacosx-version-min=$MACOS_VER \
    -o "$OUTPUT"

# 4. Cleanup and Run
if [ $? -eq 0 ]; then
    rm glad.o
    echo "✅ Build Successful! Launching..."
    echo "------------------------------------"
    ./"$OUTPUT"
else
    rm -f glad.o
    echo "❌ Build Failed."
fi

