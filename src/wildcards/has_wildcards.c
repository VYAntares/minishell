/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   has_wildcards.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:36:15 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 03:07:36 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
