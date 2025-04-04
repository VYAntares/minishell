/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:00:00 by eahmeti           #+#    #+#             */
/*   Updated: 2025/04/04 13:19:39 by eahmeti          ###   ########.fr       */
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

/**
 * @brief Parcourt les tokens et applique l'expansion de variables
 * 
 * @param tokens Liste de tokens
 * @param shell Structure shell
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
int	expand_env_vars(t_token *tokens, t_shell *shell)
{
	t_token			*current;
	t_token_word	*current_word;

	current = tokens;
	while (current)
	{
		// Pour les variables d'environnement directes
		if (current->type == T_ENV_VAR)
		{
			char *env_value = get_env_value(shell->env, current->value);
			char *old_value = current->value;
			
			// Cas spécial "$?"
			if (ft_strncmp(current->value, "?", 1) == 0)
			{
				current->value = ft_itoa(shell->exit_status);
				// printf("%s%c\n", current->value, 'c');
			}
			else if (env_value)
				current->value = ft_strdup(env_value);
			else
				current->value = ft_strdup("");
				
			if (!current->value)
				return (1);
			free(old_value);
		}
		// Pour les variables incluses dans des mots (ex: "Hello $USER")
		else if (current->type == T_WORD && current->type_word)
		{
			current_word = current->type_word;
			while (current_word)
			{
				if (current_word->type == T_D_QUOTE || current_word->type == T_NO_QUOTE)
				{
					// Rechercher "$" dans le contenu
					char *dollar_pos = ft_strchr(current_word->content, '$');
					if (dollar_pos)
					{
						// Cas complexe: variable dans une chaîne
						// Pour simplifier, on va juste détecter et gérer ce cas basique
						expand_var_in_string(current_word, shell);
					}
				}
				current_word = current_word->next;
			}
			
			// Reconstruire la valeur du token complet
			rebuild_token_value(current);
		}
		
		current = current->next;
	}
	
	return (0);
}

/**
 * @brief Expande une variable dans une chaîne (par ex. "Hello $USER")
 * 
 * @param token_word Token contenant la variable
 * @param shell Structure shell
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
int	expand_var_in_string(t_token_word *token_word, t_shell *shell)
{
	char	*content;
	char	*result;
	char	*var_name;
	char	*var_value;
	int		i;
	// int		j;
	int		start;

	content = token_word->content;
	result = ft_strdup("");
	if (!result)
		return (1);

	i = 0;
	while (content[i])
	{
		// Si on trouve un $, on cherche la variable
		if (content[i] == '$')
		{
			i++;  // Skip $
			
			// Cas spécial $?
			if (content[i] == '?')
			{
				char *tmp = ft_itoa(shell->exit_status);
				char *new_result = ft_strjoin(result, tmp);
				free(tmp);
				free(result);
				result = new_result;
				i++;
				continue;
			}
			
			// Identifier le nom de la variable
			start = i;
			while (content[i] && (ft_isalnum(content[i]) || content[i] == '_'))
				i++;
			
			if (start == i)  // $ suivi de rien d'utilisable
			{
				char *new_result = ft_strjoin(result, "$");
				free(result);
				result = new_result;
				continue;
			}
			
			// Extraire le nom de la variable
			var_name = ft_substr(content, start, i - start);
			if (!var_name)
			{
				free(result);
				return (1);
			}
			
			// Chercher la valeur
			var_value = get_env_value(shell->env, var_name);
			if (!var_value)
				var_value = "";
			
			// Ajouter au résultat
			char *new_result = ft_strjoin(result, var_value);
			free(result);
			free(var_name);
			result = new_result;
		}
		else
		{
			// Caractère normal, l'ajouter directement
			char new_char[2] = {content[i], '\0'};
			char *new_result = ft_strjoin(result, new_char);
			free(result);
			result = new_result;
			i++;
		}
	}
	
	// Remplacer le contenu
	free(token_word->content);
	token_word->content = result;
	
	return (0);
}

/**
 * @brief Reconstruit la valeur d'un token à partir de ses token_word
 * 
 * @param token Token à reconstruire
 * @return int 0 en cas de succès, 1 en cas d'erreur
 */
int	rebuild_token_value(t_token *token)
{
	t_token_word	*current;
	char			*result;
	char			*old_value;
	char			*tmp;

	if (!token || !token->type_word)
		return (0);
	
	result = ft_strdup("");
	if (!result)
		return (1);
	
	current = token->type_word;
	while (current)
	{
		tmp = ft_strjoin(result, current->content);
		free(result);
		if (!tmp)
			return (1);
		result = tmp;
		current = current->next;
	}
	
	old_value = token->value;
	token->value = result;
	free(old_value);
	
	return (0);
}