NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g

# Détection du système d'exploitation
UNAME_S := $(shell uname -s)

# Configuration spécifique pour MacOS
ifeq ($(UNAME_S),Darwin)
    READLINE_DIR = $(shell brew --prefix readline)
    INC += -I$(READLINE_DIR)/include
    READLINE = -L$(READLINE_DIR)/lib -lreadline
else
    READLINE = -lreadline
endif

# Header
INC += -I ./ -I ./libft/

# Libft
LIBFT_DIR = libft/
LIBFT = $(LIBFT_DIR)libft.a
LIBFT_INC = -I ./libft
LIBFT_LIB = -L./libft -lft

# Source files directories
SRCS_DIR = 			src/
MAIN_DIR = 			$(SRCS_DIR)main/
PARSING_DIR =		$(SRCS_DIR)parsing/
TOKENIZER_DIR = 	$(SRCS_DIR)tokenizer/
BUILTINS_DIR = 		$(SRCS_DIR)builtins/
EXEC_DIR = 			$(SRCS_DIR)exec/
EXPAND_DIR = 		$(SRCS_DIR)expand_env/

# Source files
MAIN_SRCS = 		main.c \
					init_shell.c \
					signals.c \
					debugger.c

PARSING_SRCS = 		parser.c \
					parser_utils.c \
					redir_utils.c \
					cmd_utils.c \
					parenthesis_utils.c \
					parenthesis_utils2.c

TOKENIZER_SRCS = 	tokenizer.c \
					operator_token.c \
					word_quote_token.c

BUILTINS_SRCS = 	export_utils.c \
					cd.c \
					echo.c \
					env.c \
					exit.c \
					export.c \
					pwd.c \
					unset.c

EXEC_SRCS = 		executor.c \
					heredoc.c \
					pipe_redir.c \
					executor_utils.c \
					wildcards.c

EXPAND_SRCS =		expand_env.c \
					env_heredoc.c \
					expand_env_utils.c \
					expand_redir.c \
					get_value.c \
					process_dollar_quote.c

# Concatenate all sources
SRCS = $(addprefix $(MAIN_DIR), $(MAIN_SRCS)) \
       $(addprefix $(PARSING_DIR), $(PARSING_SRCS)) \
       $(addprefix $(TOKENIZER_DIR), $(TOKENIZER_SRCS)) \
       $(addprefix $(BUILTINS_DIR), $(BUILTINS_SRCS)) \
       $(addprefix $(EXEC_DIR), $(EXEC_SRCS)) \
	   $(addprefix $(EXPAND_DIR), $(EXPAND_SRCS)) \

# Object files
OBJS_DIR = obj/
OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))

# Dependencies
DEPS = $(OBJS:.o=.d)

# Colors for pretty printing
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

all: $(LIBFT) $(NAME)

# Compile l'exécutable
$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $@$(RESET)"
	@$(CC) $(OBJS) $(LIBFT_LIB) $(READLINE) -o $@

# Compile les objets
$(OBJS_DIR)src/%.o: src/%.c minishell.h
	@echo "$(GREEN)Compiling $<$(RESET)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) $(LIBFT_INC) -MMD -c $< -o $@

# Compile la libft
$(LIBFT):
	@echo "$(GREEN)Compiling libft...$(RESET)"
	@make -C $(LIBFT_DIR)

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJS_DIR)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(RED)Cleaning executable...$(RESET)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
