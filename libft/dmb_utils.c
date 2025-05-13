/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmb_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:38:28 by lfaure            #+#    #+#             */
/*   Updated: 2025/05/13 23:34:21 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_gc	*get_head(t_get_head remove)
{
	static t_gc	*head;

	if (remove == REMOVE)
		return (head = NULL, NULL);
	if (!head)
	{
		head = malloc(sizeof(t_gc));
		if (!head)
			return (NULL);
		head->ptr = NULL;
		head->next = NULL;
	}
	return (head);
}

t_gc	*get_tail(t_gc *head)
{
	if (!head)
		return (head);
	while (head->next)
		head = head->next;
	return (head);
}

t_gc	*append_node(void *ptr, t_gc *head)
{
	t_gc	*tail;
	t_gc	*new_node;

	new_node = malloc(sizeof(t_gc));
	if (!new_node)
		return (head);
	new_node->next = NULL;
	new_node->ptr = NULL;
	tail = get_tail(get_head(GET));
	if (!tail)
		return (NULL);
	tail->next = new_node;
	new_node->ptr = ptr;
	return (head);
}
