/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:49:47 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 17:51:23 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_gc	*get_head(t_get_head remove)
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

static t_gc	*get_tail(t_gc *head)
{
	if (!head)
		return (head);
	while (head->next)
		head = head->next;
	return (head);
}

static t_gc	*append_node(void *ptr, t_gc *head)
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

// stores the pointer in a linked list for easy freeing down the line
void	*dmb_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	append_node(ptr, get_head(GET));
	return (ptr);
}

// frees everything allocated with dmb, poor mans garbace collector.
void	dmb_gc(void)
{
	t_gc	*next;
	t_gc	*head;

	head = get_head(GET);
	next = NULL;
	while (head)
	{
		if (head->ptr)
		{
			free(head->ptr);
			head->ptr = NULL;
		}
		next = head->next;
		free(head);
		head = next;
	}
	get_head(REMOVE);
}

// Will not free pointers allocated without dmb, immune to double free
void	dmb_free(void *ptr)
{
	t_gc	*tail;
	t_gc	*prev;

	tail = get_head(GET);
	prev = get_head(GET);
	while (tail)
	{
		if (tail->ptr && tail->ptr == ptr)
		{
			free(ptr);
			tail->ptr = NULL;
			prev->next = tail->next;
			free(tail);
			return ;
		}
		prev = tail;
		tail = tail->next;
	}
	return ;
}

// will free ptr even if it doesnt find it in the list.
// Useful if you gain ownership from pointers allocated outside of dmb
// not immune to double free
void	dmb_force_free(void	*ptr)
{
	t_gc	*tail;
	t_gc	*prev;

	tail = get_head(GET);
	prev = get_head(GET);
	while (tail)
	{
		if (tail->ptr && tail->ptr == ptr)
		{
			free(ptr);
			tail->ptr = NULL;
			prev->next = tail->next;
			free(tail);
			return ;
		}
		prev = tail;
		tail = tail->next;
	}
	free(ptr);
	return ;
}
