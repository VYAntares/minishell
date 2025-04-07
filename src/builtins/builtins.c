/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/07 16:26:49 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/**
 * @brief Builtin echo - Affiche les arguments
 * 
 * @param cmd Structure de commande
 * @return int 0 pour succès
 */
int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	
	// Vérifier l'option -n
	while (cmd->arg[i] && ft_strncmp(cmd->arg[i], "-n", 2) == 0)
	{
		n_flag = 1;
		i++;
	}
	
	// Afficher les arguments
	while (cmd->arg[i])
	{
		ft_putstr_fd(cmd->arg[i], 1);
		if (cmd->arg[i + 1])
			ft_putchar_fd(' ', 1);
		i++;
	}
	
	// Nouvelle ligne si pas d'option -n
	if (!n_flag)
		ft_putchar_fd('\n', 1);
	
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	char	*old_pwd;
	char	*new_pwd;
	char	cwd[PATH_MAX];

	// Récupérer le chemin
	if (cmd->arg[1])
		path = cmd->arg[1];
	else
	{
		path = get_env_value(shell->env, "HOME");
		if (!path)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (1);
		}
	}
	
	// Sauvegarder l'ancien PWD
	old_pwd = NULL;
	if (getcwd(cwd, PATH_MAX))
		old_pwd = ft_strdup(cwd);
	
	// Changer de répertoire
	if (chdir(path) != 0)
	{
		ft_putstr_fd("cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		free(old_pwd);
		return (1);
	}
	
	// Mettre à jour PWD et OLDPWD
	new_pwd = getcwd(cwd, PATH_MAX);
	if (new_pwd)
	{
		update_env_variable(shell, "PWD", new_pwd);
		if (old_pwd)
			update_env_variable(shell, "OLDPWD", old_pwd);
	}
	
	free(old_pwd);
	return (0);
}

/**
 * @brief Builtin pwd - Affiche le répertoire courant
 * 
 * @return int 0 pour succès, 1 pour erreur
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

/**
 * @brief Builtin env - Affiche l'environnement
 * 
 * @param shell Structure shell
 * @return int 0 pour succès
 */
int	builtin_env(t_shell *shell)
{
	t_env	*current;

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

/**
 * @brief Builtin exit - Quitte le shell
 * 
 * @param cmd Structure de commande
 * @param shell Structure shell
 * @return int Code de sortie
 */
int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	int	exit_code;

	ft_putendl_fd("exit", 1);
	
	if (cmd->arg[1])
	{
		// Vérifier que l'argument est un nombre
		if (!is_number(cmd->arg[1]))
		{
			ft_putstr_fd("exit: ", 2);
			ft_putstr_fd(cmd->arg[1], 2);
			ft_putendl_fd(": numeric argument required", 2);
			exit_code = 255;
		}
		else if (cmd->arg[2])
		{
			ft_putendl_fd("exit: too many arguments", 2);
			return (1);
		}
		else
		{
			exit_code = ft_atoi(cmd->arg[1]);
		}
	}
	else
	{
		exit_code = shell->exit_status;
	}
	
	// Libérer toutes les ressources
	if (shell->cmd)
		free_cmd(shell->cmd);
	free_env_list(shell->env);
	free(shell);
	rl_clear_history();
	
	exit(exit_code & 0xFF);
}

/**
 * @brief Vérifie si une chaîne est un nombre valide
 * 
 * @param str Chaîne à vérifier
 * @return int 1 si c'est un nombre, 0 sinon
 */
int	is_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}