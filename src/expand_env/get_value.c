/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_value.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 01:50:30 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/25 21:29:33 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Recherche une variable d'environnement par son nom
** Parcourt la liste chaînée des variables d'environnement
** Retourne la valeur correspondante ou NULL si non trouvée
*/
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

/*
** Convertit un entier en chaîne de caractères pour l'expansion
** Utilisé pour $$ (PID) et $? (exit status)
** Incrémente l'index et retourne la valeur convertie
*/
char	*get_int_value_of(int env, int *i)
{
	char	*env_value;

	env_value = ft_itoa(env);
	if (!env_value)
		return (NULL);
	(*i)++;
	return (env_value);
}

/*
** Extrait le nom d'une variable d'environnement depuis une ligne
** Avance l'index jusqu'à la fin du nom (alphanum + underscore)
** Retourne la valeur de la variable ou chaîne vide si inexistante
*/
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

/*
** Reconstruit une ligne en remplaçant une variable par sa valeur
** Concatène : début de ligne + valeur expandée + fin de ligne
** Retourne la nouvelle ligne ou NULL en cas d'erreur
*/
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

/*
** Point d'entrée principal pour l'expansion des variables
** Gère les cas spéciaux $$ et $? puis les variables normales
** Retourne la ligne avec la variable expandée
*/
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
