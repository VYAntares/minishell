/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/05 02:29:56 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Recherche une variable d'environnement par son nom
 * 
 * @param env Liste d'environnement
 * @param name Nom de la variable
 * @return char* Valeur de la variable ou NULL si non trouvée
 */
char	*get_env_value(t_env *env, const char *name)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strncmp(current->name, name, ft_strlen(name)) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

int	expand_env_var(t_token_word *token_word, t_shell *shell)
{
	int		i;
	char	*content;
	char	*env_name;
	char	*env_value;
	char	*exit_status;
	int		*pid;
	int		start;
	char	*new_content;
	char	*tmp1;
	char	*tmp2;

	i = 0;
	start = 0;
	content = token_word->content;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1])
		{
			start = ++i;
			if (content[i] == '$')
			{
				pid = ft_itoa(shell->pid);
				if (!pid)
					return (1);
				env_value = pid;
				i++;
			}
			else if (content[i] == '?')
			{
				exit_status = ft_itoa(shell->exit_status);
				if (!exit_status)
					return (1);
				env_value = exit_status;
				i++;
			}
			else
			{
				while (content[i] && ft_isalnum(content[i]) || content[i] == "_")
					i++;
				env_name = ft_substr(content, start, i - start);
				if (!env_name)
					return (1);
				env_value = get_env_value(shell->env, env_name);
				if (!env_value)
				{
					env_value = ft_strdup("");
					if (!env_value)
						return (1);
				}
			}
			new_content = ft_substr(content, 0, start - 1);
			if (!new_content)
				return (1);
			new_content = ft_strjoin(new_content, env_value);
			if (!new_content)
				return (1);
			new_content = ft_strjoin(new_content, content + i);
			if (!new_content)
				return (1);
			token_word->content = new_content;
			content = token_word->content;
			i = 0;
		}
		i++;
	}
	return (0);
}

int expand_var(t_cmd *cmd, t_shell *shell)
{
	int				i;
	t_token_word	*list;
	
	if (!cmd || !cmd->list_word)
		return (0);
	i = 0;
	while (i < cmd->ac && cmd->list_word[i])
	{
		list = cmd->list_word[i];
		while (list)
		{
			if (list->type != T_S_QUOTE)
			{
				if (expand_env_var(list, shell));
					return (1);
			}					
			list = list->next;
		}
		i++;
	}
}
