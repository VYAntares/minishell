/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:35:03 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/11 01:09:00 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*init_ast_node(t_ast_type type)
{
	t_ast		*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd = NULL;
	node->redir = NULL;
	node->left = NULL;
	node->right = NULL;
	node->sub_shell = NULL;
	return (node);
}

t_token	*init_new_token(t_token *tmp, t_token *prev)
{
	t_token		*new_token;

	if (!tmp)
		return (NULL);
	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->type = tmp->type;
	new_token->value = ft_strdup(tmp->value);
	if (!new_token->value)
		return (free(new_token), NULL);
	new_token->type_word = tmp->type_word;
	new_token->next = NULL;
	new_token->prev = prev;
	return (new_token);
}

t_token	*find_last_token_of_type(t_token *tokens, t_type type)
{
	t_token		*current;
	t_token		*last_found;

	current = tokens;
	last_found = NULL;
	while (current)
	{
		if (current->type == type)
			last_found = current;
		current = current->next;
	}
	return (last_found);
}

t_token	*get_tokens_after(t_token *current)
{
	t_token		*new_list;
	t_token		*tail;
	t_token		*new;
	t_token		*tmp;

	if (!current || !current->next)
		return (NULL);
	tmp = current->next;
	tail = NULL;
	new_list = NULL;
	while (tmp)
	{
		new = init_new_token(tmp, tail);
		if (!new)
			return (free_tokens(new_list), NULL);
		if (!new_list)
			new_list = new;
		if (tail)
			tail->next = new;
		tail = new;
		tmp = tmp->next;
	}
	return (new_list);
}

t_token	*get_tokens_before(t_token *tokens, t_token *current)
{
	t_token		*new_list;
	t_token		*tail;
	t_token		*new;
	t_token		*tmp;

	if (!tokens || !current || tokens == current)
		return (NULL);
	new_list = NULL;
	tail = NULL;
	tmp = tokens;
	while (tmp && tmp != current)
	{
		new = init_new_token(tmp, tail);
		if (!new)
			return (free_tokens(new_list), NULL);
		if (!new_list)
			new_list = new;
		if (tail)
			tail->next = new;
		tail = new;
		tmp = tmp->next;
	}
	return (new_list);
}
