/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmb.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 17:51:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:59:24 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DMBGC_H
# define DMBGC_H

# include <stdlib.h>

typedef struct s_gc
{
	void		*ptr;
	struct s_gc	*next;
}	t_gc;

typedef enum GetHead
{
	GET = 0,
	REMOVE
}	t_get_head;

void	*dmb_malloc(size_t size);
void	dmb_gc(void);
void	dmb_free(void	*ptr);
void	dmb_force_free(void	*ptr);

#endif