/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:41:27 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:36:15 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Implemente la commande env qui affiche toutes les variables d'environnement.
** 1. Verifie qu'aucun argument n'est passe (env ne prend pas d'arguments)
** 2. Parcourt la liste chainee des variables d'environnement
** 3. Affiche chaque variable au format "NAME=value" sur stdout
** Retourne 127 si arguments detectes, 0 si affichage reussi.
** Reproduit le comportement standard de la commande env Unix.
*/
int	builtin_env(t_shell *shell)
{
	t_env	*current;

	if (shell->cmd->ac > 1)
	{
		ft_putstr_fd("env: too many arguments\n", 2);
		return (127);
	}
	current = shell->env;
	while (current)
	{
		ft_putstr_fd(current->name, 1);
		ft_putchar_fd('=', 1);
		ft_putendl_fd(current->value, 1);
		current = current->next;
	}
	return (0);
}
