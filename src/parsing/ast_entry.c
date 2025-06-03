/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_entry.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 02:20:37 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 20:59:31 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Point d'entree du parsing : transforme les tokens en AST.
** Determine s'il faut traiter les parentheses ou les operateurs logiques.
** Retourne l'arbre syntaxique abstrait ou NULL en cas d'erreur.
*/
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
