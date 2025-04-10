/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 17:49:55 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/10 13:02:06 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_builtin(char *cmd)
{
	return ((!ft_strncmp(cmd, "echo", 4) && ft_strlen(cmd) == 4)
		|| (!ft_strncmp(cmd, "cd", 2) && ft_strlen(cmd) == 2)
		|| (!ft_strncmp(cmd, "pwd", 3) && ft_strlen(cmd) == 3)
		|| (!ft_strncmp(cmd, "export", 6) && ft_strlen(cmd) == 6)
		|| (!ft_strncmp(cmd, "unset", 5) && ft_strlen(cmd) == 5)
		|| (!ft_strncmp(cmd, "env", 3) && ft_strlen(cmd) == 3)
		|| (!ft_strncmp(cmd, "exit", 4) && ft_strlen(cmd) == 4));
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

char	**env_to_array(t_env *env)
{
	int		count;
	t_env	*current;
	char	**array;
	char	*tmp1;
	char	*tmp2;
	int		i;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	array = malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	i = 0;
	current = env;
	while (current)
	{
		tmp1 = ft_strjoin(current->name, "=");
		if (!tmp1)
			return (free_array(array), NULL);
		tmp2 = ft_strjoin(tmp1, current->value);
		if (!tmp2)
			return (free(tmp1), free_array(array), NULL);
		array[i] = tmp2;
		free(tmp1);
		i++;
		current = current->next;
	}
	array[i] = NULL;
	return (array);
}

char    *extract_path(char *path, char *command_name)
{
	int     start;
	int     end;
	char    *path_name;
	char    *slash_name;

	start = 0;
	end = 0;
	slash_name = ft_strjoin("/", command_name);
	if (!slash_name)
		return (NULL);
	while (path[end])
	{
		if (path[end] == ':' || path[end + 1] == '\0')
		{
			if (path[end + 1] == '\0')
				end++;
			path_name = ft_strjoin(ft_substr(path, start, end - start), slash_name);
			if (!path_name)
				return (free(slash_name), NULL);
			if (access(path_name, X_OK) == 0)
				return (free(slash_name), path_name);
			free(path_name);
			start = end + 1;
		}
		end++;
	}
	free(slash_name);
	return (NULL);
}

char    *find_command_path(char *name, t_shell *shell)
{
	char    *env_path;
	t_env   *env;

	if (ft_strchr(name, '/'))
	{
		if (access(name, X_OK) == 0)
			return (ft_strdup(name));
		return (NULL);
	}
	env = shell->env;
	while (env)
	{
		if (ft_strncmp(env->name, "PATH", 4) == 0)
			break;
		env = env->next;
	}
	if (!env)
		return (NULL);
	env_path = env->value;
	return (extract_path(env_path, name));
}

int execute_command(t_cmd *cmd, t_shell *shell)
{
	int		status;
	pid_t	pid;
	char	**env_array;
	int		stdin_backup;
	int		stdout_backup;

	if (!cmd || !cmd->name)
		return (1);
	if (expand_var(cmd, shell) != 0)
		return (1);
	if (is_builtin(cmd->name))
	{
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
	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
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
		free_array(env_array);
		perror("execve");
		exit(1);
	}
	waitpid(pid, &status, 0);
	if ((status & 0x7f) == 0)
		return ((status & 0xff00) >> 8);
	return (1);
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
		exit(exit_code);
	}
	if (pid < 0)
	{
		perror("minishell: fork");
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

int execute_ast(t_ast *ast, t_shell *shell)
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
