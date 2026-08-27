/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:49:49 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:09:07 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Compte le nombre d'arguments d'une commande en ignorant les redirections.
** Saute les tokens de redirection et leurs fichiers associes.
** Retourne le nombre total d'arguments (commande + parametres).
*/
int	count_args(t_token *tokens)
{
	t_token		*current;
	int			count;

	count = 0;
	current = tokens;
	while (current)
	{
		if (is_redirection(current->type))
		{
			if (current->next)
				current = current->next->next;
			else
				current = NULL;
			continue ;
		}
		if (is_command_argument(current->type))
			count++;
		current = current->next;
	}
	return (count);
}

/*
** Initialise une structure t_cmd avec les champs de base.
** Alloue le tableau d'arguments selon le nombre calcule.
** Retourne 1 en cas de succes, 0 en cas d'erreur d'allocation.
*/
int	init_cmd(t_cmd *cmd, t_token *tokens)
{
	if (!tokens)
		return (0);
	cmd->ac = count_args(tokens);
	cmd->name = NULL;
	cmd->path = NULL;
	cmd->type_redir = NULL;
	cmd->list_word = NULL;
	cmd->next = NULL;
	cmd->arg = NULL;
	cmd->arg = dmb_malloc(sizeof(char *) * (cmd->ac + 1));
	if (!cmd->arg)
		return (0);
	return (1);
}

/*
** Ajoute un argument au tableau cmd->arg a l'index donne.
** Le premier argument (index 0) devient aussi cmd->name.
** Retourne 1 en cas de succes, 0 si duplication echoue.
*/
int	add_command_arg(t_cmd *cmd, t_token *current, int i)
{
	if (i == 0)
	{
		cmd->name = ft_strdup(current->value);
		if (!cmd->name)
			return (0);
	}
	cmd->arg[i] = ft_strdup(current->value);
	if (!cmd->arg[i])
		return (0);
	cmd->arg[i + 1] = NULL;
	return (1);
}

/*
** Sauvegarde les t_token_word pour chaque argument de la commande.
** Necessaire pour l'expansion des variables et des wildcards.
** Construit un tableau parallele a cmd->arg avec les metadonnees.
*/
void	track_word_tokens(t_cmd *cmd, t_token *tokens)
{
	t_token		*current;
	int			i;

	i = 0;
	current = tokens;
	cmd->list_word = dmb_malloc(sizeof(t_token_word) * (cmd->ac + 1));
	if (!cmd->list_word)
		return ;
	while (current && i < cmd->ac)
	{
		current = skip_redirection(current);
		if (!current)
			break ;
		if (is_command_argument(current->type))
		{
			cmd->list_word[i] = current->type_word;
			i++;
		}
		current = current->next;
	}
	cmd->list_word[i] = NULL;
}

/*
** Point d'entree pour creer une structure t_cmd complete.
** Compte, initialise, remplit les arguments et trace les tokens.
** Retourne la commande prete ou NULL en cas d'erreur.
*/
t_cmd	*create_command(t_token *tokens)
{
	t_token		*current;
	t_cmd		*cmd;
	int			i;

	if (!tokens)
		return (NULL);
	cmd = dmb_malloc(sizeof(t_cmd));
	if (!init_cmd(cmd, tokens) || !cmd)
		return (dmb_free(cmd), NULL);
	i = 0;
	current = tokens;
	while (current && i < cmd->ac)
	{
		current = skip_redirection(current);
		if (!current)
			break ;
		if (is_command_argument(current->type))
		{
			if (!add_command_arg(cmd, current, i++))
				return (NULL);
		}
		current = current->next;
	}
	track_word_tokens(cmd, tokens);
	return (cmd);
}
