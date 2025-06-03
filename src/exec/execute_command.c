/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:16:13 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:26:14 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Gere l'attente et le traitement du statut d'un processus enfant.
** 1. waitpid() bloque jusqu'a ce que l'enfant termine
** 2. Si termine par SIGINT (Ctrl+C), affiche newline pour l'affichage
** 3. Restaure les signaux du shell si is_minishell=1
** 4. Retourne le code de sortie ou 128+signal si termine par signal
** Convertit les statuts systeme en codes de sortie shell standard.
*/
int	handle_parent_process(pid_t pid, int is_minishell)
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	if (is_minishell)
		setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/*
** Determine si une commande est un builtin integre au shell.
** Compare le nom avec la liste des 7 builtins implementes:
** echo, cd, pwd, export, unset, env, exit
** Retourne 1 si c'est un builtin, 0 pour une commande externe.
** Les builtins s'executent dans le processus shell, pas en fork.
*/
int	is_builtin(char *cmd)
{
	return ((!ft_strncmp(cmd, "echo", 5))
		|| (!ft_strncmp(cmd, "cd", 3))
		|| (!ft_strncmp(cmd, "pwd", 4))
		|| (!ft_strncmp(cmd, "export", 7))
		|| (!ft_strncmp(cmd, "unset", 6))
		|| (!ft_strncmp(cmd, "env", 4))
		|| (!ft_strncmp(cmd, "exit", 5)));
}

/*
** Dispatche l'execution vers la bonne fonction builtin selon le nom.
** Chaque builtin a sa propre fonction d'implementation.
** Passe la structure shell pour les builtins qui modifient l'etat:
** - cd: change repertoire et variables PWD/OLDPWD
** - export/unset: modifient les variables d'environnement
** - exit: termine le shell avec nettoyage complet
*/
int	execute_builtin(t_cmd *cmd, t_shell *shell)
{
	if (!ft_strncmp(cmd->name, "echo", 4))
		return (builtin_echo(cmd));
	else if (!ft_strncmp(cmd->name, "cd", 2))
		return (builtin_cd(cmd, shell));
	else if (!ft_strncmp(cmd->name, "pwd", 3))
		return (builtin_pwd());
	else if (!ft_strncmp(cmd->name, "export", 6))
		return (builtin_export(cmd, shell));
	else if (!ft_strncmp(cmd->name, "unset", 5))
		return (builtin_unset(cmd, shell));
	else if (!ft_strncmp(cmd->name, "env", 3))
		return (builtin_env(shell));
	else if (!ft_strncmp(cmd->name, "exit", 4))
		return (builtin_exit(cmd, shell));
	return (1);
}

/*
** Execute un builtin en gerant ses redirections eventuelles.
** Probleme: builtins s'executent dans le shell parent, pas en fork
** Solution: sauvegarde stdin/stdout, applique redirections temporaires
** 1. Duplique les descripteurs originaux (stdin=0, stdout=1)
** 2. Applique les redirections (ex: echo hello > file)
** 3. Execute le builtin (qui ecrit dans le fichier)
** 4. Restaure les descripteurs originaux pour le shell
** Permet: cd, echo > file, export sans affecter le shell principal.
*/
int	handle_builtin(t_cmd *cmd, t_shell *shell)
{
	int	stdin_backup;
	int	stdout_backup;
	int	status;

	if (cmd->type_redir)
	{
		stdin_backup = dup(STDIN_FILENO);
		stdout_backup = dup(STDOUT_FILENO);
		if (execute_redirections(cmd, shell) != 0)
			return (1);
		status = execute_builtin(cmd, shell);
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		return (status);
	}
	else
		return (execute_builtin(cmd, shell));
}

/*
** Code execute par le processus enfant pour lancer une commande externe.
** 1. reset_signals_for_child(): restaure signaux par defaut (terminables)
** 2. execute_redirections(): applique < > << >> avant execve
** 3. find_command_path(): cherche l'executable dans PATH ou chemin absolu
** 4. env_to_array(): convertit variables shell en tableau pour execve
** 5. execve(): remplace le processus par la nouvelle commande
** Si execve echoue: erreur fatale, le processus enfant exit(1)
** Note: apres execve reussi, ce code n'existe plus (remplace)
*/
void	handle_command(t_cmd *cmd, t_shell *shell)
{
	char	**env_array;

	reset_signals_for_child();
	if (cmd->arg[0] == NULL)
	{
		if (execute_redirections(cmd, shell) != 0)
			exit(1);
	}
	if (cmd->type_redir && execute_redirections(cmd, shell) != 0)
		exit(1);
	cmd->path = find_command_path(cmd->name, shell);
	if (!cmd->path)
	{
		ft_putstr_fd(cmd->name, 2);
		ft_putstr_fd(": command not found\n", 2);
		exit(127);
	}
	env_array = env_to_array(shell->env);
	if (!env_array)
		exit(1);
	execve(cmd->path, cmd->arg, env_array);
	perror("execve");
	exit(1);
}
