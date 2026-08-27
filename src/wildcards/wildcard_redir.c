/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:46:33 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:44:57 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Reconstruit le contenu complet d'une redirection
** Concatène tous les token_word de la redirection
** Utilisé après expansion des wildcards pour obtenir le nom final
** Retourne la chaîne complète ou NULL en cas d'erreur
*/
char	*redirection_content(t_file_redir *redir)
{
	char			*new_content;
	char			*tmp;
	t_token_word	*word;

	new_content = ft_strdup("");
	if (!new_content)
		return (NULL);
	word = redir->word_parts;
	while (word)
	{
		tmp = ft_strjoin(new_content, word->content);
		dmb_free(new_content);
		if (!tmp)
			return (NULL);
		new_content = tmp;
		word = word->next;
	}
	return (new_content);
}

/*
** Affiche un message d'erreur pour redirection ambiguë
** Appelé quand un wildcard correspond à plusieurs fichiers
** Retourne 1 pour indiquer une erreur de redirection
*/
int	ambiguous_multiple_file_detected(t_file_redir *redir)
{
	ft_putstr_fd("miniHell: ", 2);
	ft_putstr_fd(redir->content, 2);
	ft_putendl_fd(": ambiguous redirect", 2);
	return (1);
}

/*
** Reconstruit le contenu d'une redirection après expansion wildcards
** Vérifie qu'il n'y a pas d'ambiguïté (plusieurs fichiers)
** Exception : T_APPEND autorise plusieurs fichiers
** Retourne 0 en cas de succès, 1 si ambiguë
*/
int	rebuild_redirection_content(t_file_redir *redir)
{
	char			*new_content;
	t_token_word	*word;
	int				file_count;

	if (!redir || !redir->word_parts)
		return (0);
	file_count = 0;
	word = redir->word_parts;
	while (word)
	{
		file_count++;
		word = word->next;
	}
	if (file_count > 1 && redir->type_redirection != T_APPEND)
		return (ambiguous_multiple_file_detected(redir));
	else
		new_content = redirection_content(redir);
	if (!new_content)
		return (1);
	dmb_free(redir->content);
	redir->content = new_content;
	return (0);
}

/*
** Expanse les wildcards dans toutes les redirections d'une commande
** Ignore les heredocs (pas d'expansion de wildcards)
** Reconstruit les arguments si au moins une expansion effectuée
** Retourne 1 en cas d'erreur, rebuild_command_arg_wildcard sinon
*/
int	expand_wildcard_in_redir(t_cmd *cmd, int expanded_something)
{
	t_file_redir	*redir;

	redir = cmd->type_redir;
	while (redir)
	{
		if (redir->word_parts && redir->type_redirection != T_HEREDOC)
		{
			if (expand_wildcard_in_word_list(&(redir->word_parts)) != 0)
				return (1);
			if (rebuild_redirection_content(redir) != 0)
				return (1);
			expanded_something = 1;
		}
		redir = redir->next;
	}
	if (expanded_something)
		return (rebuild_command_arg_wildcard(cmd));
	return (1);
}
