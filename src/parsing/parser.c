/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:10:02 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/11 01:09:41 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*add_ast_node(t_token *tokens, t_token *current, t_ast_type type)
{
	t_token		*before_tokens;
	t_token		*after_tokens;
	t_ast		*node;

	node = init_ast_node(type);
	if (!node)
		return (NULL);
	before_tokens = get_tokens_before(tokens, current);
	after_tokens = get_tokens_after(current);
	node->left = parse_tokens(before_tokens);
	node->right = parse_tokens(after_tokens);
	free_tokens(before_tokens);
	free_tokens(after_tokens);
	if (!node->right || !node->left)
		return (free_ast(node), NULL);
	return (node);
}

t_ast	*parse_command(t_token *tokens)
{
	t_token		*current;
	t_ast		*cmd_node;

	cmd_node = init_ast_node(AST_CMD);
	if (!cmd_node)
		return (NULL);
	cmd_node->cmd = create_command(tokens);
	if (!cmd_node->cmd)
		return (free_ast(cmd_node), NULL);
	current = tokens;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (!add_redirection_to_cmd(cmd_node->cmd, current))
				return (free_ast(cmd_node), NULL);
			current = current->next->next;
			continue ;
		}
		current = current->next;
	}
	return (cmd_node);
}

t_ast	*parse_pipes(t_token *tokens)
{
	t_token		*current;

	current = find_last_token_of_type(tokens, T_PIPE);
	if (current)
		return (add_ast_node(tokens, current, AST_PIPE));
	return (parse_command(tokens));
}

t_ast	*parse_logical_ops(t_token *tokens)
{
	t_token		*or;
	t_token		*and;

	or = find_last_token_of_type(tokens, T_OR);
	if (!or)
	{
		and = find_last_token_of_type(tokens, T_AND);
		if (!and)
			return (parse_pipes(tokens));
		return (add_ast_node(tokens, and, AST_AND));
	}
	else
	{
		and = find_last_token_of_type(or, T_AND);
		if (and)
			return (add_ast_node(tokens, and, AST_AND));
		else
			return (add_ast_node(tokens, or, AST_OR));
	}
}

t_ast	*parse_parenthesis(t_token *tokens)
{
	t_token		*content;
	t_token		*last_found;
	t_token		*opening;
	t_ast		*node;

	last_found = find_last_priority_operator(tokens);
	if (last_found)
		return (add_ast_node(tokens, last_found, get_ast_type(last_found)));
	opening = find_opening_par(tokens);
	if (!opening)
		return (NULL);
	content = extract_parenthesis_content(opening);
	if (!content)
		return (NULL);
	node = parse_tokens(content);
	node = init_ast_node(AST_SUB_SHELL);
	if (!node)
		return (free_tokens(content), NULL);
	node->sub_shell = parse_tokens(content);
	if (!node->sub_shell)
		return (free_tokens(content), free_ast(node), NULL);
	free_tokens(content);
	return (node);
}

t_ast	*parse_tokens(t_token *tokens)
{
	t_ast		*ast;

	if (!tokens)
		return (NULL);
	if (!verify_parenthesis(tokens))
		ast = parse_logical_ops(tokens);
	else
		ast = parse_parenthesis(tokens);
	if (!ast)
		return (free(ast), NULL);
	return (ast);
}
