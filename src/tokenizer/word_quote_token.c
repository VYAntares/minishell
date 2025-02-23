/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word_quote_token.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 22:37:30 by eahmeti           #+#    #+#             */
/*   Updated: 2025/02/23 18:10:23 by eahmeti          ###   ########.fr       */
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

t_token_word	*define_word_quote_type(t_token_word **list, char *content,
									int *i, int *start)
{
	t_token_word	*new_word;
	char			quote_type;

	new_word = malloc(sizeof(t_token_word));
	if (!new_word)
		return (NULL);
	quote_type = content[*i];
	*start = ++(*i);
	while (content[*i] != quote_type)
		(*i)++;
	new_word->content = ft_substr(content, *start, (*i) - (*start));
	if (quote_type == '\'')
		new_word->type = T_S_QUOTE;
	else if (quote_type == '\"')
		new_word->type = T_D_QUOTE;
	if (!add_node_back_word(list, new_word))
		return (NULL);
	if (content[*i])
		(*i)++;
	return (new_word);
}

t_token_word	*define_word_no_quote(t_token_word **list, char *content,
									int	*i, int	*start)
{
	t_token_word	*new_word;

	new_word = malloc(sizeof(t_token_word));
	if (!new_word)
		return (NULL);
	*start = *i;
	while (!is_operator(content[*i]) && content[*i] != ' '
		&& content[*i] != '\t' && content[*i] != '$'
		&& content[*i] && content[*i] != '\'' && content[*i] != '\"')
		(*i)++;
	new_word->content = ft_substr(content, *start, (*i) - (*start));
	new_word->type = T_NO_QUOTE;
	if (!add_node_back_word(list, new_word))
		return (NULL);
	return (new_word);
}

void	analyze_quote_content(t_token_word **list, t_token *new,
								char *content)
{
	t_token_word	*new_word;
	int				i;
	int				start;

	i = 0;
	start = 0;
	while (content[i])
	{
		new_word = malloc(sizeof(t_token_word));
		if (!new_word)
			return ;
		new_word->next = NULL;
		if (content[i] == '\'' || content[i] == '\"')
			new_word = define_word_quote_type(list, content, &i, &start);
		else if (content[i])
			new_word = define_word_no_quote(list, content, &i, &start);
	}
	new->type_word = *list;
}	

char	*create_content_word(char *input, int *i)
{
	char	*content;
	char	quote_type;
	int		start;

	start = *i;
	while (!is_operator(input[*i]) && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i] != '$' && input[*i])
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			quote_type = input[*i];
			(*i)++;
			while (input[*i] && input[*i] != quote_type)
				(*i)++;
			if (input[*i] != quote_type)
				return (printf("gerer erreur no quote"), NULL);   // GERER MESSAGE DERREUR
		}
		(*i)++;
	}
	content = ft_substr(input, start, *i - start);
	if (!content)
		return (free(content), NULL);
	return (content);
}

int	add_word_token(t_token **list, int *i, char *input)
{
	char			*content;
	t_token			*new;
	t_token_word	*list_word;

	list_word = NULL;
	content = create_content_word(input, i);
	if (!content)
		return (0);
	new = malloc(sizeof(t_token));
	if (!new)
		return (0);
	new->type = T_WORD;
	new->value = content;
	new->type_word = NULL;
	if (!add_node_back(list, new))
		return (free(new->value), free(new), 0);
	analyze_quote_content(&list_word, new, content);
	return (1);
}
