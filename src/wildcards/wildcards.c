/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:59:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:48:04 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Construit une chaîne composite à partir d'une liste de token_word
** Concatène tous les contenus et vérifie la présence d'un wildcard
** Retourne NULL si aucun '*' trouvé (pas d'expansion nécessaire)
** Retourne la chaîne complète si wildcard présent
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
		return (dmb_free(wildcard_str), NULL);
	return (wildcard_str);
}

/*
** Convertit un tableau de fichiers en liste chaînée de token_word
** Crée un token_word de type T_NO_QUOTE pour chaque fichier
** Construit une liste chaînée et libère le tableau original
** Retourne la nouvelle liste ou NULL en cas d'erreur
*/
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
				dmb_free(expanded[i++]);
			return (dmb_free(expanded), free_token_word_list(head), NULL);
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
** Expanse les wildcards dans une liste de token_word
** Construit le pattern, l'expanse et remplace la liste originale
** Libère l'ancienne liste et installe la nouvelle
** Retourne 0 en cas de succès, 1 en cas d'erreur
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
	dmb_free(composite_wildcard_str);
	if (!expanded)
		return (1);
	new_list = expanded_wildcard_list(expanded);
	i = 0;
	while (expanded[i])
		dmb_free(expanded[i++]);
	dmb_free(expanded);
	free_token_word_list(*list);
	*list = new_list;
	return (0);
}

/*
** Point d'entrée principal pour l'expansion des wildcards d'une commande
** Vérifie d'abord si des wildcards sont présents
** Expanse tous les arguments puis les redirections
** Retourne 0 en cas de succès, 1 en cas d'erreur
*/
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
