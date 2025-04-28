/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/15 20:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/28 18:05:43 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <dirent.h>  /* Pour DIR et struct dirent */

/*
** Fonction pour comparer si une chaîne correspond à un motif de wildcard
** Cette fonction gère récursivement les '*' qui peuvent correspondre à zéro ou plusieurs caractères
*/
int	match_wildcard(const char *wildcard_str, const char *str)
{
	if (*wildcard_str == '\0' && *str == '\0')
		return (1);
	if (*wildcard_str == '*')
		return (match_wildcard(wildcard_str + 1, str)
				|| (*str != '\0' && match_wildcard(wildcard_str, str + 1)));
	if (*wildcard_str == *str)
		return (match_wildcard(wildcard_str + 1, str + 1));
	return (0);
}

char	**realloc_capacity(char **files, int *capacity, int count, DIR *dir)
{
	char	**new_files;
	int		i;

	i = 0;
	*capacity *= 2;
	new_files = malloc(*capacity * sizeof(char *));
	if (!new_files)
	{
		i = 0;
		while (i < count)
			free(files[i++]);
		free(files);
		closedir(dir);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		new_files[i] = files[i];
		i++;
	}
	free(files);
	return (new_files);
}

void	free_files(char **files, int count, DIR *dir)
{
	int	i;
	
	i = 0;
	while (i < count)
	{
		free(files[i]);
		i++;
	}
	free(files);
	closedir(dir);
}

char	**no_match_wildcard(const char *wildcard_str, char **files)
{	
	files[0] = ft_strdup(wildcard_str);
	if (!files[0])
		return (free(files), NULL);
	files[1] = NULL;
	return (files);
}

char	**match_wildcard_array(char **files, int count)
{
	char	**new_files;
	int		i;
	
	new_files = malloc((count + 1) * sizeof(char *));
	if (!new_files)
	{
		i = 0;
		while (i < count)
			free(files[i++]);
		free(files);
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		new_files[i] = files[i];
		i++;
	}
	free(files);
	files = new_files;
	files[count] = NULL;
	bubble_sort(files);
	return (files);
}

char **fill_wildcard_array(DIR *dir, const char *wildcard_str, char **files, int *count)
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
** Fonction pour développer un motif de wildcard en liste de fichiers correspondants
*/
char	**expand_wildcard(const char *wildcard_str)
{
	DIR				*dir;
	char			**files;
	int				count;

	count = 0;
	files = malloc(10 * sizeof(char *));
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
		return (free(new_word), NULL);
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
char	*build_composite_wildcard_str(t_token_word *list)
{
	t_token_word	*current;
	char			*wildcard_str;
	int				has_wildcard;

	if (!list)
		return (NULL);
	wildcard_str = ft_strdup("");
	if (!wildcard_str)
		return (NULL);
	has_wildcard = 0;
	current = list;
	while (current)
	{
		wildcard_str = ft_strjoin(wildcard_str, current->content);
		if (!wildcard_str)
			return (NULL);
		if (ft_strchr(current->content, '*'))
			has_wildcard = 1;
		current = current->next;
	}
	if (!has_wildcard)
		return (free(wildcard_str), NULL);
	return (wildcard_str);
}

t_token_word	*expanded_wildcard_list(char **expanded)
{
	t_token_word	*new_word;
	t_token_word	*tail;
	t_token_word	*head;
	int				i;
	
	i = 0;
	tail = NULL;
	head = NULL;
	while (expanded[i])
	{
		new_word = create_new_token_word(expanded[i++], T_NO_QUOTE);
		if (!new_word)
		{
			i = 0;
			while (expanded[i])
				free(expanded[i++]);
			return (free(expanded), free_token_word_list(head), NULL);
		}
		if (!head)
			head = new_word;
		else
			tail->next = new_word;
		tail = new_word;
	}
	return (head);
}

/*
** Fonction pour étendre les wildcards dans une liste de token_word
** Version modifiée pour gérer correctement les espaces entre les fichiers
** et les tokens adjacents quotés comme "Mak"*
*/
int	expand_wildcard_in_word_list(t_token_word **list)
{
	t_token_word	*new_list;
	char			**expanded;
	char			*composite_wildcard_str;
	int				i;
	
	if (!list || !*list)
		return (0);
	composite_wildcard_str = build_composite_wildcard_str(*list);
	if (!composite_wildcard_str)
		return (0);
	expanded = expand_wildcard(composite_wildcard_str);
	free(composite_wildcard_str);
	if (!expanded)
		return (1);
	new_list = expanded_wildcard_list(expanded);
	i = 0;
	while (expanded[i])
    	free(expanded[i++]);
	free(expanded);
	free_token_word_list(*list);
	*list = new_list;
	return (0);
}

int	ambiguous_multiple_file_detected(t_file_redir *redir)
{
	ft_putstr_fd("miniHell: ", 2);
	ft_putstr_fd(redir->content, 2);
	ft_putendl_fd(": ambiguous redirect", 2);
	return (1);	
}

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
		free(new_content);
		if (!tmp)
			return (NULL);
		new_content = tmp;
		word = word->next;
	}
	return (new_content);
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

int	count_arg_after_expansion(t_cmd *cmd)
{
	int	i;
	int	new_ac;
	
	i = 0;
	new_ac = 0;
	while (i < cmd->ac)
	{
		t_token_word *current = cmd->list_word[i];
		while (current)
		{
			new_ac++;
			current = current->next;
		}
		i++;
	}
	return (new_ac);
}

int	allocate_array_arg(char ***new_args,
						int new_ac)
{
	*new_args = malloc(sizeof(char *) * (new_ac + 1));
	if (!*new_args)
	{
		if (*new_args)
			free(*new_args);
		return (1);
	}
	return (0);
}

int	free_arrays_args(char **new_args, int arg_index)
{
	int	j;
	
	j = 0;
	while (j < arg_index)
	{
		free(new_args[j]);
		j++;
	}
	free(new_args);
	return (1);
}

char	**create_new_args_array(t_cmd *cmd, int new_ac)
{
	int				i;
	int				arg_index;
	char			**new_args;
	t_token_word	*current;
	
	i = 0;
	arg_index = 0;
	new_args = malloc(sizeof(char *) * (new_ac + 1));
	if (!new_args)
	{
		if (new_args)
			free(new_args);
		return (NULL);
	}
	while (i < cmd->ac)
	{
		current = cmd->list_word[i++];
		while (current)
		{
			new_args[arg_index] = ft_strdup(current->content);
			arg_index++;
			current = current->next;
		}
	}
	return (new_args[arg_index] = NULL, new_args);
}

void	update_command(t_cmd *cmd, char **new_args, int new_ac)
{
	int	i;
	
	i = 0;
	while (i < cmd->ac)
	{
		if (cmd->arg && cmd->arg[i])
			free(cmd->arg[i++]);
	}
	if (cmd->arg)
		free(cmd->arg);
	cmd->arg = new_args;
	cmd->ac = new_ac;
	if (cmd->name)
		free(cmd->name);
	if (cmd->ac > 0)
		cmd->name = ft_strdup(cmd->arg[0]);
	else
		cmd->name = ft_strdup("");
}

/*
** Fonction pour reconstruire les arguments de commande après expansion wildcard
** Cette version est spécifiquement optimisée pour les wildcards
*/
int	rebuild_command_arg_wildcard(t_cmd *cmd)
{
	int				new_ac;
	char			**new_args;

	if (!cmd || !cmd->list_word)
		return (0);
	new_ac = count_arg_after_expansion(cmd);
	new_args = create_new_args_array(cmd, new_ac);
	update_command(cmd, new_args, new_ac);
	return (0);
}

int	has_wildcard_in_word(t_token_word *word_parts)
{
	t_token_word	*word;
	int				flag;
	
	flag = 0;
	word = word_parts;
	while (word)
	{
		if (word->type != T_NO_QUOTE && ft_strchr(word->content, '*'))
			return (0);
		if (word->type == T_NO_QUOTE && ft_strchr(word->content, '*'))
			flag = 1;
		word = word->next;
	}
	return (flag);
}

int	has_wildcard_in_cmd(t_cmd *cmd)
{
	int				has_wildcard;
	int				i;
	t_file_redir	*redir;

	i = 0;
	while (i < cmd->ac && cmd->list_word[i])
	{
		has_wildcard = has_wildcard_in_word(cmd->list_word[i]);
		if (has_wildcard)
			break ;
		i++;
	}
	if (!has_wildcard && cmd->type_redir)
	{
		redir = cmd->type_redir;
		while (redir && !has_wildcard)
			has_wildcard = has_wildcard_in_word(redir->word_parts);
		redir = redir->next;
	}
	return (has_wildcard);
}

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

int	launch_expand_wildcards(t_cmd *cmd)
{
	int				i;
	int				expanded_something;

	if (!cmd || !cmd->list_word)
		return (0);
	if (!has_wildcard_in_cmd(cmd))
		return (0);
	expanded_something = 0;
	i = 0;
	while (i < cmd->ac && cmd->list_word[i])
	{
		if (expand_wildcard_in_word_list(&(cmd->list_word[i])) != 0)
			return (1);
		expanded_something = 1;
		i++;
	}
	if (expand_wildcard_in_redir(cmd, expanded_something))
		return (1);
	return (0);
}
