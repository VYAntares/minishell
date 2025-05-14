/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_value.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:50:30 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/14 01:52:43 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*get_env_value(t_env *env, const char *name)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strlen(name) == ft_strlen(current->name)
			&& ft_strncmp(current->name, name, ft_strlen(name)) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

char	*get_int_value_of(int env, int *i)
{
	char	*env_value;

	env_value = ft_itoa(env);
	if (!env_value)
		return (NULL);
	(*i)++;
	return (env_value);
}

char	*extract_env_value(char	*line, t_shell *shell, int *i, int *start)
{
	char	*env_name;
	char	*env_value;

	while (line[(*i)] && (ft_isalnum(line[(*i)]) || line[(*i)] == '_'))
		(*i)++;
	env_name = ft_substr(line, (*start), (*i) - (*start));
	if (!env_name)
		return (NULL);
	env_value = get_env_value(shell->env, env_name);
	if (!env_value)
	{
		env_value = ft_strdup("");
		if (!env_value)
			return (NULL);
	}
	return (env_value);
}

char	*expand_line(char *line, char *env_value, int start, int i)
{
	char	*expanded_line;

	expanded_line = ft_substr(line, 0, start - 1);
	if (!expanded_line)
		return (NULL);
	expanded_line = ft_strjoin(expanded_line, env_value);
	if (!expanded_line)
		return (NULL);
	expanded_line = ft_strjoin(expanded_line, line + i);
	if (!expanded_line)
		return (NULL);
	return (expanded_line);
}

char	*launch_expansion(char *line, t_shell *shell, int *i, int *start)
{
	char	*env_value;
	char	*expanded_line;

	if (line[*i] == '$')
		env_value = get_int_value_of(shell->pid, i);
	else if (line[*i] == '?')
		env_value = get_int_value_of(shell->exit_status, i);
	else
		env_value = extract_env_value(line, shell, i, start);
	expanded_line = expand_line(line, env_value, *start, *i);
	if (!expanded_line)
		return (NULL);
	return (expanded_line);
}
