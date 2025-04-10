/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/10 23:42:04 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

sig_atomic_t	g_sigint_received;

int	main(int ac, char **av, char **envp)
{
	t_shell		*shell;
	char		*input;
	t_token		*tokens;
	t_ast		*ast;

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
	
	// Boucle principale
	while (1)
	{
		g_sigint_received = 0;
		input = readline("miniHell$ ");
		// input = readline("minishell$ ");
		
		// Gestion de EOF (Ctrl+D)
		if (!input)
		{
			printf("exit\n");
			break;
		}
		
		// Si l'entrée n'est pas vide
		if (*input)
		{
			// Ajouter à l'historique
			add_history(input);
			
			// Nettoyer les structures précédentes
			if (tokens)
				free_tokens(tokens);
			if (ast)
				free_ast(ast);
			
			// Analyser l'entrée
			tokens = tokenize(input);

			// debug_print_tokens(tokens);
			if (tokens)
			{
				// Expansions des variables d'environnement
				// expand_env_vars(tokens, shell);
				
				// Vérifier la syntaxe des parenthèses
				if (check_syntax_error_parenthesis(tokens))
				{
					// Parser et exécuter
					ast = parse_tokens(tokens);
					// if (ast)
					// {
					// 	printf("\n\n\n\n\n=== DÉBUT DE L'AST ===\n%s\n%s\n%s\n", TREE_VERTICAL, TREE_VERTICAL, TREE_VERTICAL);
					// 	debug_print_ast(ast, "", true);
					// 	printf("=== FIN DE L'AST ===\n\n\n\n\n");
					// }
					
					if (ast)
					{
						launch_heredoc(ast, shell);
						shell->exit_status = execute_ast(ast, shell);
						cleanup_heredoc_files(shell->cmd);
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
	
	return (0);
}
