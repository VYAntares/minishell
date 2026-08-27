/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 00:44:43 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 22:53:06 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Supprime une variable d'environnement de la liste chainee.
** Parcourt la liste pour trouver le nom exact (longueurs identiques).
** Gere la suppression en tete (prev=NULL) et en milieu/fin de liste.
** Libere completement le noeud: nom, valeur et structure.
** Relie les noeuds precedent et suivant pour maintenir la chaine.
*/
void	delete_line_env(t_shell *shell, t_cmd *cmd, int i)
{
	t_env	*current;
	t_env	*prev;

	current = shell->env;
	prev = NULL;
	while (current)
	{
		if (ft_strncmp(current->name, cmd->arg[i], ft_strlen(cmd->arg[i]))
			== 0 && ft_strlen(current->name) == ft_strlen(cmd->arg[i]))
		{
			if (prev)
				prev->next = current->next;
			else
				shell->env = current->next;
			dmb_free(current->name);
			dmb_free(current->value);
			dmb_free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

/*
** Implemente la commande unset qui supprime des variables d'environnement.
** Valide chaque argument comme identifiant avant suppression.
** Continue le traitement meme en cas d'erreur sur un argument.
** Accumule les erreurs dans status et retourne le resultat final.
** Exemple: unset HOME PATH VAR_INVALIDE → supprime HOME et PATH.
*/
int	builtin_unset(t_cmd *cmd, t_shell *shell)
{
	int		i;
	int		status;

	i = 1;
	status = 0;
	while (cmd->arg[i])
	{
		if (!is_valid_identifier(cmd->arg[i]))
		{
			ft_putstr_fd("minishell: unset: '", 2);
			ft_putstr_fd(cmd->arg[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			status = 1;
			i++;
			continue ;
		}
		delete_line_env(shell, cmd, i);
		i++;
	}
	return (status);
}
