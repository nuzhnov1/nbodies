# Variable definitions
MKDIR   = mkdir -p
RMDIR	= rm -rf
PROG	= nbodies
CC      = gcc
BIN     = ./bin
OBJ     = ./obj
INCLUDE = ./include
SRC     = ./src
SRCS    = $(wildcard $(SRC)/*.c)
OBJS    = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
EXE     = $(BIN)/$(PROG)
CFLAGS  = -I$(INCLUDE) -O3
LDFLAGS =
LDLIBS  = -fopenmp -lm

# Phony targets
.PHONY: build run rebuid clean

# Default goal
build: $(EXE)
	$(info Building program is complete. Executable file is located in $(BIN) \
		directory.)

# Run program
run: $(EXE)
	$(info Running "$(PROG)" program...)
	$<

# Rebuild program
rebuild: clean build

# Remove directory ./obj and ./bin
clean:
	$(info Removing directories "$(OBJ)" and "$(BIN)"...)
	$(RMDIR) $(OBJ) $(BIN)

# Create directories ./bin and ./obj
$(BIN) $(OBJ):
	$(info Creating directories "$(OBJ)" and "$(BIN)"...)
	$(MKDIR) $@

# Compile all source files
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(info Compiling "$<" file...)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

# Link all object files and create executable file
$(EXE): $(OBJS) | $(BIN)
	for item in $^ ; do \
		echo "Linking $$item file..." ; \
	done
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
