/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                              :+:    :+:           */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/06/03 16:17:42 by eahmeti        ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

sig_atomic_t	g_sigint_received;

/*
** Nettoie les fichiers temporaires crees par les heredocs.
** Parcourt toutes les commandes et leurs redirections pour supprimer
** les fichiers temporaires generes par les heredocs (<<).
*/
void	cleanup_heredoc_files(t_cmd *cmd)
{
	t_file_redir	*redir;
	t_cmd			*current;

	redir = cmd->type_redir;
	current = cmd;
	while (current)
	{
		while (redir)
		{
			if (redir->type_redirection == T_HEREDOC)
				unlink(redir->content);
			redir = redir->next;
		}
		current = current->next;
	}
}

/*
** Lit une ligne utilisateur, la tokenise, la parse et l'execute.
** Gere le cycle complet : lecture -> analyse syntaxique -> execution.
** Retourne 1 pour quitter le shell (EOF), 0 pour continuer.
*/
int	read_and_execute(t_token *tokens, t_ast *ast, t_shell *shell)
{
	char	*input;
	int		result_heredoc;

	input = readline("miniHell$ ");
	if (!input)
		return (printf("exit\n"), 1);
	if (*input)
	{
		add_history(input);
		tokens = tokenize(input);
		if (tokens && check_syntax_error_parenthesis(tokens))
		{
			ast = parse_tokens(tokens);
			if (ast)
			{
				result_heredoc = launch_heredoc(ast, shell);
				if (result_heredoc != 130)
				{
					shell->exit_status = execute_ast(ast, shell);
					cleanup_heredoc_files(shell->cmd);
				}
			}
		}
	}
	return (0);
}

/*
** Point d'entree du programme.
** Initialise le shell, configure les signaux et lance la boucle
** principale qui lit et execute les commandes utilisateur.
*/
int	main(int ac, char **av, char **envp)
{
	t_shell		*shell;
	t_token		*tokens;
	t_ast		*ast;

	(void)ac;
	(void)av;
	tokens = NULL;
	ast = NULL;
	shell = init_shell(envp);
	if (!shell)
		return (1);
	if (setup_signals() == -1)
		return (dmb_free(shell), 1);
	while (1)
	{
		if (read_and_execute(tokens, ast, shell))
			break ;
	}
	rl_clear_history();
	dmb_gc();
	return (shell->exit_status);
}
