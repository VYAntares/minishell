/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:39:53 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 00:40:35 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

int	print_error_exit(t_cmd *cmd)
{
	ft_putstr_fd("exit: ", 2);
	ft_putstr_fd(cmd->arg[1], 2);
	ft_putendl_fd(": numeric argument required", 2);
	return (255);
}

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
