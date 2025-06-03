/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_wildcards.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:40:50 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:40:43 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Double la capacité du tableau de fichiers quand il est plein
** Copie les fichiers existants dans le nouveau tableau
** Libère l'ancien tableau et ferme le répertoire en cas d'erreur
** Retourne le nouveau tableau ou NULL si échec
*/
char	**realloc_capacity(char **files, int *capacity, int count, DIR *dir)
{
	char	**new_files;
	int		i;

	i = 0;
	*capacity *= 2;
	new_files = dmb_malloc(*capacity * sizeof(char *));
	if (!new_files)
	{
		i = 0;
		while (i < count)
			dmb_free(files[i++]);
		dmb_free(files);
		closedir(dir);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		new_files[i] = files[i];
		i++;
	}
	dmb_free(files);
	return (new_files);
}

/*
** Remplit le tableau avec les fichiers correspondant au wildcard
** Ignore les fichiers cachés sauf si le pattern commence par '.'
** Réalloue dynamiquement si le tableau devient trop petit
** Retourne le tableau rempli ou NULL en cas d'erreur
*/
char	**fill_wildcard_array(DIR *dir, const char *wildcard_str,
								char **files, int *count)
{
	struct dirent	*entry;
	int				capacity;

	capacity = 10;
	entry = readdir(dir);
	while (entry)
	{
		if (entry->d_name[0] == '.' && wildcard_str[0] != '.')
		{
			entry = readdir(dir);
			continue ;
		}
		if (match_wildcard(wildcard_str, entry->d_name))
		{
			if (*count >= capacity)
				files = realloc_capacity(files, &capacity, *count, dir);
			files[*count] = ft_strdup(entry->d_name);
			if (!files[*count])
				return (free_files(files, *count, dir), NULL);
			(*count)++;
		}
		entry = readdir(dir);
	}
	return (files);
}

/*
** Finalise le tableau de fichiers trouvés
** Crée un tableau final de taille exacte terminé par NULL
** Trie les fichiers par ordre alphabétique
** Retourne le tableau final ou NULL en cas d'erreur
*/
char	**match_wildcard_array(char **files, int count)
{
	char	**new_files;
	int		i;

	new_files = dmb_malloc((count + 1) * sizeof(char *));
	if (!new_files)
	{
		i = 0;
		while (i < count)
			dmb_free(files[i++]);
		dmb_free(files);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		new_files[i] = files[i];
		i++;
	}
	dmb_free(files);
	files = new_files;
	files[count] = NULL;
	bubble_sort(files);
	return (files);
}

/*
** Gère le cas où aucun fichier ne correspond au wildcard
** Retourne un tableau contenant uniquement le pattern original
** Suit le comportement standard du shell (pas d'expansion)
*/
char	**no_match_wildcard(const char *wildcard_str, char **files)
{
	files[0] = ft_strdup(wildcard_str);
	if (!files[0])
		return (dmb_free(files), NULL);
	files[1] = NULL;
	return (files);
}

/*
** Point d'entrée principal pour l'expansion des wildcards
** Ouvre le répertoire courant et cherche les fichiers correspondants
** Retourne le pattern original si aucun match, sinon liste triée
*/
char	**expand_wildcard(const char *wildcard_str)
{
	DIR				*dir;
	char			**files;
	int				count;

	count = 0;
	files = dmb_malloc(10 * sizeof(char *));
	if (!files)
		return (NULL);
	dir = opendir(".");
	if (!dir)
		return (NULL);
	files = fill_wildcard_array(dir, wildcard_str, files, &count);
	closedir(dir);
	if (count == 0)
		files = no_match_wildcard(wildcard_str, files);
	else
		files = match_wildcard_array(files, count);
	return (files);
}
