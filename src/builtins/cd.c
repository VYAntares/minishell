/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:39:02 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/18 22:38:49 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
