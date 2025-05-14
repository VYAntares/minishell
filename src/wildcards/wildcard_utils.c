/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 03:02:12 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 03:03:15 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	free_arrays_args(char **new_args, int arg_index)
{
	int	j;

	j = 0;
	while (j < arg_index)
	{
		dmb_free(new_args[j]);
		j++;
	}
	dmb_free(new_args);
	return (1);
}

int	allocate_array_arg(char ***new_args,
						int new_ac)
{
	*new_args = dmb_malloc(sizeof(char *) * (new_ac + 1));
	if (!*new_args)
	{
		if (*new_args)
			dmb_free(*new_args);
		return (1);
	}
	return (0);
}

/*
** Fonction pour libérer une liste de token_word
*/
void	free_token_word_list(t_token_word *list)
{
	t_token_word	*current;
	t_token_word	*next;

	current = list;
	while (current)
	{
		next = current->next;
		dmb_free(current->content);
		dmb_free(current);
		current = next;
	}
}

/*
** Fonction pour créer un nouveau token_word
*/
t_token_word	*create_new_token_word(const char *content, t_type_word type)
{
	t_token_word	*new_word;

	new_word = dmb_malloc(sizeof(t_token_word));
	if (!new_word)
		return (NULL);
	new_word->content = ft_strdup(content);
	if (!new_word->content)
		return (dmb_free(new_word), NULL);
	new_word->type = type;
	new_word->next = NULL;
	return (new_word);
}


void	free_files(char **files, int count, DIR *dir)
{
	int	i;

	i = 0;
	while (i < count)
	{
		dmb_free(files[i]);
		i++;
	}
	dmb_free(files);
	closedir(dir);
}