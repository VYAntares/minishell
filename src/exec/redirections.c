/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:28:27 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	only_redirection(t_cmd *cmd)
{
	t_file_redir	*redir;
	int				fd;

	redir = cmd->type_redir;
	while (redir)
	{
		if (redir->is_ambiguous)
			return (ambiguous_error(redir));
		if (redir->type_redirection == T_REDIR_IN
			|| redir->type_redirection == T_HEREDOC)
		{
			fd = open(redir->content, O_RDONLY);
			if (fd == -1)
				return (perror(redir->content), 1);
			close(fd);
		}
		else if (redir->type_redirection == T_REDIR_OUT)
			fd = open(redir->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
			fd = open(redir->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1)
			return (perror(redir->content), 1);
		redir = redir->next;
	}
	return (0);
}

int	process_outpout_redirection(t_file_redir *redir,
								t_file_redir **last_output)
{
	int	fd;

	if (redir->is_ambiguous)
		return (ambiguous_error(redir));
	if (redir->type_redirection == T_REDIR_OUT)
		fd = open(redir->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
		return (perror(redir->content), 1);
	if (redir == *last_output)
		dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	process_input_redirection(t_file_redir *redir, t_file_redir **last_input)
{
	int	fd;

	if (redir->is_ambiguous)
		return (ambiguous_error(redir));
	fd = open(redir->content, O_RDONLY);
	if (fd == -1)
		return (perror(redir->content), 1);
	if (redir == *last_input)
		dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

int	process_all_redirections(t_file_redir *redir,
							t_file_redir **last_input,
							t_file_redir **last_output)
{
	while (redir)
	{
		if ((redir->type_redirection == T_REDIR_OUT
				|| redir->type_redirection == T_APPEND))
		{
			if (process_outpout_redirection(redir, last_output) != 0)
				return (1);
		}
		else
		{
			if (process_input_redirection(redir, last_input) != 0)
				return (1);
		}
		redir = redir->next;
	}
	return (0);
}

int	execute_redirections(t_cmd *cmd, t_shell *shell)
{
	t_file_redir	*redir;
	t_file_redir	*last_input;
	t_file_redir	*last_output;

	last_input = NULL;
	last_output = NULL;
	if (expand_redir(cmd, shell) != 0)
		return (1);
	if (!cmd->name && cmd->type_redir)
		return (only_redirection(cmd));
	redir = cmd->type_redir;
	if (find_lasts_redirection(redir, &last_input, &last_output))
		return (1);
	redir = cmd->type_redir;
	if (process_all_redirections(redir, &last_input, &last_output) != 0)
		return (1);
	return (0);
}
