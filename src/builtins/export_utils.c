/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:52:30 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:45:18 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Verifie qu'un nom de variable respecte les regles Unix.
** Doit commencer par lettre ou underscore, puis lettres/chiffres/underscore.
** Exemples valides: "VAR", "_test", "PATH2" / Invalides: "2VAR", "test-var"
** Retourne 1 si valide, 0 sinon.
*/
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

/*
** Gere une declaration export sans valeur (ex: export VAR).
** Valide que l'identifiant respecte les regles de nommage.
** Affiche une erreur si l'identifiant est invalide et met status=1.
** Ne cree pas la variable, juste validation pour compatibilite bash.
*/
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

/*
** Ajoute une nouvelle variable d'environnement a la fin de la liste.
** Alloue un nouveau noeud t_env avec nom et valeur dupliques.
** Gere le cas de la premiere variable (liste vide).
** Retourne 0 si succes, 1 en cas d'erreur d'allocation.
*/
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

/*
** Met a jour une variable existante ou l'ajoute si inexistante.
** Parcourt la liste pour trouver le nom correspondant.
** Si trouve: remplace la valeur, si absent: ajoute en fin de liste.
** Strategie: chercher puis modifier, ou creer si pas trouve.
*/
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

/*
** Traite un export avec affectation (ex: export VAR=value).
** 1. Parse l'argument pour separer nom et valeur sur '='
** 2. Valide l'identifiant selon les regles Unix
** 3. Met a jour ou cree la variable dans l'environnement
** Gere les erreurs de parsing et de validation proprement.
*/
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
