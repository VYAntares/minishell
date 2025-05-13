/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:07:45 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

sig_atomic_t	g_sigint_received;

void	free_shell(t_ast *ast, t_token *tokens, t_shell *shell)
{
	if (tokens)
		free_tokens(tokens);
	if (ast)
		free_ast(ast);
	rl_clear_history();
	free_env_list(shell->env);
	dmb_free(shell);
}

void	execute_line(char *input, t_token *tokens, t_ast *ast, t_shell *shell)
{
	add_history(input);
	if (tokens)
		free_tokens(tokens);
	if (ast)
		free_ast(ast);
	tokens = tokenize(input);
	if (tokens && check_syntax_error_parenthesis(tokens))
	{
		ast = parse_tokens(tokens);
		if (ast)
		{
			launch_heredoc(ast, shell);
			shell->exit_status = execute_ast(ast, shell);
			cleanup_heredoc_files(shell->cmd);
		}
	}
}

int	read_and_execute(t_token *tokens, t_ast *ast, t_shell *shell)
{
	char	*input;

	g_sigint_received = 0;
	input = readline("miniHell$ ");
	if (!input)
	{
		printf("exit\n");
		return (1);
	}
	if (*input)
		execute_line(input, tokens, ast, shell);
	dmb_force_free(input);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	t_shell		*shell;
	t_token		*tokens;
	t_ast		*ast;

	(void)ac;
	(void)av;
	tokens = NULL;
	ast = NULL;
	shell = init_shell(envp);
	if (!shell)
		return (1);
	if (setup_signals() == -1)
		return (free_env_list(shell->env), dmb_free(shell), 1);
	while (1)
	{
		if (read_and_execute(tokens, ast, shell))
			break ;
	}
	free_shell(ast, tokens, shell);
	dmb_gc();
	return (0);
}
