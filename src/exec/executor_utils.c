/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 01:28:58 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 00:22:25 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*assemble_array(char *name, char *value)
{
	char	*tmp1;
	char	*tmp2;

	tmp1 = ft_strjoin(name, "=");
	if (!tmp1)
		return (NULL);
	tmp2 = ft_strjoin(tmp1, value);
	if (!tmp2)
		return (dmb_free(tmp1), NULL);
	dmb_free(tmp1);
	return (tmp2);
}

char	**env_to_array(t_env *env)
{
	int		count;
	t_env	*current;
	char	**array;
	int		i;

	count = 0;
	current = env;
	while (current)
	{
		count++;
		current = current->next;
	}
	array = dmb_malloc(sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	i = 0;
	current = env;
	while (current)
	{
		array[i] = assemble_array(current->name, current->value);
		if (!array[i++])
			return (NULL);
		current = current->next;
	}
	return (array[i] = NULL, array);
}

char	*test_path_for_command(char *directory, char *command_name)
{
	char	*slash_name;
	char	*full_path;

	slash_name = ft_strjoin("/", command_name);
	if (!slash_name)
		return (NULL);
	full_path = ft_strjoin(directory, slash_name);
	dmb_free(slash_name);
	if (!full_path)
		return (NULL);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	dmb_free(full_path);
	return (NULL);
}

char	*extract_path(char *path, char *command_name)
{
	int		start;
	int		end;
	char	*directory;
	char	*valid_path;

	start = 0;
	end = 0;
	while (1)
	{
		while (path[end] && path[end] != ':')
			end++;
		directory = ft_substr(path, start, end - start);
		if (!directory)
			return (NULL);
		valid_path = test_path_for_command(directory, command_name);
		dmb_free(directory);
		if (valid_path)
			return (valid_path);
		if (!path[end])
			break ;
		end++;
		start = end;
	}
	return (NULL);
}

char	*find_command_path(char *name, t_shell *shell)
{
	char	*env_path;
	t_env	*env;

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
			break ;
		env = env->next;
	}
	if (!env)
		return (NULL);
	env_path = env->value;
	return (extract_path(env_path, name));
}
