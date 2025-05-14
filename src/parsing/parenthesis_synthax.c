/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenthesis_verif.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:23:24 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:24:43 by eahmeti          ###   ########.fr       */
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
		if (tmp->type == T_PARENTH_OPEN && ++i)
		{
			if (!good_position_parenth(tmp))
				return (PARENTH_AFTER_WRONG_TOKEN);
		}
		else if (tmp->type == T_PARENTH_CLOSE && ++count)
		{
			if (i == 0)
				return (STARTING_WITH_CLOSE_PARENTH);
			i--;
		}
		tmp = tmp->next;
	}
	if (i == 0)
		return (count);
	return (UNMATCHING_PARENTH);
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
