/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 20:59:57 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 18:55:34 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// (c1 && (c2 || (c3 && c4)) && ((c5 || c6) && (c7 || (c8 && c9)))) || (((c10 && (c11 || c12)) && (c13 || (c14 && c15))) && ((c16 || (c17 && c18)) && (c19 || c20))) && ((((c21 || (c22 && c23)) && (c24 || (c25 && c26))) && ((c27 || c28) && (c29 || c30))) || ((c31 && (c32 || (c33 && c34))) && ((c35 || (c36 && c37)) && (c38 || c39)))) || (((c40 && (c41 || c42)) && ((c43 || c44) && (c45 && c46))) || ((((c47 && c48) || (c49 && c50)))))
// (c1 && (c2 || (c3 && c4)) && ((c5 || c6) && (c7 || (c8 && c9)))) || (((c10 && (c11 || c12)) && (c13 || (c14 && c15))) && ((c16 || (c17 && c18)) && (c19 || c20))) && ((((c21 || (c22 && c23)) && (c24 || (c25 && c26))) && ((c27 || c28) && (c29 || c30))) || ((c31 && (c32 || (c33 && c34))) && ((c35 || (c36 && c37)) && (c38 || c39)))) || (((c40 && (c41 || c42)) && ((c43 || c44) && (c45 && c46))) || ((c47 && c48) || (c49 && c50))) 

#ifndef MINISHELL_H
# define MINISHELL_H

# define COLOR_RESET   "\x1b[0m"
# define COLOR_BLUE    "\x1b[34m"
# define COLOR_GREEN   "\x1b[32m"
# define COLOR_YELLOW  "\x1b[33m"
# define COLOR_CYAN    "\x1b[36m"

# define UNMATCHING_PARENTH -1
# define STARTING_WITH_CLOSE_PARENTH -2
# define PARENTH_AFTER_WRONG_TOKEN -3

# include "libft/libft.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include <sys/stat.h>
# include <dirent.h>
# include <string.h>
# include <sys/ioctl.h>
# include <termios.h>
# include <curses.h>
# include <term.h>

extern sig_atomic_t	g_sigint_received;

typedef struct s_env
{
	char			*name;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_history
{
	char				*command;
	struct s_history	*next;
	struct s_history	*prev;
}	t_history;

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_HEREDOC,
	T_APPEND,
	T_ENV_VAR,
	T_AND,
	T_OR,
	T_WILDCARD,
	T_PARENTH_OPEN,
	T_PARENTH_CLOSE,
}	t_type;

typedef enum e_type_word
{
	T_NO_QUOTE,
	T_S_QUOTE,
	T_D_QUOTE,
}	t_type_word;

typedef struct s_token_word
{
	char				*content;
	t_type_word			type;
	struct s_token_word	*next;
}	t_token_word;

typedef struct s_cmd
{
	char			*name;
	char			*path;
	char			**arg;
	t_token_word	**list_word;
	int				ac;
	int				type_redirection;
	char			*file_redirection;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_token
{
	char			*value;
	t_type			type;
	t_token_word	*type_word;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

typedef enum e_ast_type
{
	AST_PIPE,
	AST_CMD,
	AST_REDIR,
	AST_AND,
	AST_OR,
	AST_SUB_SHELL,
}	t_ast_type;

typedef struct s_ast
{
	t_ast_type		type;
	t_cmd			*cmd;
	t_token			*redir;
	struct s_ast	*left;
	struct s_ast	*right;
	struct s_ast	*sub_shell;
}	t_ast;

typedef struct s_shell
{
	t_env		*env;
	t_cmd		*cmd;
	t_history	*history;
	int			exit_status;
}	t_shell;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                               FUNCTIONS                                   */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                                SRC/MAIN                                   */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// main.c
int				main(int ac, char **av, char **envp);

// init_shell.c
void			free_env_list(t_env *head);
t_env			*create_env_node(const char *env_str);
t_env			*init_env(char **envp);
t_shell			*init_shell(char **envp);

// signals.c
void			handle_sigint(int sig);
int				setup_signals(void);

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                                SRC/PARSING                                */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// cmd_utils.c
int				is_command_argument(t_type type);
int				count_args(t_token *tokens);
int				init_cmd(t_cmd *cmd, t_token *tokens);
int				add_command_arg(t_cmd *cmd, t_token *current, int i);
void			track_word_tokens(t_cmd *cmd, t_token *tokens);
t_cmd			*create_command(t_token *tokens);

// parenthesis_utils.c
int				good_position_parenth(t_token *tmp);
int				verify_parenthesis(t_token *tokens);
t_token			*find_matching_closing_par(t_token *opening);
t_token			*find_matching_opening_par(t_token *closing);
t_token			*find_opening_par(t_token *tokens);
t_token			*extract_parenthesis_content(t_token *start);

// parser_utils.c
t_ast			*init_ast_node(t_ast_type type);
t_token			*init_new_token(t_token *tmp, t_token *prev);
t_token			*find_last_token_of_type(t_token *tokens, t_type type);
t_token			*get_tokens_after(t_token *current);
t_token			*get_tokens_before(t_token *tokens, t_token *current);

// parser.c
t_ast			*add_ast_node(t_token *tokens, t_token *current, t_ast_type type);
t_ast			*parse_command(t_token *tokens);
t_ast			*parse_pipes(t_token *tokens);
t_ast			*parse_logical_ops(t_token *tokens);
int				check_syntax_error_parenthesis(t_token *tokens);
int				check_if_operator_is_last(t_token *first_op, t_token *second_op);
void			point_last_operator_outside_parenthesis(t_token **or, t_token **and,
											t_token **pipe, t_token *current);
t_token			*find_last_priority_operator(t_token *tokens);
t_ast_type		get_ast_type(t_token *token);
t_ast			*parse_parenthesis(t_token *tokens);
t_ast			*parse_tokens(t_token *tokens);

// redir_utils.c
int				is_redirection(t_type type);
int				add_redirection(t_ast *node, t_token *redir_token);
int				add_redirection_to_node(t_ast *node, t_token *current);
t_token			*skip_redirection(t_token *current);

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                                SRC/TOKENIZER                              */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// env_token.c
int				handle_question_mark(t_token **list, int *i);
int				handle_dollar(t_token **list);
int				add_env_var_token(t_token **list, int *i, char *input);

// operator_token.c
int				add_operator_token(t_token **list, int *i, char *input);
int				handle_wild_and_parenthesis(t_token *new, int *i, char *input);
int				handle_single_operator(t_token *new, int *i, char *input);
int				handle_double_redir(t_token *new, int *i, char *input);
int				handle_double_operator(t_token *new, int *i, char *input);

// tokenizer.c
t_token			*tokenize(char *input);
int				process_token(t_token **tokens, int *i, char *input);
int				add_node_back(t_token **list, t_token *new);
int				is_operator(char input);

// word_quote_content.c
t_token_word	*define_word_quote_type(t_token_word **list, char *content,
									int *i, int *start);
t_token_word	*define_word_no_quote(t_token_word **list, char *content,
									int	*i, int	*start);
void			analyze_quote_content(t_token_word **list, t_token *new,
								char *content);
int				add_word_token(t_token **list, int *i, char *input);
char			*create_content_word(char *input, int *i);
int				add_word_token(t_token **list, int *i, char *input);

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                                 SRC/EXEC                                  */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// executor.c
char			**env_to_array(t_env *env);
char   			*extract_path(char *path, char *command_name);
char			*find_command_path(char *name, t_shell *shell);
int				execute_command(t_cmd *cmd, t_shell *shell);
int				execute_ast(t_ast *ast, t_shell *shell);

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*                                                                           */
/*                                 SRC/FREE                                  */
/*                                                                           */
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

// free_parsing.c
void			free_tokens(t_token *tokens);
void			free_ast(t_ast *node);
void			free_cmd(t_cmd *cmd);
void			free_array(char **array);

#endif
