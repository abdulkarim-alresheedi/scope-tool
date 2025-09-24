CC = gcc
CFLAGS = -Wall -Wextra -fPIC -Iinclude
LDFLAGS = -ldl

CORE_DIR = core
PLUGINS_DIR = plugins
INCLUDE_DIR = include
BUILD_DIR = build

CORE_SRCS = $(CORE_DIR)/main.c $(CORE_DIR)/tracer.c $(CORE_DIR)/loader.c
CORE_OBJS = $(CORE_SRCS:$(CORE_DIR)/%.c=$(BUILD_DIR)/%.o)

PLUGINS_SRCS = $(PLUGINS_DIR)/ext_file.c \
               $(PLUGINS_DIR)/ext_net.c \
               $(PLUGINS_DIR)/ext_exec.c \
               $(PLUGINS_DIR)/ext_mem.c \
               $(PLUGINS_DIR)/ext_debug.c

PLUGINS_OBJS = $(PLUGINS_SRCS:$(PLUGINS_DIR)/%.c=$(BUILD_DIR)/%.so)

TARGET = $(BUILD_DIR)/scope
UTILS_LIB = $(BUILD_DIR)/libscopeutils.so

# Colors
RESET  = \033[0m
RED    = \033[1;31m
GRN    = \033[1;32m
YEL    = \033[1;33m
BLU    = \033[1;34m

all: $(TARGET) plugins
	@echo "$(GRN)[OK] Build finished$(RESET)"

# Build core program
$(TARGET): $(CORE_OBJS) $(UTILS_LIB)
	@echo "$(BLU)[INFO] Linking core -> $(TARGET)$(RESET)"
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(CORE_DIR)/%.c | $(BUILD_DIR)
	@echo "$(BLU)[INFO] Compiling core -> $<$(RESET)"
	$(CC) $(CFLAGS) -c $< -o $@

# Build shared utils library
$(UTILS_LIB): $(CORE_DIR)/utils.c include/utils.h | $(BUILD_DIR)
	@echo "$(BLU)[INFO] Building shared library -> $(UTILS_LIB)$(RESET)"
	$(CC) $(CFLAGS) -shared -o $@ $(CORE_DIR)/utils.c

# Build plugins and copy to plugins/
plugins: $(PLUGINS_OBJS)
	@mkdir -p $(PLUGINS_DIR)
	@cp $(BUILD_DIR)/*.so $(PLUGINS_DIR)/
	@echo "$(GRN)[OK] Plugins copied to $(PLUGINS_DIR)$(RESET)"

$(BUILD_DIR)/%.so: $(PLUGINS_DIR)/%.c $(UTILS_LIB) | $(BUILD_DIR)
	@echo "$(BLU)[INFO] Building plugin -> $<$(RESET)"
	$(CC) $(CFLAGS) -shared -o $@ $< $(UTILS_LIB)

# Ensure build dir exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean
clean:
	@echo "$(YEL)[WARN] Cleaning build files...$(RESET)"
	rm -rf $(BUILD_DIR) $(PLUGINS_DIR)/*.so
	@echo "$(GRN)[OK] Clean done$(RESET)"
