/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 17:49:55 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 18:45:48 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// int	is_builtin(char *cmd)
// {
// 	return (!ft_strcmp(cmd, "echo")
// 		|| !ft_strcmp(cmd, "cd")
// 		|| !ft_strcmp(cmd, "pwd")
// 		|| !ft_strcmp(cmd, "export")
// 		|| !ft_strcmp(cmd, "unset")
// 		|| !ft_strcmp(cmd, "env")
// 		|| !ft_strcmp(cmd, "exit"));
// }

// int	excecute_builtin(t_cmd *cmd)
// {
// 	if (!ft_strncmp(cmd->name, "echo", 4))
// 		return (builtin_echo(cmd));
// 	else if (!ft_strncmp(cmd->name, "cd", 2))
// 		return (builtin_cd(cmd));
// 	else if (!ft_strncmp(cmd->name, "pwd", 3))
// 		return (builtin_pwd(cmd));
// 	else if (!ft_strncmp(cmd->name, "export", 6))
// 		return (builtin_export(cmd));
// 	else if (!ft_strncmp(cmd->name, "unset", 5))
// 		return (builtin_unset(cmd));
// 	else if (!ft_strncmp(cmd->name, "env", 3))
// 		return (builtin_env(cmd));
// 	else if (!ft_strncmp(cmd->name, "exit", 4))
// 		return (builtin_exit(cmd));
// 	return (1);
// }

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
			return (NULL);  // implementer free_array
		tmp2 = ft_strjoin(tmp1, current->value);
		if (!tmp1)
			return (free(tmp1), NULL); //free_array
		array[i] = tmp2;
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
	while (path[end])
	{
		if (path[end] == ':' || path[end + 1] == '\0')
		{
			if (path[end + 1] == '\0')
				end++;
			path_name = ft_strjoin(ft_substr(path, start, end - start), slash_name);
			if (!path_name)
				return (NULL);
			if (access(path_name, X_OK) == 0)
				return (free(slash_name), path_name);
			free(path_name);
			start = end + 1;
		}
		end++;
	}
	return (NULL); // jsp si vrmt utile mais au cas ou
}

char    *find_command_path(char *name, t_shell *shell)
{
	char    *env_path;
	t_env   *env;

	env = shell->env;

	while (env)
	{
		if (ft_strncmp(env->name, "PATH", 4) == 0)
			break;
		env = env->next;
	}
	env_path = env->value;
	return (extract_path(env_path, name));
}

int execute_command(t_cmd *cmd, t_shell *shell)
{
	int		status;
	pid_t	pid;
	char	**env_array;
	
	if (!cmd || !cmd->name)
		return (1);
	// if (is_builtin(cmd->name))
	// 	return (excecute_builtin(cmd));
	pid = fork();
	if (pid == -1)
		return (perror("fork"), 1);
	if (pid == 0)
	{
		// gerer redirection !!!

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
		//free_array(env_array);
		perror("execve");
		exit(1);
	}
	// Processus parent
	waitpid(pid, &status, 0);
	// Si le processus s'est terminé normalement (équivalent de WIFEXITED)
	if ((status & 0x7f) == 0)
		return ((status & 0xff00) >> 8);         // Récupérer le code de retour (équivalent de WEXITSTATUS)
	return (1);
}

int execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);

	else if (ast->type == AST_CMD)
		return (execute_command(ast->cmd, shell));

	// else if (ast->type == AST_PIPE)
	// 	return (excecute_pipe(ast->left, ast->right, shell));

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
		return (execute_ast(ast->sub_shell, shell));
	return (1);
}
