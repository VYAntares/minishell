/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:27:25 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:28:42 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	find_lasts_redirection(t_file_redir *redir,
							t_file_redir **last_input,
							t_file_redir **last_output)
{
	while (redir)
	{
		if (redir->type_redirection == T_REDIR_IN
			|| redir->type_redirection == T_HEREDOC)
			*last_input = redir;
		else if (redir->type_redirection == T_REDIR_OUT
			|| redir->type_redirection == T_APPEND)
			*last_output = redir;
		redir = redir->next;
	}
	return (0);
}

int	ambiguous_error(t_file_redir *redir)
{
	t_token_word	*word;

	word = redir->word_parts;
	while (word)
	{
		if (word->type == T_NO_QUOTE && ft_strchr(word->content, '$'))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(word->content, 2);
			ft_putendl_fd(": ambiguous redirect", 2);
			return (1);
		}
		word = word->next;
	}
	ft_putendl_fd("minishell: ambiguous redirect", 2);
	return (1);
}
