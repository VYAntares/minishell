/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 19:04:34 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 17:47:12 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_env_list(t_env *head)
{
	t_env		*current;
	t_env		*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->name);
		free(current->value);
		free(current);
		current = next;
	}
}

t_env	*create_env_node(const char *env_str)
{
	t_env		*node;
	char		*equals_pos;
	int			name_len;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	equals_pos = ft_strchr(env_str, '=');
	if (!equals_pos)
		return (free(node), NULL);
	name_len = equals_pos - env_str;
	node->name = ft_substr(env_str, 0, name_len);
	if (!name_len)
		return (free(node), NULL);
	node->value = ft_strdup(equals_pos + 1);
	if (!node->value)
		return (free(node->name), free(node), NULL);
	node->next = NULL;
	return (node);
}

t_env	*init_env(char **envp)
{
	t_env		*head;
	t_env		*current;
	t_env		*new_node;
	int			i;

	head = create_env_node(envp[0]);
	if (!head)
		return (NULL);
	current = head;
	i = 1;
	while (envp[i])
	{
		new_node = create_env_node(envp[i]);
		if (!new_node)
			return (free_env_list(head), NULL);
		current->next = new_node;
		current = new_node;
		i++;
	}
	return (head);
}

t_shell	*init_shell(char **envp)
{
	t_shell		*shell;
	t_env		*env;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (free(shell), NULL);
	env = init_env(envp);
	if (!env)
		return (free(shell), NULL);
	shell->env = env;
	shell->cmd = NULL;
	shell->history = NULL;
	shell->exit_status = 0;
	return (shell);
}
