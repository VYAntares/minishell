/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:24:57 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Expanse les variables d'environnement dans un token_word
** Parcourt le contenu à la recherche de '$' suivi d'un caractère
** Remplace chaque variable par sa valeur via launch_expansion
** Retourne 0 en cas de succès
*/
int	expand_env_var(t_token_word *token_word, t_shell *shell)
{
	int		i;
	int		start;
	char	*content;
	char	*new_content;

	i = 0;
	start = 0;
	content = token_word->content;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1] && content[i + 1] != ' ')
		{
			start = ++i;
			new_content = launch_expansion(content, shell, &i, &start);
			token_word->content = new_content;
			content = token_word->content;
			i = 0;
		}
		i++;
	}
	return (0);
}

/*
** Reconstruit le tableau d'arguments après expansion des variables
** Calcule le nouveau nombre d'arguments (avec splitting)
** Crée le nouveau tableau et remplace l'ancien
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
int	rebuild_command_arg(t_cmd *cmd)
{
	int				i;
	int				k;
	int				new_ac;
	char			**new_args;

	i = 0;
	new_ac = 0;
	new_args = NULL;
	while (i < cmd->ac)
	{
		if (should_we_split(cmd->list_word[i]) == 0)
			new_ac++;
		else
			new_ac += count_arg_after_split(cmd->list_word[i]);
		i++;
	}
	new_args = dmb_malloc(sizeof(char *) * (new_ac + 1));
	if (!new_args)
		return (1);
	k = 0;
	i = 0;
	while (i < cmd->ac)
		fill_new_args(cmd->list_word[i++], &k, new_args);
	replace_args_and_free(new_args, cmd, k, new_ac);
	return (0);
}

/*
** Point d'entrée principal pour l'expansion des variables d'une commande
** Préprocesse les dollars, expanse toutes les variables non-quotées
** Reconstruit les arguments après expansion et splitting
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
int	expand_var(t_cmd *cmd, t_shell *shell)
{
	t_token_word	*list;
	int				i;

	if (!cmd || !cmd->list_word)
		return (0);
	i = 0;
	if (cmd->type_redir && prepro_dol_redir(cmd->type_redir->word_parts) != 0)
		return (1);
	while (i < cmd->ac && cmd->list_word[i])
	{
		if (preprocess_dollar_quotes(&(cmd->list_word[i])) != 0)
			return (1);
		list = cmd->list_word[i];
		while (list)
		{
			if (list->type != T_S_QUOTE)
			{
				if (expand_env_var(list, shell) != 0)
					return (1);
			}
			list = list->next;
		}
		i++;
	}
	return (rebuild_command_arg(cmd));
}
