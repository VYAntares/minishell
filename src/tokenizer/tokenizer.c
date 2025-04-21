/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:37:11 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/21 14:11:57 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	add_node_back_word(t_token_word **list, t_token_word *new)
{
	t_token_word	*last;

	if (!list || !new)
		return (0);
	new->next = NULL;
	if (!*list)
	{
		*list = new;
		return (1);
	}
	last = *list;
	while (last->next)
		last = last->next;
	last->next = new;
	return (1);
}

int	is_operator(char input)
{
	return (input == '>' || input == '<' || input == '|'
		|| input == '(' || input == '&' || input == ')');
}

int	add_node_back(t_token **list, t_token *new)
{
	t_token		*last;

	if (!list || !new)
		return (0);
	new->next = NULL;
	if (!*list)
	{
		new->prev = NULL;
		*list = new;
		return (1);
	}
	last = *list;
	while (last->next)
		last = last->next;
	last->next = new;
	new->prev = last;
	return (1);
}

int	process_token(t_token **tokens, int *i, char *input)
{
	if (input[*i] == ' ' || input[*i] == '\t')
		return ((*i)++, 1);
	else if (is_operator(input[*i]))
	{
		if (input[*i] == '&' && input[*i + 1] != '&')
			return (ft_putstr_fd
				("minishell: syntax error near unexpected token `&'\n", 2), 0);
		return (add_operator_token(tokens, i, input));
	}
	return (add_word_token(tokens, i, input));
}

t_token	*tokenize(char *input)
{
	t_token		*tokens;
	int			i;

	i = 0;
	tokens = NULL;
	while (input[i])
	{
		if (!process_token(&tokens, &i, input))
			return (free_tokens(tokens), NULL);
	}
	return (tokens);
}
