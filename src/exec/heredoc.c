/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:10:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/11 02:27:47 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*create_temp_file(void)
{
	static int	counter;
	char		*temp_file;
	char		*num_str;

	temp_file = NULL;
	num_str = ft_itoa(++counter);
	if (!num_str)
		return (NULL);
	temp_file = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	return (temp_file);
}

int	handle_heredoc(t_file_redir *redir, t_shell *shell)
{
	char	*line;
	char	*temp_file;
	char	*expanded_line;
	char	*delimiter;
	int		fd;

	delimiter = ft_strdup(redir->content);
	temp_file = create_temp_file();
	if (!temp_file)
		return (1);
	fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (free(temp_file), 1);
	free(redir->content);
	redir->content = ft_strdup(temp_file);
	while (1)
	{
		line = readline("> ");
		if (!line || (ft_strlen(line) == ft_strlen(delimiter)
				&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0))
		{
			break ;
		}
		expanded_line = expand_env_heredoc(line, shell);
		if (!expanded_line)
		{
			close(fd);
			unlink(temp_file);
			return (1);
		}
		ft_putendl_fd(expanded_line, fd);
	}
	close(fd);
	return (0);
}

void	chain_commands(t_ast *ast, t_cmd **first_cmd, t_cmd **last_cmd)
{
	if (!ast)
		return ;
	if (ast->type == AST_CMD)
	{
		if (*first_cmd == NULL)
			*first_cmd = ast->cmd;
		else
			(*last_cmd)->next = ast->cmd;
		*last_cmd = ast->cmd;
	}
	else if (ast->type == AST_SUB_SHELL)
		chain_commands(ast->sub_shell, first_cmd, last_cmd);
	else
	{
		chain_commands(ast->left, first_cmd, last_cmd);
		chain_commands(ast->right, first_cmd, last_cmd);
	}
}

t_cmd	*get_chained_commands(t_ast *ast)
{
	t_cmd	*first_cmd;
	t_cmd	*last_cmd;

	first_cmd = NULL;
	last_cmd = NULL;
	chain_commands(ast, &first_cmd, &last_cmd);
	return (first_cmd);
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
				if (handle_heredoc(redir, shell) != 0)
					return ;
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
}
