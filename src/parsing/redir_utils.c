/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:45:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:10:48 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Vérifie si le type de token correspond à un argument de commande
** Retourne 1 si c'est un WORD, ENV_VAR ou WILDCARD, 0 sinon
*/	
int	is_command_argument(t_type type)
{
	return (type == T_WORD || type == T_ENV_VAR || type == T_WILDCARD);
}

/*
** Vérifie si le type de token correspond à une redirection
** Retourne 1 si c'est REDIR_IN, REDIR_OUT, HEREDOC ou APPEND, 0 sinon
*/
int	is_redirection(t_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_HEREDOC || type == T_APPEND);
}

/*
** Passe au token suivant en sautant les redirections
** Avance de deux tokens (opérateur + fichier) pour chaque redirection
** Retourne le prochain token non-redirection
*/
t_token	*skip_redirection(t_token *current)
{
	while (is_redirection(current->type) && current->next)
		current = current->next->next;
	return (current);
}

/*
** Crée une structure de redirection à partir d'un token
** Extrait le type, le fichier cible et les parties de mots
** Vérifie la syntaxe et initialise tous les champs
** Retourne la redirection créée ou NULL en cas d'erreur
*/
t_file_redir	*create_redir(t_token *current)
{
	t_file_redir	*redir;

	if (!current || !current->next)
	{
		ft_putstr_fd
		("minishell: syntax error near unexpected token `newline'\n", 2);
		return (NULL);
	}
	redir = dmb_malloc(sizeof(t_file_redir));
	if (!redir)
		return (NULL);
	redir->type_redirection = current->type;
	redir->content = ft_strdup(current->next->value);
	if (!redir->content)
		return (dmb_free(redir), NULL);
	redir->word_parts = current->next->type_word;
	redir->is_ambiguous = 0;
	redir->next = NULL;
	return (redir);
}

/*
** Ajoute une redirection à la liste des redirections d'une commande
** Crée la redirection et l'insère en fin de liste chaînée
** Retourne 1 en cas de succès, 0 en cas d'erreur
*/
int	add_redirection_to_cmd(t_cmd *cmd, t_token *current)
{
	t_file_redir	*new;
	t_file_redir	*last;

	new = create_redir(current);
	if (!new)
		return (0);
	if (!cmd->type_redir)
		cmd->type_redir = new;
	else
	{
		last = cmd->type_redir;
		while (last->next)
			last = last->next;
		last->next = new;
	}
	return (1);
}
