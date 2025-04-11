/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenthesis_utils2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 01:04:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/11 01:10:52 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

int	check_syntax_error_parenthesis(t_token *tokens)
{
	int		result;

	result = verify_parenthesis(tokens);
	if (result == STARTING_WITH_CLOSE_PARENTH)
		return (ft_putstr_fd
			("miniHell: syntax error near unexpected token `)'\n", 2), 0);
	else if (result == UNMATCHING_PARENTH)
		return (ft_putstr_fd
			("miniHell: syntax error: unexpected end of file\n", 2), 0);
	else if (result == PARENTH_AFTER_WRONG_TOKEN)
		return (ft_putstr_fd
			("miniHell: syntax error near unexpected token `('\n", 2), 0);
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
