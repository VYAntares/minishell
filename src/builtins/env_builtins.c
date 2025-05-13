/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtins.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:15:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:35:43 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_valid_identifier(const char *id)
{
	int	i;

	if (!id || !*id)
		return (0);
	if (!ft_isalpha(id[0]) && id[0] != '_')
		return (0);
	i = 1;
	while (id[i])
	{
		if (!ft_isalnum(id[i]) && id[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	update_env_variable(t_shell *shell, const char *name, const char *value)
{
	t_env	*current;
	char	*new_value;

	current = shell->env;
	while (current)
	{
		if (ft_strncmp(current->name, name, ft_strlen(name) + 1) == 0)
		{
			new_value = ft_strdup(value);
			if (!new_value)
				return (1);
			dmb_free(current->value);
			current->value = new_value;
			return (0);
		}
		current = current->next;
	}
	return (add_env_variable(shell, name, value));
}

int	add_env_variable(t_shell *shell, const char *name, const char *value)
{
	t_env	*new_env;
	t_env	*current;

	new_env = dmb_malloc(sizeof(t_env));
	if (!new_env)
		return (1);
	new_env->name = ft_strdup(name);
	if (!new_env->name)
		return (dmb_free(new_env), 1);
	new_env->value = ft_strdup(value);
	if (!new_env->value)
		return (dmb_free(new_env->name), dmb_free(new_env), 1);
	new_env->next = NULL;
	if (!shell->env)
		shell->env = new_env;
	else
	{
		current = shell->env;
		while (current->next)
			current = current->next;
		current->next = new_env;
	}
	return (0);
}

// Gère le cas d'une variable avec une valeur (contenant '=')
int	handle_export_with_value(char *arg, t_shell *shell, int *status)
{
	char	*equals_pos;
	char	*name;
	char	*value;

	equals_pos = ft_strchr(arg, '=');
	name = ft_substr(arg, 0, equals_pos - arg);
	value = ft_strdup(equals_pos + 1);
	if (!name || !value)
		return (dmb_free(name), dmb_free(value), 1);
	if (!*name || !is_valid_identifier(name))
	{
		ft_putstr_fd("minishell: export: '", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		*status = 1;
		return (dmb_free(name), dmb_free(value), 0);
	}
	update_env_variable(shell, name, value);
	return (dmb_free(name), dmb_free(value), 0);
}

// Gère le cas d'une variable déclarée sans valeur
int	handle_export_declaration(char *arg, int *status)
{
	if (!is_valid_identifier(arg))
	{
		ft_putstr_fd("minishell: export: '", 2);
		ft_putstr_fd(arg, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		*status = 1;
	}
	return (0);
}

// Fonction principale export refactorisée
int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*equals_pos;
	int		status;

	if (!cmd->arg[1])
		return (print_sorted_env(shell));
	i = 1;
	status = 0;
	while (cmd->arg[i])
	{
		equals_pos = ft_strchr(cmd->arg[i], '=');
		if (equals_pos)
		{
			if (handle_export_with_value(cmd->arg[i], shell, &status))
				return (1);
		}
		else
			handle_export_declaration(cmd->arg[i], &status);
		i++;
	}
	return (status);
}

void	create_and_fill_env(char **env_array, t_shell *shell)
{
	int		i;
	t_env	*current;
	char	*tmp;

	i = 0;
	current = shell->env;
	while (current)
	{
		env_array[i] = ft_strjoin(current->name, "=");
		tmp = ft_strjoin(env_array[i], current->value);
		dmb_free(env_array[i]);
		env_array[i] = tmp;
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
}

void	display_array(char **env_array)
{
	char	*equals_pos;
	int		i;

	i = 0;
	while (env_array[i])
	{
		ft_putstr_fd("declare -x ", 1);
		equals_pos = ft_strchr(env_array[i], '=');
		if (equals_pos)
		{
			write(1, env_array[i], equals_pos - env_array[i] + 1);
			ft_putchar_fd('"', 1);
			ft_putstr_fd(equals_pos + 1, 1);
			ft_putchar_fd('"', 1);
		}
		else
			ft_putstr_fd(env_array[i], 1);
		ft_putchar_fd('\n', 1);
		i++;
	}
}

void	bubble_sort(char **env_array)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (env_array[i])
	{
		j = i + 1;
		while (env_array[j])
		{
			if (ft_strncmp(env_array[i],
					env_array[j],
					ft_strlen(env_array[j])) > 0)
			{
				temp = env_array[i];
				env_array[i] = env_array[j];
				env_array[j] = temp;
			}
			j++;
		}
		i++;
	}
}

int	print_sorted_env(t_shell *shell)
{
	t_env	*current;
	char	**env_array;
	int		count;

	count = 0;
	current = shell->env;
	while (current)
	{
		count++;
		current = current->next;
	}
	env_array = dmb_malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (1);
	create_and_fill_env(env_array, shell);
	bubble_sort(env_array);
	display_array(env_array);
	return (0);
}

void	delete_line_env(t_shell *shell, t_cmd *cmd, int i)
{
	t_env	*current;
	t_env	*prev;

	current = shell->env;
	prev = NULL;
	while (current)
	{
		if (ft_strncmp(current->name, cmd->arg[i], ft_strlen(cmd->arg[i]))
			== 0 && ft_strlen(current->name) == ft_strlen(cmd->arg[i]))
		{
			if (prev)
				prev->next = current->next;
			else
				shell->env = current->next;
			dmb_free(current->name);
			dmb_free(current->value);
			dmb_free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	int		status;

	i = 1;
	status = 0;
	while (cmd->arg[i])
	{
		if (!is_valid_identifier(cmd->arg[i]))
		{
			ft_putstr_fd("minishell: unset: '", 2);
			ft_putstr_fd(cmd->arg[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			status = 1;
			i++;
			continue ;
		}
		delete_line_env(shell, cmd, i);
		i++;
	}
	return (status);
}
