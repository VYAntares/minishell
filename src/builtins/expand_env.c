/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/19 16:58:24 by eahmeti          ###   ########.fr       */
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

char	*expand_env_heredoc(char *line, t_shell *shell)
{
	char	*expanded_line;
	int		i;
	int		start;

	i = 0;
	start = 0;
	expanded_line = NULL;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1] && line[i + 1] != ' ')
		{
			start = ++i;
			expanded_line = launch_expansion(line, shell, &i, &start);
			free(line);
			line = ft_strdup(expanded_line);
			if (!line)
				return (NULL);
			i = 0;
		}
		i++;
	}
	if (!expanded_line)
		return (line);
	return (expanded_line);
}

int	rebuild_redirection(t_cmd *cmd)
{
	char			*new_content;
	t_file_redir	*redir;
	t_token_word	*current;

	redir = cmd->type_redir;
	while (redir)
	{
		new_content = ft_strdup("");
		if (!new_content)
			return (1);
		current = redir->word_parts;
		while (current)
		{
			new_content = ft_strjoin(new_content, current->content);
			if (!new_content)
				return (1);
			current = current->next;
		}
		redir->content = ft_strdup(new_content);
		redir = redir->next;
		free(new_content);
	}
	return (0);
}

int	expand_redir_name(t_token_word *current,
					t_shell *shell,
					t_file_redir *redir)
{
	int	contains_dollar;

	contains_dollar = ft_strchr(current->content, '$') != NULL;
	if (current->type == T_NO_QUOTE && contains_dollar)
	{
		if (expand_env_var(current, shell) != 0)
			return (1);
		if (ft_strchr(current->content, ' ')
			|| ft_strchr(current->content, '\t'))
			redir->is_ambiguous = 1;
	}
	else if (current->type != T_S_QUOTE)
	{
		if (expand_env_var(current, shell) != 0)
			return (1);
	}
	return (0);
}

int	launch_redir_expansion(t_shell *shell, t_file_redir *redir)
{
	t_token_word	*current;

	redir->is_ambiguous = 0;
	if (redir->type_redirection == T_HEREDOC)
	{
		redir = redir->next;
		return (0);
	}
	current = redir->word_parts;
	while (current)
	{
		if (expand_redir_name(current, shell, redir) != 0)
			return (1);
		current = current->next;
	}
	return (0);
}

int	expand_redir(t_cmd *cmd, t_shell *shell)
{
	t_file_redir	*redir;

	redir = cmd->type_redir;
	while (redir)
	{
		if (launch_redir_expansion(shell, redir) != 0)
			return (1);
		redir = redir->next;
	}
	if (!cmd->type_redir->is_ambiguous
		&& cmd->type_redir->type_redirection != T_HEREDOC)
	{
		if (rebuild_redirection(cmd) != 0)
			return (1);
	}
	return (0);
}

int	expand_env_var(t_token_word *token_word, t_shell *shell)
{
	int		i;
	int		start;
	char	*content;
	char	*new_content;

	i = 0;
	start = 0;
	content = token_word->content;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1] && content[i + 1] != ' ')
		{
			start = ++i;
			new_content = launch_expansion(content, shell, &i, &start);
			token_word->content = new_content;
			content = token_word->content;
			i = 0;
		}
		i++;
	}
	return (0);
}

int	process_dollar(t_token_word	*current)
{
	char			*new_content;
	int				len;
	int				dollar_count;
	int				i;

	len = ft_strlen(current->content);
	if (len > 0 && current->content[len - 1] == '$')
	{
		dollar_count = 0;
		i = len - 1;
		while (i >= 0 && current->content[i] == '$')
		{
			dollar_count++;
			i--;
		}
		if (dollar_count % 2 != 0)
		{
			new_content = ft_substr(current->content, 0, len - 1);
			if (!new_content)
				return (1);
			free(current->content);
			current->content = new_content;
		}
	}
	return (0);
}

int	prepro_dol_redir(t_token_word *word_parts)
{
	t_token_word	*current;

	current = word_parts;
	while (current && current->next)
	{
		if (current->content && current->type == T_NO_QUOTE && current->next
			&& (current->next->type == T_D_QUOTE
				|| current->next->type == T_S_QUOTE))
		{
			if (process_dollar(current) != 0)
				return (1);
		}
		current = current->next;
	}
	return (0);
}

int	preprocess_dollar_quotes(t_token_word **head)
{
	t_token_word	*current;

	current = *head;
	while (current && current->next)
	{
		if (current->content && current->type == T_NO_QUOTE && current->next
			&& (current->next->type == T_D_QUOTE
				|| current->next->type == T_S_QUOTE))
		{
			if (process_dollar(current) != 0)
				return (1);
		}
		current = current->next;
	}
	return (0);
}

// Vérifier si l'argument contient des guillemets
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
	free(tmp);
	if (split_words)
		free_array(split_words);
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
		free(tmp);
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
		free(tmp);
		if (split_words)
			free_array(split_words);
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
			free(cmd->name);
		cmd->name = ft_strdup(cmd->arg[0]);
	}
	if (tmp_args)
	{
		i = 0;
		while (tmp_args[i])
		{
			free(tmp_args[i]);
			i++;
		}
		free(tmp_args);
	}
}

int	rebuild_command_arg(t_cmd *cmd)
{
	int				i;
	int				k;
	int				new_ac;
	char			**new_args;

	i = 0;
	new_ac = 0;
	new_args = NULL;
	while (i < cmd->ac)
	{
		if (should_we_split(cmd->list_word[i]) == 0)
			new_ac++;
		else
			new_ac += count_arg_after_split(cmd->list_word[i]);
		i++;
	}
	new_args = malloc(sizeof(char *) * (new_ac + 1));
	if (!new_args)
		return (1);
	k = 0;
	i = 0;
	while (i < cmd->ac)
		fill_new_args(cmd->list_word[i++], &k, new_args);
	replace_args_and_free(new_args, cmd, k, new_ac);
	return (0);
}

int	expand_var(t_cmd *cmd, t_shell *shell)
{
	t_token_word	*list;
	int				i;

	if (!cmd || !cmd->list_word)
		return (0);
	i = 0;
	if (cmd->type_redir && prepro_dol_redir(cmd->type_redir->word_parts) != 0)
		return (1);
	while (i < cmd->ac && cmd->list_word[i])
	{
		if (preprocess_dollar_quotes(&(cmd->list_word[i])) != 0)
			return (1);
		list = cmd->list_word[i];
		while (list)
		{
			if (list->type != T_S_QUOTE)
			{
				if (expand_env_var(list, shell) != 0)
					return (1);
			}
			list = list->next;
		}
		i++;
	}
	return (rebuild_command_arg(cmd));
}
