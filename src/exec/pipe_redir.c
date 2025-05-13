/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_redir.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:59:14 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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

int	only_redirection(t_cmd *cmd)
{
	t_file_redir	*redir;
	int				fd;

	// printf("enter in only_redir\n");
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

void	execute_left_command_in_pipe(int pipefd[2],
									t_ast *left,
									t_shell *shell)
{
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	exit(execute_ast(left, shell));
}

int	handle_right_process_fork_error(int pipefd[2], int pid1)
{
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	return (perror("fork"), 1);
}

void	execute_right_command_in_pipe(int pipefd[2],
									t_ast *right,
									t_shell *shell)
{
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	exit(execute_ast(right, shell));
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
		execute_left_command_in_pipe(pipefd, left, shell);
	pid2 = fork();
	if (pid2 == -1)
		return (handle_right_process_fork_error(pipefd, pid1));
	if (pid2 == 0)
		execute_right_command_in_pipe(pipefd, right, shell);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if ((status2 & 0x7f) == 0)
		return ((status2 & 0xff00) >> 8);
	return (1);
}
