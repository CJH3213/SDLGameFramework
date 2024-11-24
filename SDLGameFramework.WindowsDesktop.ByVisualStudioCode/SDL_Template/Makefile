# 设置编译器
CXX = g++
CXXFLAGS = -std=c++20 -fcoroutines -frtti -g -fdiagnostics-color=always

# 目录结构
# SDL_Template/
# ├── Build/        .o
# ├── Inc/          .h
# ├── Output/       .exe
# ├── Src/          .c
# ├── SDL2/         三方库SDL2
# ├── FFmpeg/       三方库FFmpeg
# ├── Makefile      makefile

# 从.cpp文件开始编译时，.cpp文件在Src文件夹下，因此要获取上级目录
PARENT_DIR = $(shell dirname $(CURDIR))

# 设置包含目录（头文件路径）
INCLUDES = \
    -I$(PARENT_DIR)/Inc \
    -I$(PARENT_DIR)/SDL2/SDL2-2.28.5/include \
    -I$(PARENT_DIR)/SDL2/SDL2_ttf-2.0.15/include \
    -I$(PARENT_DIR)/SDL2/SDL2_image-2.0.5/include \
    -I$(PARENT_DIR)/FFmpeg/x64/include

# 设置库目录
LIBDIRS = \
    -L$(PARENT_DIR)/SDL2/SDL2-2.28.5/lib/x64 \
    -L$(PARENT_DIR)/SDL2/SDL2_ttf-2.0.15/lib/x64 \
    -L$(PARENT_DIR)/SDL2/SDL2_image-2.0.5/lib/x64 \
    -L$(PARENT_DIR)/FFmpeg/x64/lib

# 设置库文件
LIBS = \
    -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf \
    -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale

# 获取Src目录下的所有 .cpp 文件
SRCS = $(wildcard $(PARENT_DIR)/Src/*.cpp)

# 对于每个 .cpp 文件都生成同名的 .o 文件
OBJS = $(SRCS:$(PARENT_DIR)/Src/%.cpp=$(PARENT_DIR)/Build/%.o)

# 指定输出的可执行文件名，exe名和项目文件夹同名
TARGET = $(PARENT_DIR)/Output/$(notdir $(PARENT_DIR)).exe

# 默认目标
#all: $(TARGET)

# 编译目标文件：-c只编译不链接、$<第1个依赖文件cpp、-o指定输出文件、$@目标文件o
$(PARENT_DIR)/Build/%.o: $(PARENT_DIR)/Src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIBDIRS) $(LIBS) -o $(TARGET)

# 清理目标
clean:
	rm -f $(OBJS) $(TARGET)
