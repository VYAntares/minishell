/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/15 18:31:26 by eahmeti          ###   ########.fr       */
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

char	*expand_env_heredoc(char *line, t_shell *shell)
{
	char	*expanded_line;
	char	*env_name;
	char	*env_value;
	int		i;
	int		start;

	i = 0;
	start = 0;
	expanded_line = NULL;
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1])
		{
			start = ++i;
			if (line[i] == '$')
			{
				env_value = ft_itoa(shell->pid);
				if (!env_value)
					return (NULL);
				i++;
			}
			else if (line[i] == '?')
			{
				env_value = ft_itoa(shell->exit_status);
				if (!env_value)
					return (NULL);
				i++;
			}
			else
			{
				while (line[i] && (ft_isalnum(line[i]) || line[i] == '_'))
					i++;
				env_name = ft_substr(line, start, i - start);
				if (!env_name)
					return (NULL);
				env_value = get_env_value(shell->env, env_name);
				if (!env_value)
				{
					env_value = ft_strdup("");
					if (!env_value)
						return (NULL);
				}
			}
			expanded_line = ft_substr(line, 0, start - 1);
			if (!expanded_line)
				return (NULL);
			expanded_line = ft_strjoin(expanded_line, env_value);
			if (!expanded_line)
				return (NULL);
			expanded_line = ft_strjoin(expanded_line, line + i);
			if (!expanded_line)
				return (NULL);
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

	new_content = ft_strdup("");
	redir = cmd->type_redir;
	current = redir->word_parts;
	if (!new_content)
		return (1);
	while (redir)
	{
		while (current)
		{
			new_content = ft_strjoin(new_content, current->content);
			if (!new_content)
				return (1);
			current = current->next;
		}
		cmd->type_redir->content = new_content;
		redir = redir->next;
	}
	return (0);
}

int expand_redir(t_cmd *cmd, t_shell *shell)
{
	t_file_redir	*redir;
	t_token_word	*current;

	redir = cmd->type_redir;
	while (redir)
	{
		current = redir->word_parts;
		while (current)
		{
			if (current->type != T_S_QUOTE
				&& redir->type_redirection != T_HEREDOC)
			{
				if (expand_env_var(current, shell) != 0)
					return (1);
			}
			current = current->next;
		}
		redir = redir->next;
	}
	if (cmd->type_redir->type_redirection != T_HEREDOC)
	{
		if (rebuild_redirection(cmd) != 0)
			return (1);
	}
	return (0);
}










int	expand_env_var(t_token_word *token_word, t_shell *shell)
{
	int		i;
	char	*content;
	char	*env_name;
	char	*env_value;
	int		start;
	char	*new_content;

	i = 0;
	start = 0;
	content = token_word->content;
	while (content[i])
	{
		if (content[i] == '$' && content[i + 1] && content[i + 1] != ' ')
		{
			start = ++i;
			if (content[i] == '$')
			{
				env_value = ft_itoa(shell->pid);
				if (!env_value)
					return (1);
				i++;
			}
			else if (content[i] == '?')
			{
				env_value = ft_itoa(shell->exit_status);
				if (!env_value)
					return (1);
				i++;
			}
			else
			{
				while (content[i] && (ft_isalnum(content[i]) || content[i] == '_'))
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

int preprocess_dollar_quotes(t_token_word **head)
{
    t_token_word    *current;
    t_token_word    *to_free;
    char            *new_content;
    int             len;
    int             dollar_count;

    current = *head;
    while (current && current->next)
    {
        // Cas 1: Token contenant uniquement "$"
        if (current->content && current->content[0] == '$' && !current->content[1] 
            && current->type == T_NO_QUOTE && current->next 
            && (current->next->type == T_D_QUOTE || current->next->type == T_S_QUOTE))
        {
            to_free = current;
            if (current == *head)
                *head = current->next;
            else
            {
                // Trouver le token précédent pour ajuster les liens
                t_token_word *prev = *head;
                while (prev && prev->next != current)
                    prev = prev->next;
                if (prev)
                    prev->next = current->next;
            }
            current = current->next;
            free(to_free->content);
            free(to_free);
        }
        // Cas 2: Token se terminant par un ou plusieurs "$"
        else if (current->content && current->type == T_NO_QUOTE && current->next
            && (current->next->type == T_D_QUOTE || current->next->type == T_S_QUOTE))
        {
            len = ft_strlen(current->content);
            if (len > 0 && current->content[len - 1] == '$')
            {
                // Compter les dollars consécutifs à la fin
                dollar_count = 0;
                int i = len - 1;
                while (i >= 0 && current->content[i] == '$')
                {
                    dollar_count++;
                    i--;
                }
                
                // Si le nombre de dollars est impair, supprimer le dernier
                if (dollar_count % 2 != 0)
                {
                    new_content = ft_substr(current->content, 0, len - 1);
                    if (!new_content)
                        return (1);
                    free(current->content);
                    current->content = new_content;
                }
            }
            current = current->next;
        }
        else
            current = current->next;
    }
    return (0);
}

/* 
 * Fonction pour reconstruire les arguments de la commande après expansion
 * Cette fonction remplace rebuild_command_arg et gère correctement 
 * les espaces dans les variables selon qu'elles sont entre guillemets ou non
 */
int rebuild_command_arg(t_cmd *cmd)
{
    int i, j, k;
    int new_ac = 0;
    char **new_args = NULL;
    t_token_word *word;
    char *tmp;
    char **split_words;
    char **tmp_args;
    
    // Première passe: compter le nombre total d'arguments après expansion
    for (i = 0; i < cmd->ac; i++)
    {
        int has_quote = 0;
        int will_split = 0;
        
        // Vérifier si l'argument contient des guillemets
        word = cmd->list_word[i];
        while (word)
        {
            if (word->type == T_S_QUOTE || word->type == T_D_QUOTE)
                has_quote = 1;
            
            // Si c'est un NO_QUOTE et contient des espaces
            if (word->type == T_NO_QUOTE && (ft_strchr(word->content, ' ') || 
                                             ft_strchr(word->content, '\t')))
                will_split = 1;
            
            word = word->next;
        }
        
        if (has_quote || !will_split)
            new_ac++; // Un seul argument
        else
        {
            // Compter combien d'arguments seront créés après la division
            tmp = ft_strdup("");
            word = cmd->list_word[i];
            
            while (word)
            {
                char *new_tmp = ft_strjoin(tmp, word->content);
                free(tmp);
                tmp = new_tmp;
                word = word->next;
            }
            
            split_words = ft_split(tmp, ' ');
            j = 0;
            while (split_words && split_words[j])
                j++;
                
            new_ac += j; // Ajouter le nombre de mots après division
            
            free(tmp);
            if (split_words)
                free_array(split_words);
        }
    }
    
    // Allouer le nouveau tableau d'arguments
    new_args = malloc(sizeof(char *) * (new_ac + 1));
    if (!new_args)
        return (1);
    
    // Deuxième passe: remplir le nouveau tableau d'arguments
    k = 0;
    for (i = 0; i < cmd->ac; i++)
    {
        int has_quote = 0;
        int will_split = 0;
        
        // Vérifier si l'argument contient des guillemets
        word = cmd->list_word[i];
        while (word)
        {
            if (word->type == T_S_QUOTE || word->type == T_D_QUOTE)
                has_quote = 1;
            
            // Si c'est un NO_QUOTE et contient des espaces
            if (word->type == T_NO_QUOTE && (ft_strchr(word->content, ' ') || 
                                             ft_strchr(word->content, '\t')))
                will_split = 1;
            
            word = word->next;
        }
        
        if (has_quote || !will_split)
        {
            // Traiter comme un seul argument
            tmp = ft_strdup("");
            word = cmd->list_word[i];
            
            while (word)
            {
                char *new_tmp = ft_strjoin(tmp, word->content);
                free(tmp);
                tmp = new_tmp;
                word = word->next;
            }
            
            new_args[k++] = tmp;
        }
        else
        {
            // Diviser l'argument en plusieurs selon les espaces
            tmp = ft_strdup("");
            word = cmd->list_word[i];
            
            while (word)
            {
                char *new_tmp = ft_strjoin(tmp, word->content);
                free(tmp);
                tmp = new_tmp;
                word = word->next;
            }
            
            split_words = ft_split(tmp, ' ');
            j = 0;
            while (split_words && split_words[j])
            {
                new_args[k++] = ft_strdup(split_words[j]);
                j++;
            }
            
            free(tmp);
            if (split_words)
                free_array(split_words);
        }
    }
    
    new_args[k] = NULL;
    
    // Libérer l'ancien tableau d'arguments
    tmp_args = cmd->arg;
    cmd->arg = new_args;
    cmd->ac = new_ac;
    
    // Mettre à jour le nom de la commande si nécessaire
    if (cmd->ac > 0)
    {
        if (cmd->name)
            free(cmd->name);
        cmd->name = ft_strdup(cmd->arg[0]);
    }
    
    // Libérer l'ancien tableau
    if (tmp_args)
    {
        for (i = 0; tmp_args[i]; i++)
            free(tmp_args[i]);
        free(tmp_args);
    }
    
    return (0);
}

/*
 * Version modifiée de expand_var qui utilise la fonction rebuild_command_arg améliorée
 */
int expand_var(t_cmd *cmd, t_shell *shell)
{
    int i;
    t_token_word *list;
    
    if (!cmd || !cmd->list_word)
        return (0);
    
    i = 0;
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
    
    // Utiliser la fonction améliorée de rebuild_command_arg
    return (rebuild_command_arg(cmd));
}

// int	rebuild_command_arg(t_cmd *cmd)
// {
// 	int				i;
// 	t_token_word	*word;
// 	char			*tmp;
// 	char			*new_tmp;

// 	i = 0;
// 	if (!cmd || !cmd->list_word)
// 		return (0);
// 	while (i < cmd->ac)
// 	{
// 		if (cmd->arg[i])
// 			free(cmd->arg[i]);
// 		tmp = ft_strdup("");
// 		if (!tmp)
// 			return (1);
// 		word = cmd->list_word[i];
// 		while (word)
// 		{
// 			new_tmp = ft_strjoin(tmp, word->content);
// 			free(tmp);
// 			if (!tmp)
// 				return (1);
// 			tmp = new_tmp;
// 			word = word->next;
// 		}
// 		cmd->arg[i] = tmp;
// 		i++;
// 	}
// 	if (cmd->ac > 0)
// 	{
// 		if (cmd->name)
// 			free(cmd->name);
// 		cmd->name = ft_strdup(cmd->arg[0]);
// 		if (!cmd->name)
// 			return (1);
// 	}
// 	return (0);
// }

// int expand_var(t_cmd *cmd, t_shell *shell)
// {
// 	int				i;
// 	t_token_word	*list;
	
// 	if (!cmd || !cmd->list_word)
// 		return (0);
// 	i = 0;
// 	while (i < cmd->ac && cmd->list_word[i])
// 	{
// 		if (preprocess_dollar_quotes(&(cmd->list_word[i])) != 0)
// 			return (1);
// 		list = cmd->list_word[i];
// 		while (list)
// 		{
// 			if (list->type != T_S_QUOTE)
// 			{
// 				if (expand_env_var(list, shell) != 0)
// 					return (1);
// 			}					
// 			list = list->next;
// 		}
// 		i++;
// 	}
// 	if (rebuild_command_arg(cmd) != 0)
// 		return (1);
// 	return (0);
// }
