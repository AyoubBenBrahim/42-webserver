CXX					= c++
CXXFLAGS			= -Wall -Wextra -Werror -std=c++98
NAME				= Webserv

# make it clear later :)
SRCS				= $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
INCS				= $(wildcard $(INC_DIR)/**/*.hpp) $(wildcard $(INC_DIR)/*.hpp)
OBJS				= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

SRC_DIR				= srcs
INC_DIR				= inc
OBJ_DIR				= objects
INCLUDES			= -I$(INC_DIR) -I$(INC_DIR)/parsing -I$(INC_DIR)/server -I$(INC_DIR)/utils -I$(INC_DIR)/client 
# TRASH				= ./conf/.Trash

#====== Colors =======#
RESET				= "\033[0m"
RED					= "\033[1;31m"
GREEN				= "\033[1;32m"
YELLOW				= "\033[1;33m"
ORANGE				= "\033[1;34m"
MAGENTA				= "\033[1;35m"
CYAN				= "\033[1;36m"
WHITE				= "\033[1;37m"

all:				$(NAME) #| $(TRASH)

$(TRASH):
	@mkdir -p $(TRASH)

$(OBJ_DIR)/%.o:		$(SRC_DIR)/%.cpp $(INCS)
					@echo $(YELLOW)Compiling $<...$(RESET)
					@mkdir -p $(dir $@)
					@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME):			$(OBJS)
					@echo $(ORANGE)Linking $(OBJECTS) to $(NAME)...$(RESET)
					@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@
					@echo $(CYAN)$(NAME) is ready!$(RESET)



clean:
					@echo $(MAGENTA)Cleaning object files...$(RESET)
					@sleep 0.2
					@rm -rf $(OBJ_DIR)
					@rm -f upload/*
					@echo $(GREEN)---DONE!---$(RESET)
# @rm -rf $(TRASH)

fclean:				clean
					@echo $(MAGENTA)Cleaning $(NAME) executable...$(RESET)
					@sleep 0.2
					@rm -f $(NAME)
					@echo $(GREEN)---DONE!---$(RESET)
					@rm -rf .vscode
					@rm -rf .DS_Store

re:					fclean all

siege:
	brew install siege

.PHONY:				all clean fclean re
