/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_builtins.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:15:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/03 14:50:47 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Met à jour une variable d'environnement
 * 
 * @param shell Structure shell
 * @param name Nom de la variable
 * @param value Nouvelle valeur
 * @return int 0 pour succès, 1 pour erreur
 */
int	update_env_variable(t_shell *shell, const char *name, const char *value)
{
	t_env	*current;
	char	*new_value;

	current = shell->env;
	while (current)
	{
		if (ft_strncmp(current->name, name, ft_strlen(name)) == 0)
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
	
	// Si on arrive ici, la variable n'existe pas
	return (add_env_variable(shell, name, value));
}

/**
 * @brief Ajoute une nouvelle variable d'environnement
 * 
 * @param shell Structure shell
 * @param name Nom de la variable
 * @param value Valeur de la variable
 * @return int 0 pour succès, 1 pour erreur
 */
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
	{
		shell->env = new_env;
	}
	else
	{
		current = shell->env;
		while (current->next)
			current = current->next;
		current->next = new_env;
	}
	
	return (0);
}

/**
 * @brief Builtin export - Ajoute des variables à l'environnement
 * 
 * @param cmd Structure de commande
 * @param shell Structure shell
 * @return int 0 pour succès, 1 pour erreur
 */
int	builtin_export(t_cmd *cmd, t_shell *shell)
{
	int		i;
	char	*equals_pos;
	char	*name;
	char	*value;

	// Si pas d'arguments, afficher les variables avec "declare -x"
	if (!cmd->arg[1])
		return (print_sorted_env(shell));
	
	i = 1;
	while (cmd->arg[i])
	{
		equals_pos = ft_strchr(cmd->arg[i], '=');
		if (equals_pos)
		{
			// Format NAME=VALUE
			name = ft_substr(cmd->arg[i], 0, equals_pos - cmd->arg[i]);
			value = ft_strdup(equals_pos + 1);
			if (!name || !value)
			{
				free(name);
				free(value);
				return (1);
			}
			
			if (!is_valid_identifier(name))
			{
				ft_putstr_fd("export: '", 2);
				ft_putstr_fd(cmd->arg[i], 2);
				ft_putendl_fd("': not a valid identifier", 2);
				free(name);
				free(value);
				i++;
				continue;
			}
			
			update_env_variable(shell, name, value);
			free(name);
			free(value);
		}
		else
		{
			// Format NAME
			if (!is_valid_identifier(cmd->arg[i]))
			{
				ft_putstr_fd("export: '", 2);
				ft_putstr_fd(cmd->arg[i], 2);
				ft_putendl_fd("': not a valid identifier", 2);
			}
			// Pas d'action si pas de '='
		}
		i++;
	}
	
	return (0);
}

/**
 * @brief Vérifie si un identifiant est valide pour une variable
 * 
 * @param id Identifiant à vérifier
 * @return int 1 si valide, 0 sinon
 */
int	is_valid_identifier(const char *id)
{
	int	i;

	if (!id || !*id)
		return (0);
	
	// Premier caractère: lettre ou underscore
	if (!ft_isalpha(id[0]) && id[0] != '_')
		return (0);
	
	// Autres caractères: lettres, chiffres ou underscore
	i = 1;
	while (id[i])
	{
		if (!ft_isalnum(id[i]) && id[i] != '_')
			return (0);
		i++;
	}
	
	return (1);
}

/**
 * @brief Affiche l'environnement trié avec "declare -x"
 * 
 * @param shell Structure shell
 * @return int 0 pour succès
 */
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
			printf("%s", env_array[i]);
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

/**
 * @brief Builtin unset - Supprime des variables de l'environnement
 * 
 * @param cmd Structure de commande
 * @param shell Structure shell
 * @return int 0 pour succès, 1 pour erreur
 */
int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	t_env	*current;
	t_env	*prev;

	i = 1;
	while (cmd->arg[i])
	{
		if (!is_valid_identifier(cmd->arg[i]))
		{
			ft_putstr_fd("unset: '", 2);
			ft_putstr_fd(cmd->arg[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			i++;
			continue;
		}
		
		// Chercher la variable
		current = shell->env;
		prev = NULL;
		
		while (current)
		{
			if (ft_strncmp(current->name, cmd->arg[i], ft_strlen(cmd->arg[i])) == 0)
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
	
	return (0);
}