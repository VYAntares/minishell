/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 13:10:02 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/10 14:15:04 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

	// tester avec cette commande 
	// c1 && (c2 && (c3 || c4) && c5) && c6 || ((c7 || c8) && c9) && c10 || (c11 && (c12 || c13) && c14) && c15 || ((c16 || c17) && c18)

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

int	check_syntax_error_parenthesis(t_token *tokens)
{
	int		result;

	result = verify_parenthesis(tokens);
	if (result == STARTING_WITH_CLOSE_PARENTH)
		return (printf("bash: syntax error near unexcepted token `)'\n"), 0); // revoir erreur
	else if (result == UNMATCHING_PARENTH)
		return (printf("no matching parenthesis\n"), 0); // faut revoir
	else if (result == PARENTH_AFTER_WRONG_TOKEN)
		return (printf("bash: syntax error near unexcepted token `('\n"), 0); // faut revoir
	return (1);
}

int	check_if_operator_is_last(t_token *first_op, t_token *second_op)
{
	t_token		*tmp;
	int			valid;

	if (!first_op)
		return (0);
	tmp = first_op;
	valid = 1;
	while (tmp)
	{
		if (tmp == second_op)
		{
			valid = 0;
			break ;
		}
		tmp = tmp->next;
	}
	if (valid)
		return (1);
	return (0);
}

void	point_last_operator_outside_parenthesis(t_token **or,
											t_token **and,
											t_token **pipe,
											t_token *current)
{
	int		parenth_count;

	parenth_count = 0;
	while (current)
	{
		if (current->type == T_PARENTH_OPEN)
			parenth_count++;
		else if (current->type == T_PARENTH_CLOSE)
			parenth_count--;
		else if (parenth_count == 0)
		{
			if (current->type == T_OR)
				*or = current;
			else if (current->type == T_AND)
				*and = current;
			else if (current->type == T_PIPE)
				*pipe = current;
		}
		current = current->next;
	}
}

t_token	*find_last_priority_operator(t_token *tokens)
{
	t_token		*current;
	t_token		*or;
	t_token		*and;
	t_token		*pipe;

	if (!tokens)
		return (NULL);
	or = NULL;
	and = NULL;
	pipe = NULL;
	current = tokens;
	point_last_operator_outside_parenthesis(&or, &and, &pipe, current);
	if (check_if_operator_is_last(or, and))
		return (or);
	if (check_if_operator_is_last(and, or))
		return (and);
	return (pipe);
}

t_ast_type	get_ast_type(t_token *token)
{
	if (!token)
		return (AST_CMD);
	if (token->type == T_AND)
		return (AST_AND);
	if (token->type == T_OR)
		return (AST_OR);
	if (token->type == T_PIPE)
		return (AST_PIPE);
	return (AST_CMD);
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
