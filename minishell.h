/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:43 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/18 23:14:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
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
# include <dirent.h>
# include "libft.h"

extern sig_atomic_t	g_sigint_received;

/* =================================================================== */
/*                          CONSTANTES ET ERREURS                      */
/* =================================================================== */

# define STARTING_WITH_CLOSE_PARENTH -1
# define UNMATCHING_PARENTH -2
# define PARENTH_AFTER_WRONG_TOKEN -3

/* =================================================================== */
/*                               TYPES                                 */
/* =================================================================== */

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
	int					is_ambiguous;
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

/* =================================================================== */
/*                      FONCTIONS PAR CATÉGORIE                        */
/* =================================================================== */

/*
** =================================================================
** BUILTINS
** =================================================================
*/

/* builtins/cd.c */
int				update_repertory(char *path, t_shell *shell);
int				builtin_cd(t_cmd *cmd, t_shell *shell);

/* builtins/echo.c */
int				builtin_echo(t_cmd *cmd);

/* builtins/env.c */
int				builtin_env(t_shell *shell);

/* builtins/exit.c */
int				is_number(char *str);
int				print_error_exit(t_cmd *cmd);
int				builtin_exit(t_cmd *cmd, t_shell *shell);

/* builtins/export.c */
void			create_and_fill_env(char **env_array, t_shell *shell);
void			display_array(char **env_array);
void			bubble_sort(char **env_array);
int				print_sorted_env(t_shell *shell);
int				builtin_export(t_cmd *cmd, t_shell *shell);

/* builtins/export_utils.c */
int				handle_export_declaration(char *arg, int *status);
int				is_valid_identifier(const char *id);
int				add_env_variable(t_shell *shell,
					const char *name,
					const char *value);
int				update_env_variable(t_shell *shell,
					const char *name,
					const char *value);
int				handle_export_with_value(char *arg,
					t_shell *shell,
					int *status);

/* builtins/pwd.c */
int				builtin_pwd(void);

/* builtins/unset.c */
void			delete_line_env(t_shell *shell, t_cmd *cmd, int i);
int				builtin_unset(t_cmd *cmd, t_shell *shell);

/*
** =================================================================
** EXEC
** =================================================================
*/

/* exec/execute_command.c */
int				is_builtin(char *cmd);
int				execute_builtin(t_cmd *cmd, t_shell *shell);
int				handle_builtin(t_cmd *cmd, t_shell *shell);
void			handle_command(t_cmd *cmd, t_shell *shell);
int				handle_parent_process(pid_t pid, int is_minishell);

/* exec/executor.c */
int				execute_command(t_cmd *cmd, t_shell *shell);
int				execute_subshell(t_ast *sub_shell, t_shell *shell);
int				execute_ast(t_ast *ast, t_shell *shell);

/* exec/executor_utils.c */
char			*assemble_array(char *name, char *value);
char			**env_to_array(t_env *env);
char			*test_path_for_command(char *directory, char *command_name);
char			*extract_path(char *path, char *command_name);
char			*find_command_path(char *name, t_shell *shell);

/* exec/heredoc.c */
char			*create_temp_file(void);
int				process_heredoc_lines(int fd,
					char *delimiter,
					t_shell *shell,
					char *temp_file);
int				handle_heredoc(t_file_redir *redir,
					t_shell *shell,
					char *delimiter);
char			*purify_quote(t_file_redir	*redir);
void			launch_heredoc(t_ast *ast, t_shell *shell);

/* exec/heredoc_sigint.c*/
void			handle_heredoc_sigint(int sig);
int				setup_heredoc_signals(void);
int 			process_sig(int *sig_received, char *line, char *temp_file);

/* exec/heredoc_utils.c */
void			chain_commands(t_ast *ast, t_cmd **first_cmd, t_cmd **last_cmd);
t_cmd			*get_chained_commands(t_ast *ast);

/* exec/pipe.c */
void			execute_left_command_in_pipe(int pipefd[2],
					t_ast *left,
					t_shell *shell);
int				handle_right_process_fork_error(int pipefd[2], int pid1);
void			execute_right_command_in_pipe(int pipefd[2],
					t_ast *right,
					t_shell *shell);
int				execute_pipe(t_ast *left, t_ast *right, t_shell *shell);

/* exec/redirection_utils.c */
int				find_lasts_redirection(t_file_redir *redir,
					t_file_redir **last_input,
					t_file_redir **last_output);
int				ambiguous_error(t_file_redir *redir);

/* exec/redirections.c */
int				only_redirection(t_cmd *cmd);
int				process_outpout_redirection(t_file_redir *redir,
					t_file_redir **last_output);
int				process_input_redirection(t_file_redir *redir,
					t_file_redir **last_input);
int				process_all_redirections(t_file_redir *redir,
					t_file_redir **last_input,
					t_file_redir **last_output);
int				execute_redirections(t_cmd *cmd, t_shell *shell);

/*
** =================================================================
** EXPAND_ENV
** =================================================================
*/

/* expand_env/env_heredoc.c */
char			*expand_env_heredoc(char *line, t_shell *shell);

/* expand_env/expand_env.c */
int				expand_env_var(t_token_word *token_word, t_shell *shell);
int				rebuild_command_arg(t_cmd *cmd);
int				expand_var(t_cmd *cmd, t_shell *shell);

/* expand_env/expand_env_utils.c */
int				should_we_split(t_token_word *list_word);
int				count_arg_after_split(t_token_word *list_word);
char			*rebuild_word(t_token_word *list_word);
void			fill_new_args(t_token_word *list_word, int *k, char **new_args);
void			replace_args_and_free(char **new_args,
					t_cmd *cmd,
					int k,
					int new_ac);

/* expand_env/expand_redir.c */
int				rebuild_redirection(t_cmd *cmd);
int				expand_redir_name(t_token_word *current,
					t_shell *shell,
					t_file_redir *redir);
int				launch_redir_expansion(t_shell *shell, t_file_redir *redir);
int				expand_redir(t_cmd *cmd, t_shell *shell);

/* expand_env/get_value.c */
char			*get_env_value(t_env *env, const char *name);
char			*get_int_value_of(int env, int *i);
char			*extract_env_value(char *line,
					t_shell *shell,
					int *i,
					int *start);
char			*expand_line(char *line, char *env_value, int start, int i);
char			*launch_expansion(char *line,
					t_shell *shell,
					int *i,
					int *start);

/* expand_env/process_dollar_quote.c */
int				process_dollar(t_token_word *current);
int				prepro_dol_redir(t_token_word *word_parts);
int				preprocess_dollar_quotes(t_token_word **head);

/*
** =================================================================
** MAIN
** =================================================================
*/

/* main/debugger.c */
void			debug_print_ast(t_ast *node, const char *prefix, bool is_last);
void			debug_print_tokens(t_token *tokens);

/* main/init_shell.c */
t_env			*create_env_node(const char *env_str);
void			*create_minimal_env(char *name, char *value);
t_env			*init_minimal_env(void);
t_env			*init_env(char **envp);
t_shell			*init_shell(char **envp);

/* main/main.c */
void			cleanup_heredoc_files(t_cmd *cmd);

/* main/signals.c */
void			handle_sigint(int sig);
int				setup_signals(void);
int				setup_signals_for_commands(void);
void			reset_signals_for_child(void);

/*
** =================================================================
** PARSING
** =================================================================
*/

/* parsing/ast_entry.c */
t_ast			*parse_tokens(t_token *tokens);

/* parsing/cmd_utils.c */
int				count_args(t_token *tokens);
int				init_cmd(t_cmd *cmd, t_token *tokens);
int				add_command_arg(t_cmd *cmd, t_token *current, int i);
void			track_word_tokens(t_cmd *cmd, t_token *tokens);
t_cmd			*create_command(t_token *tokens);

/* parsing/parenthesis_operator.c */
t_ast_type		get_ast_type(t_token *token);
int				check_if_operator_is_last(t_token *first_op,
					t_token *second_op);
void			point_last_operator_outside_parenthesis(t_token **or,
					t_token **and,
					t_token **pipe,
					t_token *current);
t_token			*find_last_priority_operator(t_token *tokens);

/* parsing/parenthesis_synthax.c */
int				good_position_parenth(t_token *tmp);
int				verify_parenthesis(t_token *tokens);
int				check_syntax_error_parenthesis(t_token *tokens);

/* parsing/parenthesis_utils.c */
t_token			*find_matching_closing_par(t_token *opening);
t_token			*find_matching_opening_par(t_token *closing);
t_token			*find_opening_par(t_token *tokens);
t_token			*extract_parenthesis_content(t_token *start);

/* parsing/parser.c */
t_ast			*add_ast_node(t_token *tokens,
					t_token *current,
					t_ast_type type);
t_ast			*parse_command(t_token *tokens);
t_ast			*parse_pipes(t_token *tokens);
t_ast			*parse_logical_ops(t_token *tokens);
t_ast			*parse_parenthesis(t_token *tokens);

/* parsing/parser_utils.c */
t_ast			*init_ast_node(t_ast_type type);
t_token			*init_new_token(t_token *tmp, t_token *prev);
t_token			*find_last_token_of_type(t_token *tokens, t_type type);
t_token			*get_tokens_after(t_token *current);
t_token			*get_tokens_before(t_token *tokens, t_token *current);

/* parsing/redir_utils.c */
int				is_command_argument(t_type type);
int				is_redirection(t_type type);
t_token			*skip_redirection(t_token *current);
t_file_redir	*create_redir(t_token *current);
int				add_redirection_to_cmd(t_cmd *cmd, t_token *current);

/*
** =================================================================
** TOKENIZER
** =================================================================
*/

/* tokenizer/operator_token.c */
int				handle_double_operator(t_token *new, int *i, char *input);
int				handle_double_redir(t_token *new, int *i, char *input);
int				handle_single_operator(t_token *new, int *i, char *input);
int				handle_wild_and_parenthesis(t_token *new, int *i, char *input);
int				add_operator_token(t_token **list, int *i, char *input);

/* tokenizer/tokenizer.c */
int				add_node_back_word(t_token_word **list, t_token_word *new);
int				is_operator(char input);
int				add_node_back(t_token **list, t_token *new);
int				process_token(t_token **tokens, int *i, char *input);
t_token			*tokenize(char *input);

/* tokenizer/word_quote_token.c */
t_token_word	*define_word_quote_type(t_token_word **list,
					char *content,
					int *i,
					int *start);
t_token_word	*define_word_no_quote(t_token_word **list,
					char *content,
					int *i,
					int *start);
void			analyze_quote_content(t_token_word **list,
					t_token *new,
					char *content);
char			*create_content_word(char *input, int *i);
int				add_word_token(t_token **list, int *i, char *input);

/*
** =================================================================
** WILDCARDS
** =================================================================
*/

/* wildcards/expand_wildcards.c */
char			**realloc_capacity(char **files,
					int *capacity,
					int count,
					DIR *dir);
char			**fill_wildcard_array(DIR *dir,
					const char *wildcard_str,
					char **files,
					int *count);
char			**match_wildcard_array(char **files, int count);
char			**no_match_wildcard(const char *wildcard_str, char **files);
char			**expand_wildcard(const char *wildcard_str);

/* wildcards/has_wildcards.c */
int				match_wildcard(const char *wildcard_str, const char *str);
int				has_wildcard_in_word(t_token_word *word_parts);
int				has_wildcard_in_cmd(t_cmd *cmd);

/* wildcards/rebuilld_arg.c */
int				count_arg_after_expansion(t_cmd *cmd);
char			**create_new_args_array(t_cmd *cmd, int new_ac);
void			update_command(t_cmd *cmd, char **new_args, int new_ac);
int				rebuild_command_arg_wildcard(t_cmd *cmd);

/* wildcards/wildcard_redir.c */
char			*redirection_content(t_file_redir *redir);
int				ambiguous_multiple_file_detected(t_file_redir *redir);
int				rebuild_redirection_content(t_file_redir *redir);
int				expand_wildcard_in_redir(t_cmd *cmd, int expanded_something);

/* wildcards/wildcard_utils.c */
void			free_token_word_list(t_token_word *list);
t_token_word	*create_new_token_word(const char *content, t_type_word type);
void			free_files(char **files, int count, DIR *dir);

/* wildcards/wildcards.c */
char			*build_composite_wildcard_str(t_token_word *list);
t_token_word	*expanded_wildcard_list(char **expanded);
int				expand_wildcard_in_word_list(t_token_word **list);
int				launch_expand_wildcards(t_cmd *cmd);

#endif