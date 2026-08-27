/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:25:48 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:23:51 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Execute la commande gauche d'un pipe (cmd1 dans "cmd1 | cmd2").
** 1. Ferme pipefd[0] car cette commande ecrit seulement dans le pipe
** 2. dup2(pipefd[1], STDOUT) redirige sa sortie vers le pipe
** 3. Ferme pipefd[1] original car on a la copie sur STDOUT
** 4. Execute la commande - sa sortie ira dans le pipe au lieu de l'ecran
*/
void	execute_left_command_in_pipe(int pipefd[2],
									t_ast *left,
									t_shell *shell)
{
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	exit(execute_ast(left, shell));
}

/*
** Nettoie en cas d'echec du fork du processus droit.
** Ferme les deux descripteurs du pipe pour eviter les fuites.
** Attend le processus gauche deja cree pour eviter un zombie.
** Retourne une erreur apres nettoyage complet.
*/
int	handle_right_process_fork_error(int pipefd[2], int pid1)
{
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, NULL, 0);
	return (perror("fork"), 1);
}

/*
** Execute la commande droite d'un pipe (cmd2 dans "cmd1 | cmd2").
** 1. Ferme pipefd[1] car cette commande lit seulement depuis le pipe
** 2. dup2(pipefd[0], STDIN) redirige son entree depuis le pipe
** 3. Ferme pipefd[0] original car on a la copie sur STDIN
** 4. Execute la commande - elle lira depuis le pipe au lieu du clavier
*/
void	execute_right_command_in_pipe(int pipefd[2],
									t_ast *right,
									t_shell *shell)
{
	close(pipefd[1]);
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	exit(execute_ast(right, shell));
}

/*
** Orchestre l'execution d'un pipe complet (cmd1 | cmd2).
** 1. pipe() cree 2 descripteurs: pipefd[0]=lecture, pipefd[1]=ecriture
** 2. fork() cree enfant1 qui execute cmd1 et ecrit dans pipefd[1]
** 3. fork() cree enfant2 qui execute cmd2 et lit depuis pipefd[0]
** 4. Parent ferme LES DEUX descripteurs (crucial pour eviter blocage)
** 5. waitpid() attend les deux enfants et retourne le statut de cmd2
** Resultat: sortie de cmd1 devient entree de cmd2 via le pipe
*/
int	execute_pipe(t_ast *left, t_ast *right, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1;
	int		status2;

	if (pipe(pipefd) == -1)
		return (perror("pipe"), 1);
	pid1 = fork();
	if (pid1 == -1)
		return (perror("fork"), close(pipefd[0]), close(pipefd[1]), 1);
	if (pid1 == 0)
		execute_left_command_in_pipe(pipefd, left, shell);
	pid2 = fork();
	if (pid2 == -1)
		return (handle_right_process_fork_error(pipefd, pid1));
	if (pid2 == 0)
		execute_right_command_in_pipe(pipefd, right, shell);
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status2))
		return (WEXITSTATUS(status2));
	return (1);
}
