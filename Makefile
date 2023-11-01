NAME		= webserv

CXX			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++98
INCLUDES	= -Iinc
HEADERS		= inc/ParseConfigFile.hpp \
        	  inc/Server.hpp \
			  inc/AcceptorSockets.hpp
SRCDIR		= srcs
OBJDIR		= objs
SOURCES		= $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS		= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

RED = \033[41m
GREEN = \033[42m
NC = \033[0m

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@mkdir -p $(@D)
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJECTS)
	@$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $@
	@echo "$(GREEN)Build Successful ............[OK]$(NC)"

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@rm -rf $(OBJDIR)
	@echo "$(RED)Cleaning Objects...[OK]$(NC)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)Cleaning All.......[OK]$(NC)"

re: fclean all

.PHONY: all clean fclean re