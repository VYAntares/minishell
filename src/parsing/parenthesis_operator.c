/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parenthesis_operator.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/11 01:04:45 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:14:50 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Convertit le type d'un token en type de nœud AST correspondant
** Retourne AST_AND, AST_OR, AST_PIPE selon le token, AST_CMD par défaut
*/
t_ast_type	get_ast_type(t_token *token)
{
	if (!token)
		return (AST_CMD);
	if (token->type == T_AND)
		return (AST_AND);
	if (token->type == T_OR)
		return (AST_OR);
	if (token->type == T_PIPE)
		return (AST_PIPE);
	return (AST_CMD);
}

/*
** Vérifie si le premier opérateur apparaît après le second dans la liste
** Parcourt depuis first_op pour chercher second_op
** Retourne 1 si first_op est le dernier, 0 sinon
*/
int	check_if_operator_is_last(t_token *first_op, t_token *second_op)
{
	t_token		*tmp;
	int			valid;

	if (!first_op)
		return (0);
	tmp = first_op;
	valid = 1;
	while (tmp)
	{
		if (tmp == second_op)
		{
			valid = 0;
			break ;
		}
		tmp = tmp->next;
	}
	if (valid)
		return (1);
	return (0);
}

/*
** Trouve les derniers opérateurs OR, AND et PIPE hors des parenthèses
** Maintient un compteur de parenthèses pour ignorer celles imbriquées
** Met à jour les pointeurs vers les derniers opérateurs trouvés
*/
void	point_last_operator_outside_parenthesis(t_token **or,
											t_token **and,
											t_token **pipe,
											t_token *current)
{
	int		parenth_count;

	parenth_count = 0;
	while (current)
	{
		if (current->type == T_PARENTH_OPEN)
			parenth_count++;
		else if (current->type == T_PARENTH_CLOSE)
			parenth_count--;
		else if (parenth_count == 0)
		{
			if (current->type == T_OR)
				*or = current;
			else if (current->type == T_AND)
				*and = current;
			else if (current->type == T_PIPE)
				*pipe = current;
		}
		current = current->next;
	}
}

/*
** Trouve l'opérateur de plus faible priorité le plus à droite
** Priorité : OR < AND < PIPE (évaluation de droite à gauche)
** Ignore les opérateurs à l'intérieur des parenthèses
** Retourne l'opérateur trouvé ou NULL si aucun
*/
t_token	*find_last_priority_operator(t_token *tokens)
{
	t_token		*current;
	t_token		*or;
	t_token		*and;
	t_token		*pipe;

	if (!tokens)
		return (NULL);
	or = NULL;
	and = NULL;
	pipe = NULL;
	current = tokens;
	point_last_operator_outside_parenthesis(&or, &and, &pipe, current);
	if (check_if_operator_is_last(or, and))
		return (or);
	if (check_if_operator_is_last(and, or))
		return (and);
	return (pipe);
}
