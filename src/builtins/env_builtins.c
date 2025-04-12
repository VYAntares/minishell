/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtins.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:15:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/12 11:28:32 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
			free(current->value);
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

	new_env = malloc(sizeof(t_env));
	if (!new_env)
		return (1);
	
	new_env->name = ft_strdup(name);
	if (!new_env->name)
		return (free(new_env), 1);
	new_env->value = ft_strdup(value);
	if (!new_env->value)
		return (free(new_env->name), free(new_env), 1);
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

int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*equals_pos;
	char	*name;
	char	*value;
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
			name = ft_substr(cmd->arg[i], 0, equals_pos - cmd->arg[i]);
			value = ft_strdup(equals_pos + 1);
			if (!name || !value)
			{
				free(name);
				free(value);
				return (1);
			}
			// Vérifier si le nom est vide ou invalide
			if (!*name || !is_valid_identifier(name))
			{
				ft_putstr_fd("minishell: export: '", 2);
				ft_putstr_fd(cmd->arg[i], 2);
				ft_putendl_fd("': not a valid identifier", 2);
				free(name);
				free(value);
				status = 1;
				i++;
				continue;
			}
			update_env_variable(shell, name, value);
			free(name);
			free(value);
		}
		else
		{
			if (!is_valid_identifier(cmd->arg[i]))
			{
				ft_putstr_fd("minishell: export: '", 2);
				ft_putstr_fd(cmd->arg[i], 2);
				ft_putendl_fd("': not a valid identifier", 2);
				status = 1;
			}
		}
		i++;
	}
	return (status);
}

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

int	print_sorted_env(t_shell *shell)
{
	t_env	*current;
	char	**env_array;
	int		count;
	int		i;
	int		j;
	char	*temp;

	// Compter les variables
	count = 0;
	current = shell->env;
	while (current)
	{
		count++;
		current = current->next;
	}
	
	// Créer un tableau pour le tri
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (1);
	
	// Remplir le tableau
	i = 0;
	current = shell->env;
	while (current)
	{
		env_array[i] = ft_strjoin(current->name, "=");
		char *tmp = ft_strjoin(env_array[i], current->value);
		free(env_array[i]);
		env_array[i] = tmp;
		i++;
		current = current->next;
	}
	env_array[i] = NULL;
	
	// Trier le tableau (tri à bulles simple)
	i = 0;
	while (env_array[i])
	{
		j = i + 1;
		while (env_array[j])
		{
			if (ft_strncmp(env_array[i], env_array[j], ft_strlen(env_array[j])) > 0)
			{
				temp = env_array[i];
				env_array[i] = env_array[j];
				env_array[j] = temp;
			}
			j++;
		}
		i++;
	}
	
	// Afficher
	i = 0;
	while (env_array[i])
	{
		ft_putstr_fd("declare -x ", 1);
		char *equals_pos = ft_strchr(env_array[i], '=');
		if (equals_pos)
		{
			// Afficher le nom
			write(1, env_array[i], equals_pos - env_array[i] + 1);

			// Afficher la valeur entre guillemets
			ft_putchar_fd('"', 1);
			ft_putstr_fd(equals_pos + 1, 1);
			ft_putchar_fd('"', 1);
		}
		else
		{
			ft_putstr_fd(env_array[i], 1);
		}
		ft_putchar_fd('\n', 1);
		i++;
	}
	
	// Libérer le tableau
	free_array(env_array);
	
	return (0);
}

int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	t_env	*current;
	t_env	*prev;
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
			status = 1;  // Code d'erreur pour identifiant invalide
			i++;
			continue;
		}
		
		// Chercher la variable
		current = shell->env;
		prev = NULL;
		
		while (current)
		{
			// Comparaison exacte de la longueur du nom
			if (ft_strncmp(current->name, cmd->arg[i], ft_strlen(cmd->arg[i])) == 0 &&
				ft_strlen(current->name) == ft_strlen(cmd->arg[i]))
			{
				// Supprimer la variable
				if (prev)
					prev->next = current->next;
				else
					shell->env = current->next;
				
				free(current->name);
				free(current->value);
				free(current);
				break;
			}
			prev = current;
			current = current->next;
		}
		
		i++;
	}
	return (status);
}