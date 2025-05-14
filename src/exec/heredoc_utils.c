/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:19:15 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:19:41 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	chain_commands(t_ast *ast, t_cmd **first_cmd, t_cmd **last_cmd)
{
	if (!ast)
		return ;
	if (ast->type == AST_CMD)
	{
		if (*first_cmd == NULL)
			*first_cmd = ast->cmd;
		else
			(*last_cmd)->next = ast->cmd;
		*last_cmd = ast->cmd;
	}
	else if (ast->type == AST_SUB_SHELL)
		chain_commands(ast->sub_shell, first_cmd, last_cmd);
	else
	{
		chain_commands(ast->left, first_cmd, last_cmd);
		chain_commands(ast->right, first_cmd, last_cmd);
	}
}

t_cmd	*get_chained_commands(t_ast *ast)
{
	t_cmd	*first_cmd;
	t_cmd	*last_cmd;

	first_cmd = NULL;
	last_cmd = NULL;
	chain_commands(ast, &first_cmd, &last_cmd);
	return (first_cmd);
}
