/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:10:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/03 14:28:03 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*create_temp_file(void)
{
	static int	counter = 0;
	char		*temp_file;
	char		*num_str;

	num_str = ft_itoa(counter++);
	if (!num_str)
		return (NULL);
	temp_file = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	return (temp_file);
}

int	handle_heredoc(char *delimiter)
{
	char	*line;
	char	*temp_file;
	int		fd;

	temp_file = create_temp_file();
	if (!temp_file)
		return (1);
	
	fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (free(temp_file), 1);

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0)
		{
			free(line);
			break;
		}
		ft_putendl_fd(line, fd);
		free(line);
	}
	
	close(fd);
	
	fd = open(temp_file, O_RDONLY);
	if (fd == -1)
		return (free(temp_file), 1);
	
	dup2(fd, STDIN_FILENO);
	close(fd);
	
	unlink(temp_file);
	free(temp_file);
	
	return (0);
}