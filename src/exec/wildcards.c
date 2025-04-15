/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/15 19:46:35 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <dirent.h>  /* Pour DIR et struct dirent */

/*
** Fonction pour comparer si une chaîne correspond à un motif de wildcard
** Cette fonction gère récursivement les '*' qui peuvent correspondre à zéro ou plusieurs caractères
*/
int	match_wildcard(const char *pattern, const char *str)
{
	// Si on est à la fin des deux chaînes, c'est un match
	if (*pattern == '\0' && *str == '\0')
		return (1);

	// Si on rencontre '*', on a deux choix:
	// 1. Sauter '*' (si la séquence est vide)
	// 2. Utiliser '*' pour correspondre au caractère courant et continuer
	if (*pattern == '*')
		return (match_wildcard(pattern + 1, str) || 
			   (*str != '\0' && match_wildcard(pattern, str + 1)));

	// Pour tout autre caractère, vérifier s'ils sont identiques
	if (*pattern == *str)
		return (match_wildcard(pattern + 1, str + 1));

	// Pas de correspondance
	return (0);
}

/*
** Fonction de comparaison pour qsort
*/
static int	compare_strings(const void *a, const void *b)
{
	return (ft_strncmp(*(const char **)a, *(const char **)b, ft_strlen(*(const char **)b)));
}

/*
** Fonction pour développer un motif de wildcard en liste de fichiers correspondants
*/
char	**expand_wildcard(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**files;
	int				count;
	int				capacity;

	count = 0;
	capacity = 10;
	files = malloc(capacity * sizeof(char *));
	if (!files)
		return (NULL);
	dir = opendir(".");
	if (!dir)
	{
		free(files);
		return (NULL);
	}
	while ((entry = readdir(dir)) != NULL)
	{
		// Ignorer fichiers cachés sauf si spécifiquement demandés
		if (entry->d_name[0] == '.' && pattern[0] != '.')
			continue;
		if (match_wildcard(pattern, entry->d_name))
		{
			// Redimensionner le tableau si nécessaire
			if (count >= capacity)
			{
				capacity *= 2;
				char **temp = realloc(files, capacity * sizeof(char *));
				if (!temp)
				{
					for (int i = 0; i < count; i++)
						free(files[i]);
					free(files);
					closedir(dir);
					return (NULL);
				}
				files = temp;
			}
			files[count] = ft_strdup(entry->d_name);
			if (!files[count])
			{
				for (int i = 0; i < count; i++)
					free(files[i]);
				free(files);
				closedir(dir);
				return (NULL);
			}
			count++;
		}
	}
	closedir(dir);
	// Si aucun match, retourner le motif original
	if (count == 0)
	{
		files[0] = ft_strdup(pattern);
		if (!files[0])
		{
			free(files);
			return (NULL);
		}
		files[1] = NULL;
	}
	else
	{
		// Ajuster le tableau final et le terminer par NULL
		char **temp = realloc(files, (count + 1) * sizeof(char *));
		if (!temp)
		{
			for (int i = 0; i < count; i++)
				free(files[i]);
			free(files);
			return (NULL);
		}
		files = temp;
		files[count] = NULL;
		// Trier les résultats alphabétiquement
		qsort(files, count, sizeof(char *), compare_strings);
	}
	return (files);
}

/*
** Fonction pour créer un nouveau token_word
*/
t_token_word	*create_new_token_word(const char *content, t_type_word type)
{
	t_token_word	*new_word;

	new_word = malloc(sizeof(t_token_word));
	if (!new_word)
		return (NULL);
	new_word->content = ft_strdup(content);
	if (!new_word->content)
	{
		free(new_word);
		return (NULL);
	}
	new_word->type = type;
	new_word->next = NULL;
	return (new_word);
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
		free(current->content);
		free(current);
		current = next;
	}
}

/*
** Fonction pour étendre les wildcards dans une liste de token_word
** Version modifiée pour gérer correctement les espaces entre les fichiers
*/
int	expand_wildcard_in_word_list(t_token_word **list)
{
	t_token_word	*current;
	t_token_word	*next;
	t_token_word	*new_list;
	t_token_word	*tail;
	char			**expanded;
	int				i;

	if (!list || !*list)
		return (0);
	new_list = NULL;
	tail = NULL;
	current = *list;
	
	while (current)
	{
		next = current->next;
		
		// Si c'est un token non quoté contenant un wildcard
		if (current->type == T_NO_QUOTE && ft_strchr(current->content, '*'))
		{
			// Étendre le wildcard
			expanded = expand_wildcard(current->content);
			if (!expanded)
				return (1);
				
			// Si pas d'expansion, garder le token original
			if (!expanded[0])
			{
				if (!new_list)
					new_list = current;
				else
					tail->next = current;
				tail = current;
				tail->next = NULL;
				free(expanded);
			}
			else
			{
				// Ajouter chaque fichier correspondant comme un token séparé
				for (i = 0; expanded[i]; i++)
				{
					t_token_word *new_word = create_new_token_word(expanded[i], T_NO_QUOTE);
					if (!new_word)
					{
						// Libérer la mémoire en cas d'erreur
						for (int j = 0; expanded[j]; j++)
							free(expanded[j]);
						free(expanded);
						free_token_word_list(new_list);
						return (1);
					}
					
					// Ajouter à la nouvelle liste
					if (!new_list)
						new_list = new_word;
					else
						tail->next = new_word;
					tail = new_word;
				}
				
				// Libérer le tableau et le token original
				for (i = 0; expanded[i]; i++)
					free(expanded[i]);
				free(expanded);
				free(current->content);
				free(current);
			}
		}
		else
		{
			// Copier le token tel quel s'il n'y a pas de wildcard ou s'il est entre quotes
			if (!new_list)
				new_list = current;
			else
				tail->next = current;
			tail = current;
			tail->next = NULL;
		}
		
		current = next;
	}
	
	// Mettre à jour le pointeur de liste
	*list = new_list;
	return (0);
}

/*
** Fonction pour reconstruire la chaîne de contenu après l'expansion wildcard pour les redirections
*/
int	rebuild_redirection_content(t_file_redir *redir)
{
	char			*new_content;
	t_token_word	*word;

	if (!redir || !redir->word_parts)
		return (0);
	
	new_content = ft_strdup("");
	if (!new_content)
		return (1);
	
	word = redir->word_parts;
	while (word)
	{
		char *tmp = ft_strjoin(new_content, word->content);
		free(new_content);
		if (!tmp)
			return (1);
		new_content = tmp;
		word = word->next;
	}
	
	// Remplacer l'ancien contenu
	free(redir->content);
	redir->content = new_content;
	
	return (0);
}

/*
** Version améliorée de la fonction expand_wildcards
** Gère correctement la séparation des fichiers dans l'expansion
*/
int	expand_wildcards(t_cmd *cmd)
{
	int				i;
	t_file_redir	*redir;
	int				has_wildcard;

	if (!cmd || !cmd->list_word)
		return (0);
    
	has_wildcard = 0;
	// Vérifier d'abord s'il y a des wildcards à expandre
	for (i = 0; i < cmd->ac && cmd->list_word[i]; i++)
	{
		t_token_word *word = cmd->list_word[i];
		while (word)
		{
			if (word->type == T_NO_QUOTE && ft_strchr(word->content, '*'))
			{
				has_wildcard = 1;
				break;
			}
			word = word->next;
		}
		if (has_wildcard)
			break;
	}
    
	// Si pas de wildcard, rien à faire
	if (!has_wildcard)
		return (0);
    
	// Étendre les wildcards dans les arguments
	for (i = 0; i < cmd->ac && cmd->list_word[i]; i++)
	{
		if (expand_wildcard_in_word_list(&(cmd->list_word[i])) != 0)
			return (1);
	}
	
	// Étendre les wildcards dans les redirections
	redir = cmd->type_redir;
	while (redir)
	{
		if (redir->word_parts && redir->type_redirection != T_HEREDOC)
		{
			if (expand_wildcard_in_word_list(&(redir->word_parts)) != 0)
				return (1);
				
			// Reconstruire le contenu de la redirection après expansion
			if (rebuild_redirection_content(redir) != 0)
				return (1);
		}
		redir = redir->next;
	}
	
	// Reconstruire les arguments après expansion
	return (rebuild_command_arg(cmd));
}
