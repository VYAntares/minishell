/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:10:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 03:23:03 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*create_temp_file(void)
{
	static int	counter;
	char		*temp_file;
	char		*num_str;

	temp_file = NULL;
	num_str = ft_itoa(++counter);
	if (!num_str)
		return (NULL);
	temp_file = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	dmb_free(num_str);
	return (temp_file);
}

int	process_heredoc_lines(int fd, char *delimiter,
						t_shell *shell, char *temp_file)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = readline("> ");
		if (!line || (ft_strlen(line) == ft_strlen(delimiter)
				&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0))
		{
			if (line)
				free(line);
			break ;
		}
		expanded_line = expand_env_heredoc(line, shell);
		if (!expanded_line)
			return (unlink(temp_file), free(line), 1);
		ft_putendl_fd(expanded_line, fd);
		free(line);
	}
	return (0);
}

int	handle_heredoc(t_file_redir *redir, t_shell *shell, char *delimiter)
{
	char	*temp_file;
	int		fd;
	int		result;

	temp_file = create_temp_file();
	if (!temp_file)
		return (1);
	fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (dmb_free(temp_file), 1);
	dmb_free(redir->content);
	redir->content = ft_strdup(temp_file);
	result = process_heredoc_lines(fd, delimiter, shell, temp_file);
	close(fd);
	dmb_free(delimiter);
	return (result);
}

char	*purify_quote(t_file_redir	*redir)
{
	t_token_word	*parts;
	char			*delimiter;

	delimiter = ft_strdup("");
	parts = redir->word_parts;
	while (parts)
	{
		delimiter = ft_strjoin(delimiter, parts->content);
		if (!delimiter)
			return (NULL);
		parts = parts->next;
	}
	return (delimiter);
}

void	launch_heredoc(t_ast *ast, t_shell *shell)
{
	t_file_redir	*redir;
	t_cmd			*cmd;

	shell->cmd = get_chained_commands(ast);
	redir = shell->cmd->type_redir;
	cmd = shell->cmd;
	while (cmd)
	{
		redir = cmd->type_redir;
		while (redir)
		{
			if (redir->type_redirection == T_HEREDOC)
			{
				if (handle_heredoc(redir,
						shell,
						purify_quote(redir)) != 0)
					return ;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
