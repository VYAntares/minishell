/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 19:04:34 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:38:29 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_env	*create_env_node(const char *env_str)
{
	t_env		*node;
	char		*equals_pos;
	int			name_len;

	node = dmb_malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	equals_pos = ft_strchr(env_str, '=');
	if (!equals_pos)
		return (dmb_free(node), NULL);
	name_len = equals_pos - env_str;
	node->name = ft_substr(env_str, 0, name_len);
	if (!name_len)
		return (dmb_free(node), NULL);
	node->value = ft_strdup(equals_pos + 1);
	if (!node->value)
		return (dmb_free(node->name), dmb_free(node), NULL);
	node->next = NULL;
	return (node);
}

void	*create_minimal_env(char *name, char *value)
{
	t_env	*node;

	node = dmb_malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->name = ft_strdup(name);
	if (!node->name)
		return (dmb_free(node), NULL);
	node->value = ft_strdup(value);
	if (!node->value)
		return (dmb_free(node->name), dmb_free(node), NULL);
	node->next = NULL;
	return (node);
}

t_env	*init_minimal_env(void)
{
	t_env	*head;
	t_env	*path_node;
	t_env	*shlvl_node;
	char	cwd[PATH_MAX];
	char	*pwd_value;

	if (getcwd(cwd, PATH_MAX))
		pwd_value = ft_strdup(cwd);
	else
		pwd_value = ft_strdup("/");
	head = create_minimal_env("PWD", pwd_value);
	if (!head)
		return (NULL);
	path_node = create_minimal_env("PATH", "/usr/bin:/bin:/usr/sbin:/sbin");
	if (!path_node)
		return (dmb_free(head), NULL);
	head->next = path_node;
	shlvl_node = create_minimal_env("SHLVL", "1");
	if (!shlvl_node)
		return (dmb_free(head), dmb_free(shlvl_node), NULL);
	path_node->next = shlvl_node;
	return (head);
}

t_env	*init_env(char **envp)
{
	t_env		*head;
	t_env		*current;
	t_env		*new_node;
	int			i;

	if (!envp || !envp[0])
		return (init_minimal_env());
	head = create_env_node(envp[0]);
	if (!head)
		return (NULL);
	current = head;
	i = 1;
	while (envp[i])
	{
		new_node = create_env_node(envp[i]);
		if (!new_node)
			return (NULL);
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
	int			shell_lvl;
	char		*shell_lvl_str;

	shell = dmb_malloc(sizeof(t_shell));
	if (!shell)
		return (dmb_free(shell), NULL);
	env = init_env(envp);
	if (!env)
		return (dmb_free(shell), NULL);
	shell->env = env;
	shell->cmd = NULL;
	shell->history = NULL;
	shell->pid = getpid();
	shell->exit_status = 0;
	shell_lvl = ft_atoi(get_env_value(env, "SHLVL"));
	shell_lvl_str = ft_itoa(shell_lvl + 1);
	update_env_variable(shell, "SHLVL", shell_lvl_str);
	dmb_free(shell_lvl_str);
	return (shell);
}
