/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:37:11 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 20:57:26 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Ajoute un noeud t_token_word a la fin d'une liste chainee.
** Utilise pour construire les listes de parties de mots (quotes, etc.).
** Retourne 1 en cas de succes, 0 en cas d'erreur.
*/
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

/*
** Determine si un caractere est un operateur du shell.
** Reconnait les redirections (<, >), pipes (|), parentheses et &&.
** Retourne 1 si c'est un operateur, 0 sinon.
*/
int	is_operator(char input)
{
	return (input == '>' || input == '<' || input == '|'
		|| input == '(' || input == '&' || input == ')');
}

/*
** Ajoute un token a la fin de la liste chainee doublement liee.
** Configure les pointeurs next et prev pour maintenir la structure.
** Retourne 1 en cas de succes, 0 en cas d'erreur.
*/
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

/*
** Traite un caractere de l'input et cree le token approprie.
** Ignore les espaces, detecte les operateurs et traite les mots.
** Retourne 1 si succes, 0 en cas d'erreur syntaxique.
*/
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

/*
** Point d'entree de la tokenisation : decoupe l'input en tokens.
** Parcourt la chaine caractere par caractere et construit la liste.
** Retourne la liste de tokens ou NULL en cas d'erreur.
*/
t_token	*tokenize(char *input)
{
	t_token		*tokens;
	int			i;

	i = 0;
	tokens = NULL;
	while (input[i])
	{
		if (!process_token(&tokens, &i, input))
			return (NULL);
	}
	return (tokens);
}
