/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/21 14:20:59 by eahmeti          ###   ########.fr       */
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
						
						// Ajoute cette condition pour quitter immédiatement en mode non-interactif
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
							exit(shell->exit_status);  // Important : utilise exit() pour quitter immédiatement
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

// int	main(int ac, char **av, char **envp)
// {
// 	t_shell		*shell;
// 	char		*input;
// 	t_token		*tokens;
// 	t_ast		*ast;

// 	(void)ac;
// 	(void)av;
// 	tokens = NULL;
// 	ast = NULL;
	
// 	// Initialisation du shell et des signaux
// 	shell = init_shell(envp);
// 	if (!shell)
// 		return (1);
// 	if (setup_signals() == -1)
// 		return (free_env_list(shell->env), free(shell), 1);
	
// 	// Boucle principale
// 	while (1)
// 	{
// 		g_sigint_received = 0;
// 		input = readline("miniHell$ ");
// 		// input = readline("minishell$ ");
		
// 		// Gestion de EOF (Ctrl+D)
// 		if (!input)
// 		{
// 			printf("exit\n");
// 			break;
// 		}
		
// 		// Si l'entrée n'est pas vide
// 		if (*input)
// 		{
// 			// Ajouter à l'historique
// 			add_history(input);
			
// 			// Nettoyer les structures précédentes
// 			if (tokens)
// 				free_tokens(tokens);
// 			if (ast)
// 				free_ast(ast);
			
// 			// Analyser l'entrée
// 			tokens = tokenize(input);

// 			// debug_print_tokens(tokens);
// 			if (tokens)
// 			{
// 				// Expansions des variables d'environnement
// 				// expand_env_vars(tokens, shell);
				
// 				// Vérifier la syntaxe des parenthèses
// 				if (check_syntax_error_parenthesis(tokens))
// 				{
// 					// Parser et exécuter
// 					ast = parse_tokens(tokens);
// 					// if (ast)
// 					// {
// 					// 	printf("\n\n\n\n\n=== DÉBUT DE L'AST ===\n%s\n%s\n%s\n", TREE_VERTICAL, TREE_VERTICAL, TREE_VERTICAL);
// 					// 	debug_print_ast(ast, "", true);
// 					// 	printf("=== FIN DE L'AST ===\n\n\n\n\n");
// 					// }
					
// 					if (ast)
// 					{
// 						launch_heredoc(ast, shell);
// 						shell->exit_status = execute_ast(ast, shell);
// 						cleanup_heredoc_files(shell->cmd);
// 					}
// 				}
// 			}
// 		}
// 		free(input);
// 	}
	
// 	// Nettoyage avant de quitter
// 	if (tokens)
// 		free_tokens(tokens);
// 	if (ast)
// 		free_ast(ast);
// 	rl_clear_history();
// 	free_env_list(shell->env);
// 	free(shell);
	
// 	return (0);
// }
