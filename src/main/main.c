/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 21:30:32 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/19 23:41:55 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

sig_atomic_t	g_sigint_received;

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

// void execute_line(char *input, t_token *tokens, t_ast *ast, t_shell *shell)
// {
//     add_history(input);
//     tokens = tokenize(input);
//     if (tokens && check_syntax_error_parenthesis(tokens))
//     {
//         ast = parse_tokens(tokens);
//         if (ast)
//         {
//             launch_heredoc(ast, shell);
            
//             // Ne pas exécuter la commande si un heredoc a été interrompu
//             if (!g_sigint_received)
//             {
//                 shell->exit_status = execute_ast(ast, shell);
//                 cleanup_heredoc_files(shell->cmd);
//             }
//             else
//             {
//                 // Nettoyer explicitement tous les fichiers temporaires
//                 if (shell->cmd)
//                     cleanup_heredoc_files(shell->cmd);
//                 g_sigint_received = 0; // Réinitialiser pour le prochain prompt
//             }
//         }
//     }
// }

int read_and_execute(t_token *tokens, t_ast *ast, t_shell *shell)
{
    char *input;

    g_sigint_received = 0;
    input = readline("miniHell$ ");
    if (!input)
    {
        printf("exit\n");
        return (1);
    }
    if (*input)
    {
        add_history(input);
        tokens = tokenize(input);
        if (tokens && check_syntax_error_parenthesis(tokens))
        {
            ast = parse_tokens(tokens);
            if (ast)
            {
                int result_heredoc = launch_heredoc(ast, shell);
                
                // Ne pas exécuter la commande si un heredoc a été interrompu
                if (result_heredoc != 130)
                {
                    shell->exit_status = execute_ast(ast, shell);
                    cleanup_heredoc_files(shell->cmd);
                }
            }
        }
    }
    free(input);
    return (0);
}

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
	return (0);
}
