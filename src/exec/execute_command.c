/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:16:13 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/18 23:02:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_builtin(char *cmd)
{
	return ((!ft_strncmp(cmd, "echo", 5))
		|| (!ft_strncmp(cmd, "cd", 3))
		|| (!ft_strncmp(cmd, "pwd", 4))
		|| (!ft_strncmp(cmd, "export", 7))
		|| (!ft_strncmp(cmd, "unset", 6))
		|| (!ft_strncmp(cmd, "env", 4))
		|| (!ft_strncmp(cmd, "exit", 5)));
}

int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!ft_strncmp(cmd->name, "echo", 4))
		return (builtin_echo(cmd));
	else if (!ft_strncmp(cmd->name, "cd", 2))
		return (builtin_cd(cmd, shell));
	else if (!ft_strncmp(cmd->name, "pwd", 3))
		return (builtin_pwd());
	else if (!ft_strncmp(cmd->name, "export", 6))
		return (builtin_export(cmd, shell));
	else if (!ft_strncmp(cmd->name, "unset", 5))
		return (builtin_unset(cmd, shell));
	else if (!ft_strncmp(cmd->name, "env", 3))
		return (builtin_env(shell));
	else if (!ft_strncmp(cmd->name, "exit", 4))
		return (builtin_exit(cmd, shell));
	return (1);
}

int	handle_builtin(t_cmd *cmd, t_shell *shell)
{
	int	stdin_backup;
	int	stdout_backup;
	int	status;

	if (cmd->type_redir)
	{
		stdin_backup = dup(STDIN_FILENO);
		stdout_backup = dup(STDOUT_FILENO);
		if (execute_redirections(cmd, shell) != 0)
			return (1);
		status = execute_builtin(cmd, shell);
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		return (status);
	}
	else
		return (execute_builtin(cmd, shell));
}

void	handle_command(t_cmd *cmd, t_shell *shell)
{
	char	**env_array;

	reset_signals_for_child();
	if (cmd->arg[0] == NULL)
	{
		if (execute_redirections(cmd, shell) != 0)
			exit(1);
	}
	if (cmd->type_redir && execute_redirections(cmd, shell) != 0)
		exit(1);
	cmd->path = find_command_path(cmd->name, shell);
	if (!cmd->path)
	{
		ft_putstr_fd(cmd->name, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	env_array = env_to_array(shell->env);
	if (!env_array)
		exit(1);
	execve(cmd->path, cmd->arg, env_array);
	perror("execve");
	exit(1);
}

int	handle_parent_process(pid_t pid, int is_minishell)
{
	int	status;

	waitpid(pid, &status, 0);
	if (((status & 0x7f) != 0) && ((status & 0x7f) == 2))
		write(STDOUT_FILENO, "\n", 1);
	if (is_minishell)
		setup_signals();
	if ((status & 0x7f) == 0)
		return ((status & 0xff00) >> 8);
	else if ((status & 0x7f) != 0)
		return (128 + (status & 0x7f));
	return (1);
}
