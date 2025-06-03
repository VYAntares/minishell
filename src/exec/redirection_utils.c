/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:27:25 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:21:56 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Trouve les dernieres redirections d'entree et sortie dans une liste.
** Parcourt toutes les redirections et garde les dernieres trouvees:
** - last_input: derniere < ou << (priorite a la derniere)
** - last_output: derniere > ou >> (priorite a la derniere)
** Respecte la semantique bash: seule la derniere redirection compte.
*/
int	find_lasts_redirection(t_file_redir *redir,
							t_file_redir **last_input,
							t_file_redir **last_output)
{
	while (redir)
	{
		if (redir->type_redirection == T_REDIR_IN
			|| redir->type_redirection == T_HEREDOC)
			*last_input = redir;
		else if (redir->type_redirection == T_REDIR_OUT
			|| redir->type_redirection == T_APPEND)
			*last_output = redir;
		redir = redir->next;
	}
	return (0);
}

/*
** Affiche un message d'erreur pour une redirection ambigue.
** Se produit quand une variable expandee contient des espaces:
** exemple: VAR="file1 file2" puis > $VAR (vers quel fichier?)
** Cherche les variables non quotees contenant $ pour le message.
** Retourne 1 pour signaler l'erreur a l'appelant.
*/
int	ambiguous_error(t_file_redir *redir)
{
	t_token_word	*word;

	word = redir->word_parts;
	while (word)
	{
		if (word->type == T_NO_QUOTE && ft_strchr(word->content, '$'))
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(word->content, 2);
			ft_putendl_fd(": ambiguous redirect", 2);
			return (1);
		}
		word = word->next;
	}
	ft_putendl_fd("minishell: ambiguous redirect", 2);
	return (1);
}
