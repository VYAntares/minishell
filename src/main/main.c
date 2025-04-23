/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/23 01:27:25 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

sig_atomic_t	g_sigint_received;

// Fonction pour lire une ligne depuis stdin en mode non-interactif
static char	*get_next_line(int fd)
{
	char	*line;
	char	buffer[10001];
	int		bytes_read;
	int		i;

	i = 0;
	while ((bytes_read = read(fd, &buffer[i], 1)) > 0 && buffer[i] != '\n')
		i++;
	if (bytes_read <= 0 && i == 0)
		return (NULL);
	buffer[i] = '\0';
	line = ft_strdup(buffer);
	return (line);
}

int	main(int ac, char **av, char **envp)
{
	t_shell		*shell;
	char		*input;
	t_token		*tokens;
	t_ast		*ast;
	int			is_interactive;

	(void)ac;
	(void)av;
	tokens = NULL;
	ast = NULL;
	
	// Initialisation du shell et des signaux
	shell = init_shell(envp);
	if (!shell)
		return (1);
	if (setup_signals() == -1)
		return (free_env_list(shell->env), free(shell), 1);
	
	// Vérifier si on est en mode interactif
	is_interactive = isatty(fileno(stdin));
	
	// Boucle principale
	while (1)
	{
		g_sigint_received = 0;
		
		// Lecture différente selon le mode
		if (is_interactive)
		{
			input = readline("miniHell$ ");
			// Gestion de EOF (Ctrl+D)
			if (!input)
			{
				printf("exit\n");
				break;
			}
		}
		else
		{
			// Mode non-interactif
			input = get_next_line(fileno(stdin));
			if (!input)
				break;
		}
		
		// Si l'entrée n'est pas vide
		if (*input)
		{
			// Ajouter à l'historique seulement en mode interactif
			if (is_interactive)
				add_history(input);
			
			// Nettoyer les structures précédentes
			if (tokens)
				free_tokens(tokens);
			if (ast)
				free_ast(ast);
			
			// Analyser l'entrée
			tokens = tokenize(input);

			if (tokens)
			{
				// Vérifier la syntaxe des parenthèses
				if (check_syntax_error_parenthesis(tokens))
				{
					// Parser et exécuter
					ast = parse_tokens(tokens);
					
					// Après avoir exécuté la commande en mode non-interactif
					if (ast)
					{
						launch_heredoc(ast, shell);
						shell->exit_status = execute_ast(ast, shell);
						cleanup_heredoc_files(shell->cmd);
						
					
						if (!is_interactive)
						{
							// Nettoyer les ressources
							if (tokens)
								free_tokens(tokens);
							if (ast)
								free_ast(ast);
							rl_clear_history();
							free_env_list(shell->env);
							
							// Sortir avec le code d'erreur approprié
							exit(shell->exit_status);
						}
					}
				}
			}
		}
		free(input);
	}
	
	// Nettoyage avant de quitter
	if (tokens)
		free_tokens(tokens);
	if (ast)
		free_ast(ast);
	rl_clear_history();
	free_env_list(shell->env);
	free(shell);
	
	return (shell->exit_status);
}
// void	free_shell(t_ast *ast, t_token *tokens, t_shell *shell)
// {
// 	if (tokens)
// 		free_tokens(tokens);
// 	if (ast)
// 		free_ast(ast);
// 	rl_clear_history();
// 	free_env_list(shell->env);
// 	free(shell);
// }

// void	execute_line(char *input, t_token *tokens, t_ast *ast, t_shell *shell)
// {
// 	add_history(input);
// 	if (tokens)
// 		free_tokens(tokens);
// 	if (ast)
// 		free_ast(ast);
// 	tokens = tokenize(input);
// 	if (tokens && check_syntax_error_parenthesis(tokens))
// 	{
// 		ast = parse_tokens(tokens);
// 		if (ast)
// 		{
// 			launch_heredoc(ast, shell);
// 			shell->exit_status = execute_ast(ast, shell);
// 			cleanup_heredoc_files(shell->cmd);
// 		}
// 	}
// }

// int	read_and_execute(t_token *tokens, t_ast *ast, t_shell *shell)
// {
// 	char	*input;

// 	g_sigint_received = 0;
// 	input = readline("miniHell$ ");
// 	if (!input)
// 	{
// 		printf("exit\n");
// 		return (1);
// 	}
// 	if (*input)
// 		execute_line(input, tokens, ast, shell);
// 	free(input);
// 	return (0);
// }

// int	main(int ac, char **av, char **envp)
// {
// 	t_shell		*shell;
// 	t_token		*tokens;
// 	t_ast		*ast;

// 	(void)ac;
// 	(void)av;
// 	tokens = NULL;
// 	ast = NULL;
// 	shell = init_shell(envp);
// 	if (!shell)
// 		return (1);
// 	if (setup_signals() == -1)
// 		return (free_env_list(shell->env), free(shell), 1);
// 	while (1)
// 	{
// 		if (read_and_execute(tokens, ast, shell))
// 			break ;
// 	}
// 	free_shell(ast, tokens, shell);
// 	return (0);
// }
