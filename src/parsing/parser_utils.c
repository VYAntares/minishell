/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:35:03 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:07:26 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Initialise un noeud de l'arbre syntaxique abstrait.
** Alloue la memoire et met tous les pointeurs a NULL.
** Retourne le noeud initialise ou NULL en cas d'erreur.
*/
t_ast	*init_ast_node(t_ast_type type)
{
	t_ast		*node;

	node = dmb_malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd = NULL;
	node->redir = NULL;
	node->left = NULL;
	node->right = NULL;
	node->sub_shell = NULL;
	return (node);
}

/*
** Cree une copie d'un token pour une nouvelle liste.
** Duplique le type, la valeur et configure les liaisons.
** Retourne le nouveau token ou NULL en cas d'erreur.
*/
t_token	*init_new_token(t_token *tmp, t_token *prev)
{
	t_token		*new_token;

	if (!tmp)
		return (NULL);
	new_token = dmb_malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->type = tmp->type;
	new_token->value = ft_strdup(tmp->value);
	if (!new_token->value)
		return (dmb_free(new_token), NULL);
	new_token->type_word = tmp->type_word;
	new_token->next = NULL;
	new_token->prev = prev;
	return (new_token);
}

/*
** Parcourt la liste pour trouver le dernier token d'un type donne.
** Utilise pour determiner quel operateur doit devenir racine de l'AST.
** Retourne le dernier token trouve ou NULL si aucun.
*/
t_token	*find_last_token_of_type(t_token *tokens, t_type type)
{
	t_token		*current;
	t_token		*last_found;

	current = tokens;
	last_found = NULL;
	while (current)
	{
		if (current->type == type)
			last_found = current;
		current = current->next;
	}
	return (last_found);
}

/*
** Cree une nouvelle liste avec tous les tokens apres un token donne.
** Utilise pour obtenir la partie droite lors du splitting.
** Retourne la nouvelle liste ou NULL si vide/erreur.
*/
t_token	*get_tokens_after(t_token *current)
{
	t_token		*new_list;
	t_token		*tail;
	t_token		*new;
	t_token		*tmp;

	if (!current || !current->next)
		return (NULL);
	tmp = current->next;
	tail = NULL;
	new_list = NULL;
	while (tmp)
	{
		new = init_new_token(tmp, tail);
		if (!new)
			return (NULL);
		if (!new_list)
			new_list = new;
		if (tail)
			tail->next = new;
		tail = new;
		tmp = tmp->next;
	}
	return (new_list);
}

/*
** Cree une nouvelle liste avec tous les tokens apres un token donne.
** Utilise pour obtenir la partie droite lors du splitting.
** Retourne la nouvelle liste ou NULL si vide/erreur.
*/
t_token	*get_tokens_before(t_token *tokens, t_token *current)
{
	t_token		*new_list;
	t_token		*tail;
	t_token		*new;
	t_token		*tmp;

	if (!tokens || !current || tokens == current)
		return (NULL);
	new_list = NULL;
	tail = NULL;
	tmp = tokens;
	while (tmp && tmp != current)
	{
		new = init_new_token(tmp, tail);
		if (!new)
			return (NULL);
		if (!new_list)
			new_list = new;
		if (tail)
			tail->next = new;
		tail = new;
		tmp = tmp->next;
	}
	return (new_list);
}
