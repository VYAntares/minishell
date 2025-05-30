/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eahmeti <eahmeti@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 19:04:34 by eahmeti           #+#    #+#             */
/*   Updated: 2025/05/13 18:38:00 by eahmeti          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../../minishell.h"
//
// const char *TREE_VERTICAL = "│";
// const char *TREE_BRANCH = "├";
// const char *TREE_CORNER = "└";
// const char *TREE_HORIZONTAL = "──";
//
// 
// const char *ast_type_color(t_ast_type type)
// {
// 	switch (type)
// 	{
// 		case AST_CMD:		return "\033[38;5;114m";    /* Vert  */
// 		case AST_PIPE:		return "\033[38;5;75m";     /* Bleu  */
// 		case AST_AND:		return "\033[38;5;213m";    /* Magenta */
// 		case AST_OR:		return "\033[38;5;208m";    /* Cyan  */
// 		case AST_REDIR:		return "\033[33m";          /* Jaune */
// 		case AST_SUB_SHELL:	return "\033[38;5;203m";    /* Rouge */
// 		default:			return "\033[0m";           /* Reset */
// 	}
// }
//
// /* Fonctions de conversion des types (inchangées) */
// const char *token_type_to_str(t_type type)
// {
// 	switch (type)
// 	{
// 		case T_WORD:			return "WORD";
// 		case T_PIPE:			return "PIPE";
// 		case T_REDIR_IN:		return "REDIR_IN";
// 		case T_REDIR_OUT:		return "REDIR_OUT";
// 		case T_HEREDOC:			return "HEREDOC";
// 		case T_APPEND:			return "APPEND";
// 		case T_ENV_VAR:			return "ENV_VAR";
// 		case T_WILDCARD:		return "WILDCARD";
// 		case T_AND:				return "AND";
// 		case T_OR:				return "OR";
// 		case T_PARENTH_OPEN:	return "PARENTH_OPEN";
// 		case T_PARENTH_CLOSE:	return "PARENTH_CLOSE";
// 		default:				return "UNKNOWN";
// 	}
// }
//
// const char *token_word_type_to_str(t_type_word type)
// {
// 	switch (type)
// 	{
// 		case T_NO_QUOTE:	return "NO_QUOTE";
// 		case T_S_QUOTE:		return "SINGLE_QUOTE";
// 		case T_D_QUOTE:		return "DOUBLE_QUOTE";
// 		default:			return "UNKNOWN";
// 	}
// }
//
// const char *ast_type_to_str(t_ast_type type)
// {
// 	switch (type)
// 	{
// 		case AST_CMD:		return "COMMAND";
// 		case AST_PIPE:		return "PIPE";
// 		case AST_AND:		return "AND";
// 		case AST_OR:		return "OR";
// 		case AST_REDIR:		return "REDIRECTION";
// 		case AST_SUB_SHELL:	return "SUB_SHELL";
// 		default:			return "UNKNOWN";
// 	}
// }
//
// void debug_print_token_words(t_token_word *word_list,
// 									const char *indent)
// {
// 	t_token_word *current = word_list;
// 	while (current)
// 	{
// 		// On utilise "└──" pour chaque entrée de la liste,
// 		// mais c'est purement décoratif
// 		printf("%s%s%s Content: '%s' (Type: %s)\n",
// 			   indent,
// 			   TREE_CORNER,
// 			   TREE_HORIZONTAL,
// 			   current->content,
// 			   token_word_type_to_str(current->type));
// 		current = current->next;
// 	}
// }
//
// void	debug_print_cmd(t_cmd *cmd, const char *prefix)
// {
//     const char *redir_indent = "     ";
//     const char *parts_indent = "                                       ";
//    
//     printf("%sCommande:\n", prefix);
//
//     // Nom de la commande
//     printf("%s%s%s Nom: %s\n",
//            prefix, TREE_BRANCH, TREE_HORIZONTAL, cmd->name);
//
//     // Arguments 
// 	// (on utilise TREE_BRANCH si il y a des redirections, sinon TREE_CORNER)
//     printf("%s%s%s Arguments:\n",
//         prefix, cmd->type_redir ? TREE_BRANCH : TREE_CORNER,
//												TREE_HORIZONTAL);
//     int i = 0;
//     while (cmd->arg && cmd->arg[i])
//     {
//         printf("%s%s     %s%s arg[%d]: %s\n",
//                prefix, 
//                cmd->type_redir ? TREE_VERTICAL : "",
//                TREE_CORNER, TREE_HORIZONTAL,
//                i, cmd->arg[i]);
//
//         // Affichage des Token Words
//         if (cmd->list_word && cmd->list_word[i])
//         {
//             printf("%s%s          %s%s Token Words:\n",
//                    prefix,
//                    cmd->type_redir ? TREE_VERTICAL : "",
//                    TREE_CORNER, TREE_HORIZONTAL);
//             t_token_word *word = cmd->list_word[i];
//             while (word)
//             {
//                 printf("%s%s               %s%s Content: '%s' (Type: %s)\n",
//                        prefix,
//                        cmd->type_redir ? TREE_VERTICAL : "",
//                        TREE_CORNER, TREE_HORIZONTAL,
//                        word->content,
//                        token_word_type_to_str(word->type));
//                 word = word->next;
//             }
//         }
//         i++;
//     }
//
//     // Redirections
//     if (cmd->type_redir)
//     {
//         printf("%s%s\n%s%s%s Redirections:\n",
//                prefix, TREE_VERTICAL, prefix, TREE_CORNER, TREE_HORIZONTAL);
//         t_file_redir *current = cmd->type_redir;
//         while (current)
//         {
//             printf("%s %s%s%s Type: %s, Fichier complet: %s\n",
//                    prefix,
//                    redir_indent,
//                    TREE_CORNER, TREE_HORIZONTAL,
//                    token_type_to_str(current->type_redirection),
//                    current->content);
//            
//             // Afficher les parties du nom de fichier
//             if (current->word_parts)
//             {
//                 printf("%s%s %s%s Parties du fichier:\n",
//                        prefix,
//                        parts_indent,
//                        TREE_CORNER,
//                        TREE_HORIZONTAL);
//                 t_token_word *word = current->word_parts;
//                 while (word)
//                 {
//                     printf("%s%s                     %s%s '%s' (%s)\n",
//                            prefix,
//                            parts_indent,
//                            TREE_CORNER,
//                            TREE_HORIZONTAL,
//                            word->content,
//                            token_word_type_to_str(word->type));
//                     word = word->next;
//                 }
//             }
//             current = current->next;
//         }
//     }
// }
//
// void	debug_print_ast(t_ast *node, const char *prefix, bool is_last)
// {
// 	if (!node)
// 		return;
//
// 	if (node->type == AST_REDIR)
// 		return;
//
// 	// Affichage du préfixe suivi du symbole (└── si dernier, ├── sinon)
// 	printf("%s%s%s %sNoeud Type: %s%s\n",
// 		   prefix,
// 		   (is_last ? TREE_CORNER : TREE_BRANCH),
// 		   TREE_HORIZONTAL,
// 		   ast_type_color(node->type),
// 		   ast_type_to_str(node->type),
// 		   "\033[0m");
//
// 	char new_prefix[512];
// 	snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
// 			(is_last ? "    " : "│   "));
//
// 	// Si c'est un CMD, on affiche son contenu
// 	if (node->type == AST_CMD && node->cmd)
// 		debug_print_cmd(node->cmd, new_prefix);
//
// 	// Affichage du contenu des parenthèses
// 	if (node->sub_shell)
// 	{
// 		printf("%s%s%s Contenu du SUB_SHELL:\n%s%s\n",
// 			   new_prefix,
// 			   (node->left || node->right ? TREE_BRANCH : TREE_BRANCH),
// 			   TREE_HORIZONTAL,
// 			   new_prefix,
// 			   TREE_VERTICAL);
// 		debug_print_ast(node->sub_shell, new_prefix,
// 						!(node->left || node->right));
// 	}
// 	if (node->left)		// Sous-arbre gauche
// 	{
// 		bool left_is_last = (node->right == NULL);
// 		printf("%s%s\n%s%s%s \033[38;5;147mGauche:\033[0m\n",
// 			   new_prefix,
// 			   TREE_VERTICAL,
// 			   new_prefix,
// 			   (left_is_last ? TREE_CORNER : TREE_BRANCH),
// 			   TREE_HORIZONTAL);
// 		debug_print_ast(node->left, new_prefix, left_is_last);
// 	}
// 	if (node->right)	// Sous-arbre droit
// 	{
// 		printf("%s%s\n%s%s%s \033[38;5;123mDroite:\033[0m\n",
// 			   new_prefix,
// 			   TREE_VERTICAL,
// 			   new_prefix,
// 			   TREE_BRANCH,
// 			   TREE_HORIZONTAL);
// 		debug_print_ast(node->right, new_prefix, true);
// 	}
// }
//
// void debug_print_tokens(t_token *tokens)
// {
//     t_token *current = tokens;
//     int count = 0;
//
//     printf("\n=== DÉBUT DES TOKENS ===\n\n\n\n\n");
//     while (current)
//     {
//         printf("Token %d:\n", count++);
//         printf("%s%s Type: %s\n",
//                TREE_BRANCH, TREE_HORIZONTAL,
//                token_type_to_str(current->type));
//         printf("%s%s Value: '%s'\n",
//                TREE_BRANCH, TREE_HORIZONTAL,
//                current->value);
//
//         if (current->type == T_WORD && current->type_word)
//         {
//             printf("%s%s Token Words:\n",
//                    TREE_CORNER, TREE_HORIZONTAL);
//             debug_print_token_words(current->type_word, "        ");
//         }
//         printf("\n");
//         current = current->next;
//     }
//     printf("\n\n\n\n\n=== FIN DES TOKENS ===\n\n\n\n\n");
// }