/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:39:02 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:34:59 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Met a jour le repertoire courant et les variables PWD/OLDPWD.
** 1. Sauvegarde le repertoire actuel avec getcwd() dans old_pwd
** 2. Change de repertoire avec chdir(), affiche erreur si echec
** 3. Recupere le nouveau repertoire et met a jour PWD/OLDPWD
** Maintient la coherence entre le systeme et les variables shell.
*/
int	update_repertory(char *path, t_shell *shell)
{
	char	*old_pwd;
	char	cwd[PATH_MAX];
	char	*new_pwd;

	old_pwd = NULL;
	if (getcwd(cwd, PATH_MAX))
		old_pwd = ft_strdup(cwd);
	if (chdir(path) != 0)
	{
		ft_putstr_fd("miniHell: ", 2);
		ft_putstr_fd("cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		dmb_free(old_pwd);
		return (1);
	}
	new_pwd = getcwd(cwd, PATH_MAX);
	if (new_pwd)
	{
		update_env_variable(shell, "PWD", new_pwd);
		if (old_pwd)
			update_env_variable(shell, "OLDPWD", old_pwd);
	}
	return (dmb_free(old_pwd), 0);
}

/*
** Implemente la commande cd (change directory) du shell.
** Sans argument ou avec ~: va dans HOME (repertoire utilisateur)
** Avec argument: va dans le repertoire specifie (relatif ou absolu)
** Gere l'erreur si HOME n'est pas defini dans l'environnement.
** Retourne 0 si succes, 1 en cas d'erreur de changement.
*/
int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->arg[1] || !ft_strncmp(cmd->arg[1], "~", 2))
	{
		path = get_env_value(shell->env, "HOME");
		if (!path)
			return (ft_putstr_fd("cd: HOME not set\n", 2), 1);
	}
	else
		path = cmd->arg[1];
	if (update_repertory(path, shell))
		return (1);
	return (0);
}
