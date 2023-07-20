
NAME 			= webserv

CC 				= c++
CFLAGS 			= -Wall -Wextra -Werror -std=c++98
INCLUDES 		= -Iinc
SRCDIR 			= srcs
OBJDIR 			= OBJS
SOURCES 		= $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS 		= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

RED				=	\033[41m
GREEN			=	\033[42m
NC				=	\033[0m


all				: $(NAME)

$(OBJDIR)/%.o	: $(SRCDIR)/%.cpp
				  	@mkdir -p $(@D)
				  	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME)			: $(OBJECTS)
					@$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@
					@echo "${GREEN}Build Successful ............[OK]${NC}"

$(OBJDIR)		:
					@mkdir -p $(OBJDIR)

clean			:
					@rm -rf $(OBJDIR)
					@echo "${RED}Cleaning Objects...[OK]${NC}" 

fclean			: clean
					@rm -f $(NAME)
					@echo "${RED}Cleaning All.......[OK]${NC}" 

re				: fclean all

.PHONY			: all clean fclean re