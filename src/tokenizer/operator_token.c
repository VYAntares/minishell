/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operator_token.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 22:39:11 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 17:54:55 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	handle_double_operator(t_token *new, int *i, char *input)
{
	if (input[*i] == '&' && input[*i + 1] == '&')
	{
		new->type = T_AND;
		new->value = ft_strdup("&&");
		(*i) += 2;
		return (1);
	}
	else if (input[*i] == '|' && input[*i + 1] == '|')
	{
		new->type = T_OR;
		new->value = ft_strdup("||");
		(*i) += 2;
		return (1);
	}
	return (0);
}

int	handle_double_redir(t_token *new, int *i, char *input)
{
	if (input[*i] == '<' && input[*i + 1] == '<')
	{
		new->type = T_HEREDOC;
		new->value = ft_strdup("<<");
		(*i) += 2;
		return (1);
	}
	else if (input[*i] == '>' && input[*i + 1] == '>')
	{
		new->type = T_APPEND;
		new->value = ft_strdup(">>");
		(*i) += 2;
		return (1);
	}
	return (0);
}

int	handle_single_operator(t_token *new, int *i, char *input)
{
	if (input[*i] == '|')
	{
		new->type = T_PIPE;
		new->value = ft_strdup("|");
		(*i)++;
		return (1);
	}
	else if (input[*i] == '>')
	{
		new->type = T_REDIR_OUT;
		new->value = ft_strdup(">");
		(*i)++;
		return (1);
	}
	else if (input[*i] == '<')
	{
		new->type = T_REDIR_IN;
		new->value = ft_strdup("<");
		(*i)++;
		return (1);
	}
	return (0);
}

int	handle_wild_and_parenthesis(t_token *new, int *i, char *input)
{
	if (input[*i] == '*')
	{
		new->type = T_WILDCARD;
		new->value = ft_strdup("*");
		(*i)++;
		return (1);
	}
	else if (input[*i] == '(')
	{
		new->type = T_PARENTH_OPEN;
		new->value = ft_strdup("(");
		(*i)++;
		return (1);
	}
	else if (input[*i] == ')')
	{
		new->type = T_PARENTH_CLOSE;
		new->value = ft_strdup(")");
		(*i)++;
		return (1);
	}
	return (0);
}

int	add_operator_token(t_token **list, int *i, char *input)
{
	t_token	*new;

	new = dmb_malloc(sizeof(t_token));
	if (!new)
		return (0);
	new->prev = NULL;
	new->next = NULL;
	if (!(handle_double_operator(new, i, input))
		&& !(handle_double_redir(new, i, input))
		&& !(handle_single_operator(new, i, input))
		&& !(handle_wild_and_parenthesis(new, i, input)))
		return (dmb_free(new->value), dmb_free(new), 0);
	if (!new->value || !add_node_back(list, new))
		return (dmb_free(new->value), dmb_free(new), 0);
	return (1);
}
