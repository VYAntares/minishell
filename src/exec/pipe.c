/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:25:48 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:28:52 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
