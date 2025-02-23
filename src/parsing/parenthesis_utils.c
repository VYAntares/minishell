/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenthesis_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 20:50:43 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 17:52:57 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	good_position_parenth(t_token *tmp)
{
	if (tmp->prev)
	{
		if (tmp->prev->type != T_OR && tmp->prev->type != T_AND
			&& tmp->prev->type != T_PIPE && tmp->prev->type != T_PARENTH_OPEN)
			return (0);
	}
	return (1);
}

int	verify_parenthesis(t_token *tokens)
{
	t_token		*tmp;
	int			i;
	int			count;

	i = 0;
	count = 0;
	tmp = tokens;
	while (tmp)
	{
		if (tmp->type == T_PARENTH_OPEN)
		{
			if (!good_position_parenth(tmp))
				return (PARENTH_AFTER_WRONG_TOKEN);
			i++;
		}
		else if (tmp->type == T_PARENTH_CLOSE)
		{
			if (i == 0)
				return (STARTING_WITH_CLOSE_PARENTH);
			i--;
			count++;
		}
		tmp = tmp->next;
	}
	if (i == 0)
		return (count);
	return (UNMATCHING_PARENTH);
}

t_token	*find_matching_closing_par(t_token *opening)
{
	t_token		*tmp;
	int			count;

	if (!opening || opening->type != T_PARENTH_OPEN)
		return (NULL);
	count = 1;
	tmp = opening->next;
	while (tmp && count > 0)
	{
		if (tmp->type == T_PARENTH_OPEN)
			count++;
		else if (tmp->type == T_PARENTH_CLOSE)
			count--;
		if (count == 0)
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

t_token	*find_matching_opening_par(t_token *closing)
{
	t_token		*tmp;
	int			count;

	if (!closing || closing->type != T_PARENTH_CLOSE)
		return (NULL);
	count = 1;
	tmp = closing->prev;
	while (tmp && count > 0)
	{
		if (tmp->type == T_PARENTH_CLOSE)
			count++;
		else if (tmp->type == T_PARENTH_OPEN)
			count--;
		if (count == 0)
			return (tmp);
		tmp = tmp->prev;
	}
	return (NULL);
}

t_token	*find_opening_par(t_token *tokens)
{
	t_token		*opening;

	if (!tokens)
		return (NULL);
	opening = tokens;
	while (opening)
	{
		if (opening->type == T_PARENTH_OPEN)
			return (opening);
		opening = opening->next;
	}
	return (NULL);
}

t_token	*extract_parenthesis_content(t_token *start)
{
	t_token		*current;
	t_token		*closing;
	t_token		*tail;
	t_token		*new_list;
	t_token		*new_token;

	if (!start || start->type != T_PARENTH_OPEN)
		return (NULL);
	tail = NULL;
	new_list = NULL;
	current = start->next;
	closing = find_matching_closing_par(start);
	if (!closing)
		return (NULL);
	while (current != closing)
	{
		new_token = init_new_token(current, tail);
		if (!new_token)
			return (free_tokens(new_list), NULL);
		if (!add_node_back(&new_list, new_token))
			return (free_tokens(new_list), NULL);
		tail = new_token;
		current = current->next;
	}
	return (new_list);
}
