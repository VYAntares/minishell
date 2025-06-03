/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:10:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:15:05 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Genere un nom unique de fichier temporaire pour heredoc.
** Utilise un compteur statique pour eviter les collisions.
** Retourne le chemin complet ou NULL en cas d'erreur.
*/
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

/*
** Lit les lignes utilisateur jusqu'au delimiteur de fin d'heredoc.
** Expanse les variables d'environnement et ecrit dans le fichier.
** Retourne 0 si succes, 1 en cas d'erreur d'expansion.
*/
int	process_heredoc_lines(int fd, char *delimiter, t_shell *shell)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		line = readline("> ");
		if (!line)
			return (0);
		if (ft_strlen(line) == ft_strlen(delimiter)
			&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
			return (0);
		expanded_line = expand_env_heredoc(line, shell);
		if (!expanded_line)
			return (1);
		ft_putendl_fd(expanded_line, fd);
	}
}

/*
** Orchestre l'execution complete d'un heredoc.
** Cree un processus enfant pour lire l'input et ecrit dans un fichier.
** Retourne 0 si succes, 130 si interruption (Ctrl+C).
*/
int	handle_heredoc(t_file_redir *redir, t_shell *shell, char *delimiter)
{
	char	*temp_file;
	pid_t	pid;
	int		status;
	int		fd;

	setup_signals_for_commands();
	temp_file = create_temp_file();
	if (!temp_file)
		return (1);
	fd = setup_heredoc_fork(temp_file, &pid);
	if (pid == 0)
		return (execute_heredoc_child(fd, delimiter, shell));
	waitpid(pid, &status, 0);
	if (WEXITSTATUS(status) != 0)
	{
		unlink(temp_file);
		setup_signals();
		return (130);
	}
	redir->content = ft_strdup(temp_file);
	setup_signals();
	return (0);
}

/*
** Reconstruit le delimiteur d'heredoc en concatenant ses parties.
** Assemble les token_word pour obtenir le delimiteur final.
** Retourne la chaine complete ou NULL en cas d'erreur.
*/
char	*purify_q(t_file_redir	*redir)
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

/*
** Point d'entree pour traiter tous les heredocs d'un AST.
** Parcourt toutes les commandes et leurs redirections.
** Retourne le statut final ou 130 en cas d'interruption.
*/
int	launch_heredoc(t_ast *ast, t_shell *shell)
{
	t_file_redir	*redir;
	t_cmd			*cmd;
	int				result_heredoc;

	shell->cmd = get_chained_commands(ast);
	cmd = shell->cmd;
	while (cmd && result_heredoc != 130)
	{
		redir = cmd->type_redir;
		while (redir)
		{
			if (redir->type_redirection == T_HEREDOC)
			{
				result_heredoc = handle_heredoc(redir, shell, purify_q(redir));
				if (result_heredoc == 130)
				{
					shell->exit_status = 1;
					return (result_heredoc);
				}
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (result_heredoc);
}
