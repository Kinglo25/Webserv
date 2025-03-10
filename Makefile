NAME            =       webserv

SRC_DIR         =       src
INC_DIR         =       inc
OBJ_DIR         =       obj

SRCS            =       $(wildcard $(SRC_DIR)/*.cpp)
OBJS            =       $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

CC              =       c++
FLAGS           =       -Wall -Wextra -Werror -Wshadow -g3 -std=c++98 
INC             =       -I$(INC_DIR)
RM              =       rm -f

# Color codes
YELLOW          =       \033[0;33m
RED             =       \033[0;31m
GREEN           =       \033[0;32m
RESET           =       \033[0m

all:            $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "$(YELLOW)Compiling $<$(RESET)"
	@$(CC) $(INC) $(FLAGS) -c $< -o $@
	@echo "$(GREEN)Compiled $< successfully!$(RESET)"

$(NAME):        $(OBJS)
	@echo "$(YELLOW)Creating an executable$(RESET)"
	@$(CC) $(INC) $(FLAGS) $^ -o $@
	@echo "$(GREEN)Executable created successfully!$(RESET)"

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "$(RED)Cleaning objects...$(RESET)"
	@$(RM) -r $(OBJ_DIR)
	@echo "$(GREEN)Cleaned up!$(RESET)"

fclean:         clean
	@echo "$(RED)Removing executable...$(RESET)"
	@$(RM) $(NAME)
	@echo "$(GREEN)Removed!$(RESET)"

re:             fclean all

debug:          FLAGS += -g
debug:          re

.PHONY: all clean fclean re debug