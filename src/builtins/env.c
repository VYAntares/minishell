/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:41:27 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 00:41:47 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
