/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:18:39 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 20:53:21 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Remet les signaux SIGINT et SIGQUIT au comportement par defaut.
** Utilise dans les processus enfants pour restaurer le comportement
** standard (terminer le processus sur Ctrl+C et Ctrl+\).
*/
void	reset_signals_for_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/*
** Gestionnaire personnalise pour SIGINT (Ctrl+C).
** Affiche une nouvelle ligne, marque le signal recu et rafraichit
** le prompt readline sans terminer le shell.
*/
void	handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	g_sigint_received = 1;
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*
** Configure la gestion des signaux pour le shell principal.
** SIGINT (Ctrl+C) -> gestionnaire personnalise pour nouveau prompt
** SIGQUIT (Ctrl+\) -> ignore completement
*/
int	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handle_sigint;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	return (0);
}

/*
** Configure la gestion des signaux pendant l'execution de commandes.
** Ignore SIGINT et SIGQUIT pour que le shell parent ne soit pas
** affecte par les signaux envoyes aux commandes enfants.
*/
int	setup_signals_for_commands(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		return (perror("Error sigaction"), -1);
	return (0);
}
