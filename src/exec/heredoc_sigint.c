/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_sigint.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 23:06:39 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/18 23:29:40 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	handle_heredoc_sigint(int sig)
{
	(void)sig;
	g_sigint_received = 1;
	printf("\n");
    rl_replace_line("", 0);
    rl_on_new_line();
}

int	setup_heredoc_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handle_heredoc_sigint;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	return (0);
}

int process_sig(int *sig_received, char *line, char *temp_file)
{
    if (*sig_received || !line)
	{
	    setup_signals();
		if (*sig_received)
		{
			unlink(temp_file);
			return (0);
		}
		return (0);
	}
    return (1);
}
