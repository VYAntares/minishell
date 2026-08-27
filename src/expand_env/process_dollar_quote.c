/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_dollar_quote.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:47:40 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:22:07 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Traite les dollars en fin de token non-quoté avant une quote
** Compte les dollars consécutifs en fin de chaîne
** Supprime le dernier dollar si le nombre est impair
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
int	process_dollar(t_token_word	*current)
{
	char			*new_content;
	int				len;
	int				dollar_count;
	int				i;

	len = ft_strlen(current->content);
	if (len > 0 && current->content[len - 1] == '$')
	{
		dollar_count = 0;
		i = len - 1;
		while (i >= 0 && current->content[i] == '$')
		{
			dollar_count++;
			i--;
		}
		if (dollar_count % 2 != 0)
		{
			new_content = ft_substr(current->content, 0, len - 1);
			if (!new_content)
				return (1);
			dmb_free(current->content);
			current->content = new_content;
		}
	}
	return (0);
}

/*
** Préprocesse les dollars dans les redirections
** Applique process_dollar aux tokens non-quotés suivis de quotes
** Parcourt la liste des parties de mots d'une redirection
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
int	prepro_dol_redir(t_token_word *word_parts)
{
	t_token_word	*current;

	current = word_parts;
	while (current && current->next)
	{
		if (current->content && current->type == T_NO_QUOTE && current->next
			&& (current->next->type == T_D_QUOTE
				|| current->next->type == T_S_QUOTE))
		{
			if (process_dollar(current) != 0)
				return (1);
		}
		current = current->next;
	}
	return (0);
}

/*
** Préprocesse les dollars dans les arguments de commande
** Applique process_dollar aux tokens non-quotés suivis de quotes
** Parcourt la liste des parties de mots d'un argument
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
int	preprocess_dollar_quotes(t_token_word **head)
{
	t_token_word	*current;

	current = *head;
	while (current && current->next)
	{
		if (current->content && current->type == T_NO_QUOTE && current->next
			&& (current->next->type == T_D_QUOTE
				|| current->next->type == T_S_QUOTE))
		{
			if (process_dollar(current) != 0)
				return (1);
		}
		current = current->next;
	}
	return (0);
}
