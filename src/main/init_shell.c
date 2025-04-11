/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_shell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 19:04:34 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/11 13:43:56 by eahmeti          ###   ########.fr       */
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

t_env *init_env(char **envp)
{
    t_env   *head;
    t_env   *current;
    t_env   *new_node;
    int     i;
    int     has_pwd = 0;
    int     has_path = 0;
    int     has_home = 0;
    int     has_shlvl = 0;
    char    cwd[PATH_MAX];

    // Si aucune variable d'environnement n'est fournie
    if (!envp || !envp[0])
    {
        head = NULL;
    }
    else
    {
        // Initialiser à partir de l'environnement fourni
        head = create_env_node(envp[0]);
        if (!head)
            return (NULL);
        
        // Vérifier si c'est PWD, PATH, HOME ou SHLVL
        if (ft_strncmp(head->name, "PWD", 4) == 0)
            has_pwd = 1;
        else if (ft_strncmp(head->name, "PATH", 5) == 0)
            has_path = 1;
        else if (ft_strncmp(head->name, "HOME", 5) == 0)
            has_home = 1;
        else if (ft_strncmp(head->name, "SHLVL", 6) == 0)
            has_shlvl = 1;
        
        current = head;
        i = 1;
        
        // Parcourir le reste des variables d'environnement
        while (envp[i])
        {
            new_node = create_env_node(envp[i]);
            if (!new_node)
                return (free_env_list(head), NULL);
            
            // Vérifier si c'est PWD, PATH, HOME ou SHLVL
            if (ft_strncmp(new_node->name, "PWD", 4) == 0)
                has_pwd = 1;
            else if (ft_strncmp(new_node->name, "PATH", 5) == 0)
                has_path = 1;
            else if (ft_strncmp(new_node->name, "HOME", 5) == 0)
                has_home = 1;
            else if (ft_strncmp(new_node->name, "SHLVL", 6) == 0)
                has_shlvl = 1;
            
            current->next = new_node;
            current = new_node;
            i++;
        }
    }

    // Ajouter les variables essentielles si elles sont manquantes
    if (!has_pwd)
    {
        new_node = malloc(sizeof(t_env));
        if (!new_node)
            return (free_env_list(head), NULL);
        
        new_node->name = ft_strdup("PWD");
        if (getcwd(cwd, PATH_MAX))
            new_node->value = ft_strdup(cwd);
        else
            new_node->value = ft_strdup("/");
        
        new_node->next = NULL;
        
        if (!head)
            head = new_node;
        else
        {
            current = head;
            while (current->next)
                current = current->next;
            current->next = new_node;
        }
    }
    
    if (!has_path)
    {
        new_node = malloc(sizeof(t_env));
        if (!new_node)
            return (free_env_list(head), NULL);
        
        new_node->name = ft_strdup("PATH");
        new_node->value = ft_strdup("/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin");
        new_node->next = NULL;
        
        if (!head)
            head = new_node;
        else
        {
            current = head;
            while (current->next)
                current = current->next;
            current->next = new_node;
        }
    }
    
    if (!has_home)
    {
        new_node = malloc(sizeof(t_env));
        if (!new_node)
            return (free_env_list(head), NULL);
        
        new_node->name = ft_strdup("HOME");
        new_node->value = ft_strdup("/");
        new_node->next = NULL;
        
        if (!head)
            head = new_node;
        else
        {
            current = head;
            while (current->next)
                current = current->next;
            current->next = new_node;
        }
    }
    
    if (!has_shlvl)
    {
        new_node = malloc(sizeof(t_env));
        if (!new_node)
            return (free_env_list(head), NULL);
        
        new_node->name = ft_strdup("SHLVL");
        new_node->value = ft_strdup("1");
        new_node->next = NULL;
        
        if (!head)
            head = new_node;
        else
        {
            current = head;
            while (current->next)
                current = current->next;
            current->next = new_node;
        }
    }
    
    return (head);
}

// t_env	*init_env(char **envp)
// {
// 	t_env		*head;
// 	t_env		*current;
// 	t_env		*new_node;
// 	int			i;

// 	head = create_env_node(envp[0]);
// 	if (!head)
// 		return (NULL);
// 	current = head;
// 	i = 1;
// 	while (envp[i])
// 	{
// 		new_node = create_env_node(envp[i]);
// 		if (!new_node)
// 			return (free_env_list(head), NULL);
// 		current->next = new_node;
// 		current = new_node;
// 		i++;
// 	}
// 	return (head);
// }

t_shell	*init_shell(char **envp)
{
	t_shell		*shell;
	t_env		*env;
	int			shell_lvl;

	shell = malloc(sizeof(t_shell));
	if (!shell)
		return (free(shell), NULL);
	env = init_env(envp);
	if (!env)
		return (free(shell), NULL);
	shell->env = env;
	shell->cmd = NULL;
	shell->history = NULL;
	shell->pid = getpid();
	shell->exit_status = 0;
	shell_lvl = ft_atoi(get_env_value(shell->env, "SHLVL"));
	update_env_variable(shell, "SHLVL", ft_itoa(shell_lvl + 1));
	return (shell);
}
