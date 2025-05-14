/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 03:05:06 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Fonction pour comparer si une chaîne correspond à un motif de wildcard
** Cette fonction gère récursivement les '*' qui peuvent correspondre à zéro
** ou plusieurs caractères
*/
int	match_wildcard(const char *wildcard_str, const char *str)
{
	if (*wildcard_str == '\0' && *str == '\0')
		return (1);
	if (*wildcard_str == '*')
		return (match_wildcard(wildcard_str + 1, str)
			|| (*str != '\0' && match_wildcard(wildcard_str, str + 1)));
	if (*wildcard_str == *str)
		return (match_wildcard(wildcard_str + 1, str + 1));
	return (0);
}

/*
** Fonction pour reconstruire le contenu d'un token_word 
** spécifiquement pour les wildcards
*/
char	*rebuild_token_word_for_wildcard(t_token_word *word_list)
{
	char			*result;
	char			*tmp;
	t_token_word	*current;

	if (!word_list)
		return (ft_strdup(""));
	result = ft_strdup(word_list->content);
	if (!result)
		return (NULL);
	current = word_list->next;
	while (current)
	{
		tmp = result;
		result = ft_strjoin(tmp, current->content);
		dmb_free(tmp);
		if (!result)
			return (NULL);
		current = current->next;
	}
	return (result);
}

