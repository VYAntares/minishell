/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:37:11 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 16:40:14 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
			return (printf("bash: syntax error near unexpected token `&'\n"), 0); // gerer ce cas la
		return (add_operator_token(tokens, i, input));
	}
	else if (input[*i] == '$')
		return (add_env_var_token(tokens, i, input));
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
