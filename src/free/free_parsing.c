/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_parsing.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 17:38:50 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 17:45:52 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_tokens(t_token *tokens)
{
	t_token		*current;
	t_token		*next;

	current = tokens;
	while (current)
	{
		next = current->next;
		if (current->value)
		{
			free(current->value);
			current->value = NULL;
		}
		free(current);
		current = next;
	}
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	if (node->cmd && node->type != AST_REDIR)
	{
		free_cmd(node->cmd);
		node->cmd = NULL;
	}
	free(node);
}

void	free_word_parts(t_token_word *word_parts)
{
	t_token_word	*current;
	t_token_word	*next;

	current = word_parts;
	while (current)
	{
		next = current->next;
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}
}

void	free_cmd(t_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->name)
	{
		free(cmd->name);
		cmd->name = NULL;
	}
	if (cmd->path)
	{
		free(cmd->path);
		cmd->path = NULL;
	}
	if (cmd->arg)
	{
		i = 0;
		while (cmd->arg[i])
		{
			free(cmd->arg[i]);
			i++;
		}
		free(cmd->arg);
		cmd->arg = NULL;
	}
	if (cmd->type_redir)
	{
		t_file_redir *current = cmd->type_redir;
		t_file_redir *next;
		
		while (current)
		{
			next = current->next;
			if (current->content)
				free(current->content);
			if (current->word_parts)
				free_word_parts(current->word_parts);
			free(current);
			current = next;
		}
		cmd->type_redir = NULL;
	}
	free(cmd);
}

void	free_array(char **array)
{
	int		i;

	i = 0;
	if (!array)
		return ;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void	cleanup_heredoc_files(t_cmd *cmd)
{
	t_file_redir	*redir;
	t_cmd			*current;

	redir = cmd->type_redir;
	current = cmd;
	while (current)
	{
		while (redir)
		{
			if (redir->type_redirection == T_HEREDOC)
				unlink(redir->content);
			redir = redir->next;
		}
		current = current->next;
	}
}

void	free_env_list(t_env *head)
{
	t_env		*current;
	t_env		*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->name);
		free(current->value);
		free(current);
		current = next;
	}
}