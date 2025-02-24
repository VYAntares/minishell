/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:45:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/24 03:56:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_redirection(t_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_HEREDOC || type == T_APPEND);
}

t_token	*skip_redirection(t_token *current)
{
	while (is_redirection(current->type) && current->next)
		current = current->next->next;
	return (current);
}

// int	add_redirection(t_ast *node, t_token *redir_token)
// {
// 	t_ast	*redir_node;
// 	t_ast	*last_redir;

// 	redir_node = malloc(sizeof(t_ast));
// 	if (!redir_node)
// 		return (0);
// 	redir_node->type = AST_REDIR;
// 	redir_node->redir = redir_token;
// 	redir_node->cmd = node->cmd;
// 	redir_node->left = NULL;
// 	redir_node->right = NULL;
// 	if (!node->left)
// 		node->left = redir_node;
// 	else
// 	{
// 		last_redir = node->left;
// 		while (last_redir->right)
// 			last_redir = last_redir->right;
// 		last_redir->right = redir_node;
// 	}
// 	return (1);
// }

// int	add_redirection_to_node(t_ast *node, t_token *current)
// {
// 	char	*new_file;

// 	if (!current->next)
// 		return (0);
// 	new_file = ft_strdup(current->next->value);
// 	if (!new_file)
// 		return (0);
// 	if (!add_redirection(node, current))
// 		return (free(new_file), 0);
// 	node->cmd->type_redirection = current->type;
// 	node->cmd->file_redirection = new_file;
// 	return (1);
// }

// modified version
t_file_redir	*create_redir(t_token *current)
{
	t_file_redir	*redir;

	redir = malloc(sizeof(t_file_redir));
	if (!redir)
		return (NULL);
	redir->type_redirection = current->type;
	redir->file_redirection = ft_strdup(current->next->value);
	if (!redir->file_redirection)
		return (NULL);
	redir->word_parts = current->next->type_word;
	redir->next = NULL;
	return (redir);
}

int	add_redirection_to_cmd(t_cmd *cmd, t_token *current)
{
	t_file_redir	*new;
	t_file_redir	*last;

	new = create_redir(current);
	if (!new)
		return (0);
	if (!cmd->type_redir)
		cmd->type_redir = new;
	else
	{
		last = cmd->type_redir;
		while (last->next)
			last = last->next;
		last->next = new;
	}
	return (1);
}
