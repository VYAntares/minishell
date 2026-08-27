/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:53:27 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:33:19 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Reconstruit le nom de fichier final apres expansion des variables.
** Concatene tous les token_word d'une redirection en une seule chaine.
** Exemple: "> $HOME/file.txt" devient "> /Users/nom/file.txt"
** Remplace redir->content par la version finale expansee.
** Appele apres que toutes les expansions soient terminees.
*/
int	rebuild_redirection(t_cmd *cmd)
{
	char			*new_content;
	t_file_redir	*redir;
	t_token_word	*current;

	redir = cmd->type_redir;
	while (redir)
	{
		new_content = ft_strdup("");
		if (!new_content)
			return (1);
		current = redir->word_parts;
		while (current)
		{
			new_content = ft_strjoin(new_content, current->content);
			if (!new_content)
				return (1);
			current = current->next;
		}
		redir->content = new_content;
		redir = redir->next;
	}
	return (0);
}

/*
** Expanse les variables d'environnement dans un token_word de redirection.
** Detecte l'ambiguite: si expansion contient espaces sans quotes.
** Exemple ambigu: VAR="file1 file2" puis > $VAR (vers quels fichiers?)
** Single quotes bloquent l'expansion: > '$HOME' reste litteralement $HOME
** Double quotes permettent l'expansion: > "$HOME" devient > /Users/nom
*/
int	expand_redir_name(t_token_word *current,
					t_shell *shell,
					t_file_redir *redir)
{
	int	contains_dollar;

	contains_dollar = ft_strchr(current->content, '$') != NULL;
	if (current->type == T_NO_QUOTE && contains_dollar)
	{
		if (expand_env_var(current, shell) != 0)
			return (1);
		if (ft_strchr(current->content, ' ')
			|| ft_strchr(current->content, '\t'))
			redir->is_ambiguous = 1;
	}
	else if (current->type != T_S_QUOTE)
	{
		if (expand_env_var(current, shell) != 0)
			return (1);
	}
	return (0);
}

/*
** Lance l'expansion pour une redirection complete.
** Ignore les heredocs (delimiteur ne doit pas etre expanse).
** Parcourt tous les token_word et les expanse individuellement.
** Marque la redirection comme ambigue si necessaire.
** Retourne 0 si succes, 1 en cas d'erreur d'expansion.
*/
int	launch_redir_expansion(t_shell *shell, t_file_redir *redir)
{
	t_token_word	*current;

	redir->is_ambiguous = 0;
	if (redir->type_redirection == T_HEREDOC)
	{
		redir = redir->next;
		return (0);
	}
	current = redir->word_parts;
	while (current)
	{
		if (expand_redir_name(current, shell, redir) != 0)
			return (1);
		current = current->next;
	}
	return (0);
}

/*
** Point d'entree pour l'expansion de toutes les redirections d'une commande.
** 1. Expanse les variables dans chaque redirection
** 2. Detecte les redirections ambigues (multiples fichiers)
** 3. Reconstruit les noms finaux si pas d'ambiguite
** 4. Ignore les heredocs qui ont un traitement special
** Prerequis obligatoire avant l'execution des redirections.
*/
int	expand_redir(t_cmd *cmd, t_shell *shell)
{
	t_file_redir	*redir;

	redir = cmd->type_redir;
	while (redir)
	{
		if (launch_redir_expansion(shell, redir) != 0)
			return (1);
		redir = redir->next;
	}
	if (!cmd->type_redir->is_ambiguous
		&& cmd->type_redir->type_redirection != T_HEREDOC)
	{
		if (rebuild_redirection(cmd) != 0)
			return (1);
	}
	return (0);
}
