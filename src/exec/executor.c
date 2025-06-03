/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 17:49:55 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:23:35 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Execute une commande simple avec gestion complete des cas speciaux.
** 1. Verifie validite de la commande (nom, arguments non vides)
** 2. Expanse les variables ($HOME) et wildcards (*) dans les arguments
** 3. Si builtin (cd, echo, etc) execute directement dans le shell parent
** 4. Sinon fork un processus enfant pour executer la commande externe
** 5. Parent attend l'enfant et retourne son code de sortie
** Gere signaux, erreurs fork et nettoyage memoire automatiquement.
*/
int	execute_command(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd || !cmd->name || !cmd->arg[0])
		return (execute_redirections(cmd, shell), 1);
	if (expand_var(cmd, shell) != 0 || launch_expand_wildcards(cmd) != 0)
		return (1);
	if (is_builtin(cmd->name))
		return (handle_builtin(cmd, shell));
	setup_signals_for_commands();
	pid = fork();
	if (pid == -1)
	{
		setup_signals();
		return (perror("fork"), dmb_gc(), 1);
	}
	if (pid == 0)
	{
		handle_command(cmd, shell);
		dmb_gc();
	}
	status = handle_parent_process(pid, 0);
	setup_signals();
	return (status);
}

/*
** Execute un sous-shell isole pour les expressions entre parentheses.
** exemple: (cmd1 && cmd2) || cmd3 - les parentheses sont un subshell
** 1. Fork un processus enfant completement isole du parent
** 2. Enfant execute l'AST du contenu des parentheses recursivement
** 3. Parent attend et recupere le code de sortie du sous-shell
** Isolation complete: variables, signaux, etc. n'affectent pas le parent.
*/
int	execute_subshell(t_ast *sub_shell, t_shell *shell)
{
	pid_t	pid;
	int		status;
	int		exit_code;

	pid = fork();
	if (pid == 0)
	{
		exit_code = execute_ast(sub_shell, shell);
		dmb_gc();
		exit(exit_code);
	}
	if (pid < 0)
	{
		perror("minishell: fork");
		dmb_gc();
		return (1);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
			return (128 + WTERMSIG(status));
		else
			return (WEXITSTATUS(status));
	}
}

/*
** Point d'entree recursif pour executer tout type de noeud AST.
** Dispatche selon le type de noeud:
** - AST_CMD: execute une commande simple
** - AST_PIPE: execute un pipe (cmd1 | cmd2)
** - AST_AND: execute && avec logique court-circuit (droite si gauche OK)
** - AST_OR: execute || avec logique court-circuit (droite si gauche KO)
** - AST_SUB_SHELL: execute un sous-shell entre parentheses
** Retourne le code de sortie final selon la logique de chaque operateur.
*/
int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return (0);
	else if (ast->type == AST_CMD)
		return (execute_command(ast->cmd, shell));
	else if (ast->type == AST_PIPE)
		return (execute_pipe(ast->left, ast->right, shell));
	else if (ast->type == AST_AND)
	{
		if (execute_ast(ast->left, shell) == 0)
			return (execute_ast(ast->right, shell));
		return (1);
	}
	else if (ast->type == AST_OR)
	{
		if (execute_ast(ast->left, shell) != 0)
			return (execute_ast(ast->right, shell));
		return (0);
	}
	else if (ast->type == AST_SUB_SHELL)
		return (execute_subshell(ast->sub_shell, shell));
	return (1);
}
