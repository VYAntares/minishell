/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_wildcards.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:36:15 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:38:01 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Fonction récursive de matching de pattern avec wildcards
** Retourne 1 si le pattern correspond à la chaîne, 0 sinon
** Gère '*' comme correspondance de 0 ou plusieurs caractères
** Compare caractère par caractère pour les autres cas
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
** Vérifie si un token_word contient des wildcards expandables
** Retourne 0 si '*' trouvé dans une partie quotée (pas d'expansion)
** Retourne 1 si '*' trouvé dans une partie non-quotée
** Parcourt toutes les parties du token_word
*/
int	has_wildcard_in_word(t_token_word *word_parts)
{
	t_token_word	*word;
	int				flag;

	flag = 0;
	word = word_parts;
	while (word)
	{
		if (word->type != T_NO_QUOTE && ft_strchr(word->content, '*'))
			return (0);
		if (word->type == T_NO_QUOTE && ft_strchr(word->content, '*'))
			flag = 1;
		word = word->next;
	}
	return (flag);
}

/*
** Vérifie si une commande contient des wildcards à expander
** Contrôle d'abord tous les arguments de la commande
** Puis vérifie les redirections si aucun wildcard dans les arguments
** Retourne 1 si au moins un wildcard trouvé, 0 sinon
*/
int	has_wildcard_in_cmd(t_cmd *cmd)
{
	int				has_wildcard;
	int				i;
	t_file_redir	*redir;

	i = 0;
	while (i < cmd->ac && cmd->list_word[i])
	{
		has_wildcard = has_wildcard_in_word(cmd->list_word[i]);
		if (has_wildcard)
			break ;
		i++;
	}
	if (!has_wildcard && cmd->type_redir)
	{
		redir = cmd->type_redir;
		while (redir && !has_wildcard)
		{
			has_wildcard = has_wildcard_in_word(redir->word_parts);
			redir = redir->next;
		}
	}
	return (has_wildcard);
}
