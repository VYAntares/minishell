/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/07 18:05:17 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	execute_redirections(t_cmd *cmd, t_shell *shell)
{
	t_file_redir	*redir;
	int				fd;

	if (expand_redir(cmd, shell) != 0)
		return (1);
	redir = cmd->type_redir;
	while (redir)
	{
		if (redir->type_redirection == T_REDIR_OUT)
		{
			fd = open(redir->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (perror(redir->content), 1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type_redirection == T_REDIR_IN)
		{
			fd = open(redir->content, O_RDONLY);
			if (fd == -1)
				return (perror(redir->content), 1);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type_redirection == T_APPEND)
		{
			fd = open(redir->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (perror(redir->content), 1);
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type_redirection == T_HEREDOC)
		{
			if (handle_heredoc(redir->content, shell) != 0)
				return (1);
		}
		redir = redir->next;
	}
	return (0);
}

int	execute_pipe(t_ast *left, t_ast *right, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (pipe(pipefd) == -1)
		return (perror("pipe"), 1);
	
	pid1 = fork();
	if (pid1 == -1)
		return (perror("fork"), close(pipefd[0]), close(pipefd[1]), 1);
	
	if (pid1 == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		exit(execute_ast(left, shell));
	}
	
	pid2 = fork();
	if (pid2 == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(pid1, NULL, 0);
		return (perror("fork"), 1);
	}
	
	if (pid2 == 0)
	{
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		exit(execute_ast(right, shell));
	}
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	
	if ((status2 & 0x7f) == 0)
		return ((status2 & 0xff00) >> 8);
	return (1);
}