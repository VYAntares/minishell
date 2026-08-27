/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 21:35:17 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 17:54:55 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	void	*temp;

	if (lst && del)
	{
		while (*lst)
		{
			temp = (*lst)->next;
			del((*lst)->content);
			dmb_free(*lst);
			*lst = temp;
		}
		*lst = NULL;
	}
}
