/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:10:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/18 23:32:27 by eahmeti          ###   ########.fr       */
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

// int	process_heredoc_lines(int fd, char *delimiter,
// 						t_shell *shell, char *temp_file)
// {
// 	char	*line;
// 	char	*expanded_line;

// 	while (1)
// 	{
// 		line = readline("> ");
// 		if (!line || (ft_strlen(line) == ft_strlen(delimiter)
// 				&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0))
// 		{
// 			if (line)
// 				free(line);
// 			break ;
// 		}
// 		expanded_line = expand_env_heredoc(line, shell);
// 		if (!expanded_line)
// 			return (unlink(temp_file), free(line), 1);
// 		ft_putendl_fd(expanded_line, fd);
// 		free(line);
// 	}
// 	return (0);
// }

int process_heredoc_lines(int fd, char *delimiter, t_shell *shell)
{
    char    *line;
    char    *expanded_line;
    
    while (1)
    {
        line = readline("> ");
        if (!line) // EOF ou erreur
            return (0);
            
        if (ft_strlen(line) == ft_strlen(delimiter) && 
            ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
        {
            return (0);
        }
        
        expanded_line = expand_env_heredoc(line, shell);
        
        if (!expanded_line)
            return (1);
            
        ft_putendl_fd(expanded_line, fd);
    }
}

int handle_heredoc(t_file_redir *redir, t_shell *shell, char *delimiter)
{
    char    *temp_file;
    pid_t   pid;
    int     status;

    // Sauvegarder les gestionnaires de signaux actuels
    setup_signals_for_commands(); // Ignorer les signaux dans le parent pendant le heredoc
    
    temp_file = create_temp_file();
    if (!temp_file)
        return (1);
    
    pid = fork();
    if (pid == -1)
    {
        setup_signals(); // Restaurer les signaux
        return (dmb_free(temp_file), 1);
    }
    
    if (pid == 0) // Processus enfant
    {
        int fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
            exit(1);
        
        setup_heredoc_signals(); // Configurer les signaux spécifiques au heredoc
        
        if (process_heredoc_lines(fd, delimiter, shell) != 0)
        {
            // close(fd);
            exit(1);
        }
        
        // close(fd);
        exit(0);
    }
    else // Processus parent
    {
        waitpid(pid, &status, 0);
        
        // Si le processus enfant a été tué par un signal
        if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGINT)
			{
				g_sigint_received = 1;
				unlink(temp_file);
				dmb_free(temp_file);
				dmb_free(delimiter);
				
				// Restaurer les gestionnaires de signaux
				setup_signals();
				
				// Ne pas créer de contenu invalide qui sera utilisé
				// Laisser redir->content intact, il sera ignoré de toute façon
				
				return (1);
			}
		}
        
        // Vérifier le code de sortie du processus enfant
        if (WEXITSTATUS(status) != 0)
        {
            unlink(temp_file);
            dmb_free(temp_file);
            dmb_free(delimiter);
            setup_signals(); // Restaurer les signaux
            return (1);
        }
        
        // Tout s'est bien passé
        dmb_free(redir->content);
        redir->content = ft_strdup(temp_file);
        dmb_free(temp_file);
        dmb_free(delimiter);
        
        // IMPORTANT: Restaurer les gestionnaires de signaux
        setup_signals();
        
        return (0);
    }
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

void launch_heredoc(t_ast *ast, t_shell *shell)
{
    t_file_redir    *redir;
    t_cmd           *cmd;
    int             heredoc_interrupted = 0;

    g_sigint_received = 0; // Réinitialiser le flag global
    
    shell->cmd = get_chained_commands(ast);
    cmd = shell->cmd;
    
    while (cmd && !heredoc_interrupted)
    {
        redir = cmd->type_redir;
        while (redir && !heredoc_interrupted)
        {
            if (redir->type_redirection == T_HEREDOC)
            {
                if (handle_heredoc(redir, shell, purify_quote(redir)) != 0)
                {
                    if (g_sigint_received)
                    {
                        heredoc_interrupted = 1;
                        break;
                    }
                }
            }
            redir = redir->next;
        }
        if (heredoc_interrupted)
            break;
        cmd = cmd->next;
    }
    
    // Si un heredoc a été interrompu, on doit nettoyer
    if (heredoc_interrupted)
    {
        // Ce flag empêchera l'exécution de la commande
        g_sigint_received = 1;
    }
}
