/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   word_quote_token.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 22:37:30 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:13:15 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Crée un token_word pour du contenu entre guillemets (simple ou double)
** Extrait le contenu entre les guillemets et définit le type approprié
** Ajoute le nouveau token à la liste et avance l'index
** Retourne le nouveau token créé ou NULL en cas d'erreur
*/
t_token_word	*define_word_quote_type(t_token_word **list, char *content,
									int *i, int *start)
{
	t_token_word	*new_word;
	char			quote_type;

	new_word = dmb_malloc(sizeof(t_token_word));
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

/*
** Crée un token_word pour du contenu sans guillemets
** Extrait le contenu jusqu'au prochain délimiteur (opérateur, espace, etc.)
** Définit le type comme T_NO_QUOTE et ajoute à la liste
** Retourne le nouveau token créé ou NULL en cas d'erreur
*/
t_token_word	*define_word_no_quote(t_token_word **list, char *content,
									int	*i, int	*start)
{
	t_token_word	*new_word;

	new_word = dmb_malloc(sizeof(t_token_word));
	if (!new_word)
		return (NULL);
	*start = *i;
	while (!is_operator(content[*i]) && content[*i] != ' '
		&& content[*i] != '\t' && content[*i] && content[*i] != '\''
		&& content[*i] != '\"')
		(*i)++;
	new_word->content = ft_substr(content, *start, (*i) - (*start));
	new_word->type = T_NO_QUOTE;
	if (!add_node_back_word(list, new_word))
		return (NULL);
	return (new_word);
}

/*
** Analyse le contenu d'un token pour identifier les parties quotées
** Parcourt le contenu et crée des token_word selon le type de quote
** Assigne la liste de token_word au token principal
** Gère les transitions entre contenu quoté et non-quoté
*/
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
		new_word = dmb_malloc(sizeof(t_token_word));
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

/*
** Extrait le contenu complet d'un mot depuis l'input
** Gère les guillemets ouverts/fermés et vérifie leur cohérence
** S'arrête aux délimiteurs (opérateurs, espaces)
** Retourne le contenu extrait ou NULL si erreur de syntaxe
*/
char	*create_content_word(char *input, int *i)
{
	char	*content;
	char	quote_type;
	int		start;

	start = *i;
	while (!is_operator(input[*i]) && input[*i] != ' ' && input[*i] != '\t'
		&& input[*i])
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			quote_type = input[*i];
			(*i)++;
			while (input[*i] && input[*i] != quote_type)
				(*i)++;
			if (input[*i] != quote_type)
				return (ft_putstr_fd
					("minishell: syntax error: unclosed\n", 2), NULL);
		}
		(*i)++;
	}
	content = ft_substr(input, start, *i - start);
	if (!content)
		return (dmb_free(content), NULL);
	return (content);
}

/*
** Crée et ajoute un token de type T_WORD à la liste de tokens
** Extrait le contenu, crée le token et analyse ses parties quotées
** Initialise tous les champs du token et l'ajoute à la liste
** Retourne 1 en cas de succès, 0 en cas d'erreur
*/
int	add_word_token(t_token **list, int *i, char *input)
{
	char			*content;
	t_token			*new;
	t_token_word	*list_word;

	list_word = NULL;
	content = create_content_word(input, i);
	if (!content)
		return (0);
	new = dmb_malloc(sizeof(t_token));
	if (!new)
		return (0);
	new->type = T_WORD;
	new->value = content;
	new->type_word = NULL;
	if (!add_node_back(list, new))
		return (dmb_free(new->value), dmb_free(new), 0);
	analyze_quote_content(&list_word, new, content);
	return (1);
}
