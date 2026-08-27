/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:15:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:47:50 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Convertit la liste chainee d'environnement en tableau de chaines.
** Chaque element au format "NAME=value" pour compatibilite avec sort.
** Utilise deux ft_strjoin successifs: nom+"=" puis +valeur.
** Termine le tableau par NULL pour les fonctions de manipulation.
*/
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

/*
** Affiche l'environnement au format bash "declare -x".
** Pour chaque variable: "declare -x NAME="value""
** Separe nom et valeur sur '=' et met la valeur entre guillemets.
** Variables sans valeur affichees sans guillemets ni '='.
*/
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

/*
** Trie le tableau d'environnement par ordre alphabetique.
** Algorithme bubble sort: compare chaque paire et echange si necessaire.
** Utilise ft_strncmp pour comparaison lexicographique standard.
** Modifie le tableau en place pour economiser la memoire.
*/
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

/*
** Affiche toutes les variables d'environnement triees (export sans args).
** 1. Compte les variables pour allouer le tableau
** 2. Convertit la liste en tableau de chaines 
** 3. Trie alphabetiquement et affiche au format declare -x
** Fonction principale appelee par "export" sans arguments.
*/
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

/*
** Point d'entree principal de la commande export.
** Sans arguments: affiche env trie au format declare -x
** Avec arguments: traite chaque argument (avec/sans affectation)
** Distingue "export VAR" (declaration) et "export VAR=val" (affectation).
** Accumule les erreurs dans status mais continue le traitement.
*/
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
