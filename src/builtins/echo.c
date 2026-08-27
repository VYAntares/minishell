/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:36:35 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:35:34 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Implemente la commande echo du shell avec support de l'option -n.
** 1. Detecte et traite tous les flags -n consecutifs au debut
** 2. Affiche tous les arguments restants separes par des espaces
** 3. Ajoute un retour a la ligne final sauf si flag -n present
** Respecte le comportement bash: echo -n -n hello (pas de newline)
** Retourne toujours 0 car echo ne peut pas echouer.
*/
int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	while (cmd->arg[i] && ft_strncmp(cmd->arg[i], "-n", 2) == 0)
	{
		n_flag = 1;
		i++;
	}
	while (cmd->arg[i])
	{
		ft_putstr_fd(cmd->arg[i], 1);
		if (cmd->arg[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', 1);
	return (0);
}
