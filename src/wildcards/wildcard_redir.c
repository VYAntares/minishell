/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:46:33 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:49:34 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*redirection_content(t_file_redir *redir)
{
	char			*new_content;
	char			*tmp;
	t_token_word	*word;

	new_content = ft_strdup("");
	if (!new_content)
		return (NULL);
	word = redir->word_parts;
	while (word)
	{
		tmp = ft_strjoin(new_content, word->content);
		dmb_free(new_content);
		if (!tmp)
			return (NULL);
		new_content = tmp;
		word = word->next;
	}
	return (new_content);
}

int	ambiguous_multiple_file_detected(t_file_redir *redir)
{
	ft_putstr_fd("miniHell: ", 2);
	ft_putstr_fd(redir->content, 2);
	ft_putendl_fd(": ambiguous redirect", 2);
	return (1);
}

/*
** Fonction pour reconstruire la chaîne de contenu après l'expansion wildcard
** pour les redirections
*/
int	rebuild_redirection_content(t_file_redir *redir)
{
	char			*new_content;
	t_token_word	*word;
	int				file_count;

	if (!redir || !redir->word_parts)
		return (0);
	file_count = 0;
	word = redir->word_parts;
	while (word)
	{
		file_count++;
		word = word->next;
	}
	if (file_count > 1 && redir->type_redirection != T_APPEND)
		return (ambiguous_multiple_file_detected(redir));
	else
		new_content = redirection_content(redir);
	if (!new_content)
		return (1);
	dmb_free(redir->content);
	redir->content = new_content;
	return (0);
}

int	expand_wildcard_in_redir(t_cmd *cmd, int expanded_something)
{
	t_file_redir	*redir;

	redir = cmd->type_redir;
	while (redir)
	{
		if (redir->word_parts && redir->type_redirection != T_HEREDOC)
		{
			if (expand_wildcard_in_word_list(&(redir->word_parts)) != 0)
				return (1);
			if (rebuild_redirection_content(redir) != 0)
				return (1);
			expanded_something = 1;
		}
		redir = redir->next;
	}
	if (expanded_something)
		return (rebuild_command_arg_wildcard(cmd));
	return (1);
}