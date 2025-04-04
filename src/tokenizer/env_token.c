/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_token.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 22:36:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/04 01:16:49 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	handle_question_mark(t_token **list, int *i)
{
	t_token		*new;
	char		*value;

	new = malloc(sizeof(t_token));
	if (!new)
		return (0);
	value = ft_strdup("?");
	if (!value)
		return (free(new), 0);
	new->type = T_ENV_VAR;
	new->value = value;
	(*i)++;
	if (!add_node_back(list, new))
		return (free(new->value), free(new), 0);
	return (1);
}

int	handle_dollar(t_token **list)
{
	t_token		*new;
	char		*value;

	new = malloc(sizeof(t_token));
	if (!new)
		return (0);
	value = ft_strdup("$");
	if (!value)
		return (free(new), 0);
	new->type = T_WORD;
	new->value = value;
	if (!add_node_back(list, new))
		return (free(new->value), free(new), 0);
	return (1);
}

int	add_env_var_token(t_token **list, int *i, char *input)
{
	t_token		*new;
	char		*content;
	int			start;

	start = ++(*i);
	if (input[*i] == '?')
		return (handle_question_mark(list, i));
	while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
		(*i)++;
	if (start == *i)
		return (handle_dollar(list));
	content = ft_substr(input, start, *i - start);
	if (!content)
		return (0);
	new = malloc(sizeof(t_token));
	if (!new)
		return (free(content), 0);
	new->type = T_ENV_VAR;
	new->value = content;
	if (!add_node_back(list, new))
		return (free(new->value), free(new), 0);
	return (1);
}
