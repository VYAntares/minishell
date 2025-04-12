/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:43 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/12 11:27:14 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>       /* Déplacé en premier pour définir FILE */
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <limits.h>
# include <stdbool.h>

# include "libft.h"

extern sig_atomic_t	g_sigint_received;

/* Constantes et erreurs */
# define STARTING_WITH_CLOSE_PARENTH -1
# define UNMATCHING_PARENTH -2
# define PARENTH_AFTER_WRONG_TOKEN -3

// extern const char *TREE_VERTICAL;
// extern const char *TREE_BRANCH;
// extern const char *TREE_CORNER;
// extern const char *TREE_HORIZONTAL;

/* Types de tokens */
typedef enum e_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_HEREDOC,
	T_APPEND,
	T_ENV_VAR,
	T_WILDCARD,
	T_AND,
	T_OR,
	T_PARENTH_OPEN,
	T_PARENTH_CLOSE
}	t_type;

/* Types de mots (quotés ou non) */
typedef enum e_type_word
{
	T_NO_QUOTE,
	T_S_QUOTE,
	T_D_QUOTE
}	t_type_word;

/* Structure pour un token de mot */
typedef struct s_token_word
{
	char				*content;
	t_type_word			type;
	struct s_token_word	*next;
}	t_token_word;

/* Structure pour un token */
typedef struct s_token
{
	char			*value;
	t_token_word	*type_word;
	t_type			type;
	struct s_token	*next;
	struct s_token	*prev;
}	t_token;

/* Structure pour une redirection de fichier */
typedef struct s_file_redir
{
	int					type_redirection;
	char				*content;
	t_token_word		*word_parts;
	struct s_file_redir	*next;
}	t_file_redir;

/* Structure pour une commande */
typedef struct s_cmd
{
	char				*name;
	char				*path;
	char				**arg;
	t_token_word		**list_word;
	t_file_redir		*type_redir;
	int					ac;
	struct s_cmd		*next;
}	t_cmd;

/* Types de nœuds AST */
typedef enum e_ast_type
{
	AST_CMD,
	AST_PIPE,
	AST_AND,
	AST_OR,
	AST_REDIR,
	AST_SUB_SHELL
}	t_ast_type;

/* Structure AST */
typedef struct s_ast
{
	t_ast_type			type;
	t_cmd				*cmd;
	t_token				*redir;
	struct s_ast		*left;
	struct s_ast		*right;
	struct s_ast		*sub_shell;
}	t_ast;

/* Structure pour les variables d'environnement */
typedef struct s_env
{
	char				*name;
	char				*value;
	struct s_env		*next;
}	t_env;

/* Structure du shell */
typedef struct s_shell
{
	t_cmd				*cmd;
	t_env				*env;
	char				**history;
	int					pid;
	int					exit_status;
}	t_shell;

/* Fonctions de tokenization */
t_token		*tokenize(char *input);
int			add_node_back(t_token **list, t_token *new);
int			add_word_token(t_token **list, int *i, char *input);
int			add_operator_token(t_token **list, int *i, char *input);
int			add_env_var_token(t_token **list, int *i, char *input);
int			is_operator(char input);

/* Fonctions de parsing */
t_ast		*parse_tokens(t_token *tokens);
t_ast		*init_ast_node(t_ast_type type);
t_token		*init_new_token(t_token *tmp, t_token *prev);
t_token		*find_last_token_of_type(t_token *tokens, t_type type);
t_token		*get_tokens_after(t_token *current);
t_token		*get_tokens_before(t_token *tokens, t_token *current);
int			check_syntax_error_parenthesis(t_token *tokens);

/* Fonctions pour les commandes */
t_cmd		*create_command(t_token *tokens);
int			is_command_argument(t_type type);
int			count_args(t_token *tokens);
int			add_command_arg(t_cmd *cmd, t_token *current, int i);

/* Fonctions pour les redirections */
int			is_redirection(t_type type);
t_token		*skip_redirection(t_token *current);
int			add_redirection_to_cmd(t_cmd *cmd, t_token *current);

/* Fonctions pour les parenthèses */
int			verify_parenthesis(t_token *tokens);
t_token		*find_matching_closing_par(t_token *opening);
t_token		*find_matching_opening_par(t_token *closing);
t_token		*find_opening_par(t_token *tokens);
t_token		*extract_parenthesis_content(t_token *start);
int			good_position_parenth(t_token *tmp);

/* Fonctions d'initialisation */
t_shell		*init_shell(char **envp);
t_env		*init_env(char **envp);
t_env		*create_env_node(const char *env_str);
int			setup_signals(void);
void		handle_sigint(int sig);

/* Fonctions d'exécution */
int			execute_ast(t_ast *ast, t_shell *shell);
int			execute_command(t_cmd *cmd, t_shell *shell);
int			execute_pipe(t_ast *left, t_ast *right, t_shell *shell);
int			execute_redirections(t_cmd *cmd, t_shell *shell);
int			handle_heredoc(t_file_redir *redir, t_shell *shell);
char		*extract_path(char *path, char *command_name);
char		*find_command_path(char *name, t_shell *shell);
char		**env_to_array(t_env *env);

/* Fonctions pour les variables d'environnement */
char		*get_env_value(t_env *env, const char *name);
int			expand_env_vars(t_token *tokens, t_shell *shell);
int			expand_env_var(t_token_word *token_word, t_shell *shell);
int			expand_var_in_string(t_token_word *token_word, t_shell *shell);
int			rebuild_token_value(t_token *token);

/* Fonctions builtin */
int			is_builtin(char *cmd);
int			execute_builtin(t_cmd *cmd, t_shell *shell);
int			builtin_echo(t_cmd *cmd);
int			builtin_cd(t_cmd *cmd, t_shell *shell);
int			builtin_pwd(void);
int			builtin_export(t_cmd *cmd, t_shell *shell);
int			builtin_unset(t_cmd *cmd, t_shell *shell);
int			builtin_env(t_shell *shell);
int			builtin_exit(t_cmd *cmd, t_shell *shell);

/* Fonctions auxiliaires pour les builtins */
int			update_env_variable(t_shell *shell, const char *name,
				const char *value);
int			add_env_variable(t_shell *shell, const char *name,
				const char *value);
int			is_valid_identifier(const char *id);
int			print_sorted_env(t_shell *shell);
int			is_number(char *str);

/* Fonctions de libération de mémoire */
void		free_tokens(t_token *tokens);
void		free_ast(t_ast *node);
void		free_cmd(t_cmd *cmd);
void		free_env_list(t_env *head);
void		free_array(char **array);

int			expand_var(t_cmd *cmd, t_shell *shell);
char		*expand_env_heredoc(char *line, t_shell *shell);
int			expand_redir(t_cmd *cmd, t_shell *shell);

void		cleanup_heredoc_files(t_cmd *cmd);
void		launch_heredoc(t_ast *ast, t_shell *shell);

void		debug_print_ast(t_ast *node, const char *prefix, bool is_last);
void		debug_print_tokens(t_token *tokens);
int			add_node_back_word(t_token_word **list, t_token_word *new);
t_token		*find_last_priority_operator(t_token *tokens);
t_ast_type	get_ast_type(t_token *token);

#endif