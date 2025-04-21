/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/21 14:58:49 by eahmeti          ###   ########.fr       */
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
** Fonction pour construire un motif composite à partir d'une liste de token_word
** Cela permet de gérer les cas comme "Mak"* où le wildcard est adjacent à un token quoté
*/
char	*build_composite_pattern(t_token_word *list)
{
	t_token_word	*current;
	char			*pattern;
	char			*tmp;
	int				has_wildcard;

	if (!list)
		return (NULL);

	pattern = ft_strdup("");
	if (!pattern)
		return (NULL);

	has_wildcard = 0;
	current = list;
	while (current)
	{
		tmp = pattern;
		pattern = ft_strjoin(tmp, current->content);
		free(tmp);
		if (!pattern)
			return (NULL);

		if (ft_strchr(current->content, '*'))
			has_wildcard = 1;
		current = current->next;
	}

	if (!has_wildcard)
	{
		free(pattern);
		return (NULL);
	}

	return (pattern);
}

/*
** Fonction pour étendre les wildcards dans une liste de token_word
** Version modifiée pour gérer correctement les espaces entre les fichiers
** et les tokens adjacents quotés comme "Mak"*
*/
int	expand_wildcard_in_word_list(t_token_word **list)
{
	t_token_word	*new_list;
	t_token_word	*tail;
	char			**expanded;
	char			*composite_pattern;
	int				i;

	if (!list || !*list)
		return (0);

	// Construire un motif composite pour gérer les cas comme "Mak"*
	composite_pattern = build_composite_pattern(*list);

	// Si pas de motif composite (pas de wildcard), pas besoin d'expansion
	if (!composite_pattern)
		return (0);

	// Étendre le motif composite
	expanded = expand_wildcard(composite_pattern);
	free(composite_pattern);
	if (!expanded)
		return (1);

	// Créer une nouvelle liste de tokens avec les résultats de l'expansion
	new_list = NULL;
	tail = NULL;
	for (i = 0; expanded[i]; i++)
	{
		t_token_word *new_word = create_new_token_word(expanded[i], T_NO_QUOTE);
		if (!new_word)
		{
			for (int j = 0; expanded[j]; j++)
				free(expanded[j]);
			free(expanded);
			free_token_word_list(new_list);
			return (1);
		}

		if (!new_list)
			new_list = new_word;
		else
			tail->next = new_word;
		tail = new_word;
	}

	// Si l'expansion n'a rien trouvé, garder la liste originale
	if (!expanded[0])
	{
		for (int j = 0; expanded[j]; j++)
			free(expanded[j]);
		free(expanded);
		return (0);
	}

	// Libérer la liste originale et le tableau d'expansion
	for (i = 0; expanded[i]; i++)
		free(expanded[i]);
	free(expanded);
	free_token_word_list(*list);

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
	int				file_count;

	if (!redir || !redir->word_parts)
		return (0);
	
	// Compter le nombre de token_word pour détecter une expansion multiple
	file_count = 0;
	word = redir->word_parts;
	while (word)
	{
		file_count++;
		word = word->next;
	}
	
	// Si plusieurs fichiers trouvés (expansion wildcard), prendre seulement le premier
	if (file_count > 1 && redir->type_redirection != T_APPEND)
	{
		// Avertir l'utilisateur si plusieurs fichiers ont été trouvés
		ft_putstr_fd("miniHell: ambiguous redirect - using first match: ", 2);
		ft_putendl_fd(redir->word_parts->content, 2);
		
		// Utiliser seulement le premier fichier trouvé
		new_content = ft_strdup(redir->word_parts->content);
	}
	else
	{
		// Cas normal: reconstruire le contenu à partir de tous les tokens
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
	}
	
	// Remplacer l'ancien contenu
	if (!new_content)
		return (1);
	
	free(redir->content);
	redir->content = new_content;
	
	return (0);
}

/*
** Fonction pour reconstruire le contenu d'un token_word spécifiquement pour les wildcards
*/
char	*rebuild_token_word_for_wildcard(t_token_word *word_list)
{
	char			*result;
	char			*tmp;
	t_token_word	*current;

	if (!word_list)
		return (ft_strdup(""));

	result = ft_strdup(word_list->content);
	if (!result)
		return (NULL);

	current = word_list->next;
	while (current)
	{
		tmp = result;
		result = ft_strjoin(tmp, current->content);
		free(tmp);
		if (!result)
			return (NULL);
		current = current->next;
	}

	return (result);
}

/*
** Fonction pour reconstruire les arguments de commande après expansion wildcard
** Cette version est spécifiquement optimisée pour les wildcards
*/
int	rebuild_command_arg_wildcard(t_cmd *cmd)
{
	int				i;
	int				new_ac;
	char			**new_args;
	t_token_word	**new_list_word;
	int				arg_index;

	if (!cmd || !cmd->list_word)
		return (0);

	// Compter le nombre de token_word après expansion
	new_ac = 0;
	for (i = 0; i < cmd->ac; i++)
	{
		t_token_word *current = cmd->list_word[i];
		while (current)
		{
			new_ac++;
			current = current->next;
		}
	}

	// Allouer le nouveau tableau d'arguments et de token_word
	new_args = malloc(sizeof(char *) * (new_ac + 1));
	new_list_word = malloc(sizeof(t_token_word *) * (new_ac + 1));
	if (!new_args || !new_list_word)
	{
		if (new_args)
			free(new_args);
		if (new_list_word)
			free(new_list_word);
		return (1);
	}

	// Remplir les nouveaux tableaux
	arg_index = 0;
	for (i = 0; i < cmd->ac; i++)
	{
		t_token_word *current = cmd->list_word[i];
		while (current)
		{
			// Créer un nouveau token_word pour chaque argument
			t_token_word *new_token = create_new_token_word(current->content, current->type);
			if (!new_token)
			{
				// Nettoyer en cas d'erreur
				for (int j = 0; j < arg_index; j++)
				{
					free(new_args[j]);
					free_token_word_list(new_list_word[j]);
				}
				free(new_args);
				free(new_list_word);
				return (1);
			}

			// Ajouter au tableau
			new_list_word[arg_index] = new_token;
			new_args[arg_index] = ft_strdup(current->content);
			if (!new_args[arg_index])
			{
				// Nettoyer en cas d'erreur
				free_token_word_list(new_token);
				for (int j = 0; j < arg_index; j++)
				{
					free(new_args[j]);
					free_token_word_list(new_list_word[j]);
				}
				free(new_args);
				free(new_list_word);
				return (1);
			}

			arg_index++;
			current = current->next;
		}
	}

	// Terminer les tableaux par NULL
	new_args[arg_index] = NULL;
	new_list_word[arg_index] = NULL;

	// Libérer les anciens tableaux
	for (i = 0; i < cmd->ac; i++)
	{
		if (cmd->arg && cmd->arg[i])
			free(cmd->arg[i]);
	}
	if (cmd->arg)
		free(cmd->arg);
	
	// Ne pas libérer cmd->list_word car les tokens sont déjà réutilisés ou libérés

	// Mettre à jour la commande
	cmd->arg = new_args;
	cmd->list_word = new_list_word;
	cmd->ac = new_ac;

	// Mettre à jour le nom de la commande
	if (cmd->name)
		free(cmd->name);
	if (cmd->ac > 0)
		cmd->name = ft_strdup(cmd->arg[0]);
	else
		cmd->name = ft_strdup("");

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
	int				expanded_something;

	if (!cmd || !cmd->list_word)
		return (0);
    
	has_wildcard = 0;
	expanded_something = 0;
	
	// Vérifier s'il y a des wildcards à expandre dans les arguments
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
    
	// Vérifier aussi les redirections pour les wildcards
	if (!has_wildcard && cmd->type_redir)
	{
		redir = cmd->type_redir;
		while (redir && !has_wildcard)
		{
			t_token_word *word = redir->word_parts;
			while (word)
			{
				if (word->type == T_NO_QUOTE && ft_strchr(word->content, '*'))
				{
					has_wildcard = 1;
					break;
				}
				word = word->next;
			}
			redir = redir->next;
		}
	}
    
	// Si pas de wildcard, rien à faire
	if (!has_wildcard)
		return (0);
    
	// Étendre les wildcards dans les arguments
	for (i = 0; i < cmd->ac && cmd->list_word[i]; i++)
	{
		if (expand_wildcard_in_word_list(&(cmd->list_word[i])) != 0)
			return (1);
		expanded_something = 1;
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
			
			expanded_something = 1;
		}
		redir = redir->next;
	}
	
	// Reconstruire les arguments après expansion seulement si nécessaire
	if (expanded_something)
		return (rebuild_command_arg_wildcard(cmd));
	
	return (0);
}