/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rebuild_args.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:54:18 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:42:55 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Compte le nombre total d'arguments après expansion des wildcards
** Parcourt chaque argument et compte ses token_word individuels
** Chaque token_word devient un argument séparé après expansion
** Retourne le nouveau nombre d'arguments
*/
int	count_arg_after_expansion(t_cmd *cmd)
{
	int				i;
	int				new_ac;
	t_token_word	*current;

	i = 0;
	new_ac = 0;
	while (i < cmd->ac)
	{
		current = cmd->list_word[i];
		while (current)
		{
			new_ac++;
			current = current->next;
		}
		i++;
	}
	return (new_ac);
}

/*
** Crée le nouveau tableau d'arguments après expansion wildcards
** Alloue un tableau de taille exacte pour tous les nouveaux arguments
** Copie le contenu de chaque token_word comme argument séparé
** Retourne le nouveau tableau ou NULL en cas d'erreur
*/
char	**create_new_args_array(t_cmd *cmd, int new_ac)
{
	int				i;
	int				arg_index;
	char			**new_args;
	t_token_word	*current;

	i = 0;
	arg_index = 0;
	new_args = dmb_malloc(sizeof(char *) * (new_ac + 1));
	if (!new_args)
	{
		if (new_args)
			dmb_free(new_args);
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

/*
** Met à jour la structure de commande avec les nouveaux arguments
** Libère l'ancien tableau et remplace par le nouveau
** Met à jour le compteur d'arguments et le nom de commande
** Gère le cas où il n'y a plus d'arguments
*/
void	update_command(t_cmd *cmd, char **new_args, int new_ac)
{
	int	i;

	i = 0;
	while (i < cmd->ac)
	{
		if (cmd->arg && cmd->arg[i])
			dmb_free(cmd->arg[i++]);
	}
	if (cmd->arg)
		dmb_free(cmd->arg);
	cmd->arg = new_args;
	cmd->ac = new_ac;
	if (cmd->name)
		dmb_free(cmd->name);
	if (cmd->ac > 0)
		cmd->name = ft_strdup(cmd->arg[0]);
	else
		cmd->name = ft_strdup("");
}

/*
** Point d'entrée pour reconstruire les arguments après wildcards
** Compte les nouveaux arguments, crée le tableau et met à jour
** Retourne 0 en cas de succès (toujours réussi)
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
