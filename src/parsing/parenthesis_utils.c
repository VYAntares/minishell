/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenthesis_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 20:50:43 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:12:08 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Trouve la parenthese fermante correspondant a une ouvrante donnee.
** Utilise un compteur pour gerer les parentheses imbriquees.
** Retourne le token ')' correspondant ou NULL si non trouve.
*/
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

/*
** Trouve la parenthese ouvrante correspondant a une fermante donnee.
** Parcourt vers l'arriere en comptant les niveaux d'imbrication.
** Retourne le token '(' correspondant ou NULL si non trouve.
*/
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

/*
** Trouve la premiere parenthese ouvrante dans une liste de tokens.
** Parcourt lineairement jusqu'a trouver un T_PARENTH_OPEN.
** Retourne le premier token '(' trouve ou NULL si aucun.
*/
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

/*
** Extrait le contenu entre une parenthese ouvrante et sa fermante.
** Cree une nouvelle liste avec tous les tokens internes.
** Retourne la liste du contenu ou NULL en cas d'erreur.
*/
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
			return (NULL);
		if (!add_node_back(&new_list, new_token))
			return (NULL);
		tail = new_token;
		current = current->next;
	}
	return (new_list);
}
