/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:48:24 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Implemente la commande pwd (print working directory).
** Utilise getcwd() pour recuperer le repertoire de travail courant.
** Affiche le chemin complet sur stdout si succes.
** En cas d'erreur: affiche le message d'erreur systeme sur stderr.
** Retourne 0 si succes, 1 en cas d'erreur (repertoire supprime, etc).
*/
int	builtin_pwd(void)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, PATH_MAX))
	{
		ft_putendl_fd(cwd, 1);
		return (0);
	}
	else
	{
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}
}
