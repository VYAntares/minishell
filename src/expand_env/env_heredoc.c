/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_heredoc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:52:03 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:30:01 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Expanse les variables d'environnement dans une ligne de heredoc
** Parcourt la ligne à la recherche de '$' suivi d'un caractère
** Remplace chaque variable par sa valeur et repart du début
** Retourne la ligne originale si aucune expansion, sinon la nouvelle
*/
char	*expand_env_heredoc(char *line, t_shell *shell)
{
	char	*expanded_line;
	int		i;
	int		start;

	i = 0;
	start = 0;
	expanded_line = NULL;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1] && line[i + 1] != ' ')
		{
			start = ++i;
			expanded_line = launch_expansion(line, shell, &i, &start);
			dmb_free(line);
			line = ft_strdup(expanded_line);
			if (!line)
				return (NULL);
			i = 0;
		}
		i++;
	}
	if (!expanded_line)
		return (line);
	return (expanded_line);
}
