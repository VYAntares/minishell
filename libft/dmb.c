/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 17:38:28 by lfaure            #+#    #+#             */
/*   Updated: 2025/05/13 23:33:30 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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
void	dmb_free(void	*ptr)
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
