/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:39:53 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:42:14 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Verifie si une chaine represente un nombre entier valide.
** Accepte les signes + et - en debut de chaine.
** Rejette les chaines vides, non numeriques ou avec signes isoles.
** Exemples valides: "123", "-42", "+0" / Invalides: "12a", "+", ""
** Retourne 1 si nombre valide, 0 sinon.
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

/*
** Affiche le message d'erreur pour un argument non numerique d'exit.
** Format: "exit: [argument]: numeric argument required"
** Reproduit le comportement bash pour les erreurs d'exit.
** Retourne 255 (code d'erreur standard pour argument invalide).
*/
int	print_error_exit(t_cmd *cmd)
{
	ft_putstr_fd("exit: ", 2);
	ft_putstr_fd(cmd->arg[1], 2);
	ft_putendl_fd(": numeric argument required", 2);
	return (255);
}

/*
** Implemente la commande exit qui termine le shell.
** 1. Affiche "exit" si shell interactif (isatty verifie le terminal)
** 2. Valide l'argument numerique et gere les erreurs (trop d'args)
** 3. Utilise l'argument fourni ou dernier exit_status comme code
** 4. Nettoie la memoire et l'historique avant exit() definitif
** Masque le code sur 8 bits (0xFF) selon la convention Unix.
*/
int	builtin_exit(t_cmd *cmd, t_shell *shell)
{
	int	exit_code;

	exit_code = 0;
	if (isatty(STDIN_FILENO))
		ft_putendl_fd("exit", 1);
	if (cmd->arg[1])
	{
		if (!is_number(cmd->arg[1]))
			exit_code = print_error_exit(cmd);
		else if (cmd->arg[2])
		{
			ft_putendl_fd("exit: too many arguments", 2);
			return (1);
		}
		else
			exit_code = ft_atoi(cmd->arg[1]);
	}
	else
		exit_code = shell->exit_status;
	dmb_free(shell);
	rl_clear_history();
	exit(exit_code & 0xFF);
}
