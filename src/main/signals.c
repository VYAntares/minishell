/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 20:18:39 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 23:07:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/**
 * Gestionnaire pour SIGINT (CTRL+C)
 * - Affiche un nouveau prompt
 * - Définit g_sigint_received pour indiquer que le signal a été traité
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

/**
 * Configuration des signaux pour le shell
 * - SIGINT : gestionnaire personnalisé
 * - SIGQUIT : ignoré
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

/**
 * Configuration des signaux pour l'exécution des commandes
 * - SIGINT : ignoré pour éviter l'interférence dans les shells imbriqués
 * - SIGQUIT : ignoré comme avant
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

/**
 * Réinitialiser les signaux aux comportements par défaut
 * - Pour les commandes externes
 */
void	reset_signals_for_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
