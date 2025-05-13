/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:45:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/21 14:24:51 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_command_argument(t_type type)
{
	return (type == T_WORD || type == T_ENV_VAR || type == T_WILDCARD);
}

int	is_redirection(t_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_HEREDOC || type == T_APPEND);
}

t_token	*skip_redirection(t_token *current)
{
	while (is_redirection(current->type) && current->next)
		current = current->next->next;
	return (current);
}

t_file_redir	*create_redir(t_token *current)
{
	t_file_redir	*redir;

	if (!current || !current->next)
	{
		ft_putstr_fd
		("minishell: syntax error near unexpected token `newline'\n", 2);
		return (NULL);
	}
	redir = malloc(sizeof(t_file_redir));
	if (!redir)
		return (NULL);
	redir->type_redirection = current->type;
	redir->content = ft_strdup(current->next->value);
	if (!redir->content)
		return (free(redir), NULL);
	redir->word_parts = current->next->type_word;
	redir->is_ambiguous = 0;
	redir->next = NULL;
	return (redir);
}

int	add_redirection_to_cmd(t_cmd *cmd, t_token *current)
{
	t_file_redir	*new;
	t_file_redir	*last;

	new = create_redir(current);
	if (!new)
		return (0);
	if (!cmd->type_redir)
		cmd->type_redir = new;
	else
	{ 
		last = cmd->type_redir;
		while (last->next)
			last = last->next;
		last->next = new;
	}
	return (1);
}
