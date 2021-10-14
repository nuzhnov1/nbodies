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
.PHONY: default run, clean

# Default goal
default: $(EXE)

# Run program
run: $(EXE)
	$<

# Remove directory ./obj and ./bin
clean:
	$(RMDIR) $(OBJ) $(BIN)

# Create directories ./bin and ./obj
$(BIN) $(OBJ):
	$(MKDIR) $@

# Compile all source files
$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

# Link all object files
$(EXE): $(OBJS) | $(BIN)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
