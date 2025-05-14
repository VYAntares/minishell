/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 17:49:55 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:17:33 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	execute_command(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		is_minishell;

	if (!cmd || !cmd->name || !cmd->arg[0])
		return (execute_redirections(cmd, shell), 1);
	if (expand_var(cmd, shell) != 0 || launch_expand_wildcards(cmd) != 0)
		return (1);
	if (is_builtin(cmd->name))
		return (handle_builtin(cmd, shell));
	is_minishell = (ft_strncmp(cmd->name, "./minishell", 11) == 0);
	if (is_minishell)
		setup_signals_for_commands();
	pid = fork();
	if (pid == -1)
	{
		if (is_minishell)
			setup_signals();
		return (perror("fork"), dmb_gc(), 1);
	}
	if (pid == 0)
	{
		handle_command(cmd, shell);
		dmb_gc();
	}
	return (handle_parent_process(pid, is_minishell));
}

int	execute_subshell(t_ast *sub_shell, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		exit_code;

	pid = fork();
	if (pid == 0)
	{
		exit_code = execute_ast(sub_shell, shell);
		dmb_gc();
		exit(exit_code);
	}
	if (pid < 0)
	{
		perror("minishell: fork");
		dmb_gc();
		return (1);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (status & 0xFF)
			return (1);
		else
			return ((status >> 8) & 0xFF);
	}
}

int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	else if (ast->type == AST_CMD)
		return (execute_command(ast->cmd, shell));
	else if (ast->type == AST_PIPE)
		return (execute_pipe(ast->left, ast->right, shell));
	else if (ast->type == AST_AND)
	{
		if (execute_ast(ast->left, shell) == 0)
			return (execute_ast(ast->right, shell));
		return (1);
	}
	else if (ast->type == AST_OR)
	{
		if (execute_ast(ast->left, shell) != 0)
			return (execute_ast(ast->right, shell));
		return (0);
	}
	else if (ast->type == AST_SUB_SHELL)
		return (execute_subshell(ast->sub_shell, shell));
	return (1);
}
