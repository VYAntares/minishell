/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:15:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:14:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
