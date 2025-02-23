/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:45:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 16:39:17 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_redirection(t_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_HEREDOC || type == T_APPEND);
}

int	add_redirection(t_ast *node, t_token *redir_token)
{
	t_ast	*redir_node;
	t_ast	*last_redir;

	redir_node = malloc(sizeof(t_ast));
	if (!redir_node)
		return (0);
	redir_node->type = AST_REDIR;
	redir_node->redir = redir_token;
	redir_node->cmd = node->cmd;
	redir_node->left = NULL;
	redir_node->right = NULL;
	if (!node->left)
		node->left = redir_node;
	else
	{
		last_redir = node->left;
		while (last_redir->right)
			last_redir = last_redir->right;
		last_redir->right = redir_node;
	}
	return (1);
}

int	add_redirection_to_node(t_ast *node, t_token *current)
{
	char	*new_file;

	if (!current->next)
		return (0);
	new_file = ft_strdup(current->next->value);
	if (!new_file)
		return (0);
	if (!add_redirection(node, current))
		return (free(new_file), 0);
	node->cmd->type_redirection = current->type;
	node->cmd->file_redirection = new_file;
	return (1);
}

t_token	*skip_redirection(t_token *current)
{
	if (is_redirection(current->type) && current->next)
		return (current->next->next);
	return (current);
}
