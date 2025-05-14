/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_entry.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:20:37 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:21:25 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

t_ast	*parse_tokens(t_token *tokens)
{
	t_ast		*ast;

	if (!tokens)
		return (NULL);
	if (!verify_parenthesis(tokens))
		ast = parse_logical_ops(tokens);
	else
		ast = parse_parenthesis(tokens);
	if (!ast)
		return (dmb_free(ast), NULL);
	return (ast);
}
