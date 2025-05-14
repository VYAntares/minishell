/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:55:02 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 02:02:13 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	should_we_split(t_token_word *list_word)
{
	int				has_quote;
	int				will_split;
	t_token_word	*word;

	has_quote = 0;
	will_split = 0;
	word = list_word;
	while (word)
	{
		if (word->type == T_S_QUOTE || word->type == T_D_QUOTE)
			has_quote = 1;
		if (word->type == T_NO_QUOTE && (ft_strchr(word->content, ' ')
				|| ft_strchr(word->content, '\t')))
			will_split = 1;
		word = word->next;
	}
	if (has_quote || !will_split)
		return (0);
	return (1);
}

int	count_arg_after_split(t_token_word *list_word)
{
	t_token_word	*word;
	char			*tmp;
	char			**split_words;
	int				j;

	tmp = ft_strdup("");
	word = list_word;
	while (word)
	{
		tmp = ft_strjoin(tmp, word->content);
		word = word->next;
	}
	split_words = ft_split(tmp, ' ');
	j = 0;
	while (split_words && split_words[j])
		j++;
	dmb_free(tmp);
	return (j);
}

char	*rebuild_word(t_token_word *list_word)
{
	char			*tmp;
	char			*new_tmp;
	t_token_word	*word;

	tmp = ft_strdup("");
	word = list_word;
	while (word)
	{
		new_tmp = ft_strjoin(tmp, word->content);
		dmb_free(tmp);
		tmp = new_tmp;
		word = word->next;
	}
	return (tmp);
}

void	fill_new_args(t_token_word *list_word, int *k, char **new_args)
{
	int		j;
	char	*tmp;
	char	**split_words;

	j = 0;
	if (should_we_split(list_word) == 0)
	{
		tmp = rebuild_word(list_word);
		new_args[(*k)++] = tmp;
	}
	else
	{
		tmp = rebuild_word(list_word);
		split_words = ft_split(tmp, ' ');
		j = 0;
		while (split_words && split_words[j])
		{
			new_args[(*k)++] = ft_strdup(split_words[j]);
			j++;
		}
		dmb_free(tmp);
	}
}

void	replace_args_and_free(char **new_args, t_cmd *cmd, int k, int new_ac)
{
	int		i;
	char	**tmp_args;

	new_args[k] = NULL;
	tmp_args = cmd->arg;
	cmd->arg = new_args;
	cmd->ac = new_ac;
	if (cmd->ac > 0)
	{
		if (cmd->name)
			dmb_free(cmd->name);
		cmd->name = ft_strdup(cmd->arg[0]);
	}
	if (tmp_args)
	{
		i = 0;
		while (tmp_args[i])
		{
			dmb_free(tmp_args[i]);
			i++;
		}
		dmb_free(tmp_args);
	}
}
