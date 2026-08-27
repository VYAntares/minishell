/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 03:02:12 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:46:20 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Libère une liste chaînée complète de token_word
** Parcourt la liste et libère le contenu puis le nœud
** Évite les fuites mémoire lors du nettoyage
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
** Crée un nouveau token_word avec le contenu et type donnés
** Alloue la mémoire et duplique le contenu
** Initialise tous les champs et retourne le nouveau token
** Retourne NULL en cas d'erreur d'allocation
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

/*
** Libère un tableau de noms de fichiers et ferme le répertoire
** Utilisé pour nettoyer après échec d'expansion de wildcards
** Libère chaque chaîne puis le tableau, ferme le DIR
*/
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
