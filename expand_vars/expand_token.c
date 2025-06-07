/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 04:46:00 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/07 01:22:02 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static char	*process_mixed_quotes_token(char *token, t_shell *shell)
{
	char	*expanded;

	expanded = expand_mixed_quotes(token, shell);
	if (!expanded)
		expanded = ft_strdup("");
	return (expanded);
}

static char	*process_regular_token(char *token, char quote_type, t_shell *shell)
{
	char	*expanded;
	char	*tmp;

	if (quote_type != '\'')
		expanded = expand_variables(token, quote_type, shell);
	else
		expanded = ft_strdup(token);
	if (!expanded)
		expanded = ft_strdup(token);
	tmp = expanded;
	expanded = remove_quotes(tmp);
	free(tmp);
	if (!expanded)
		expanded = ft_strdup("");
	if (quote_type != '\'')
	{
		tmp = expanded;
		expanded = expand_tilde(tmp, shell);
		free(tmp);
	}
	return (expanded);
}

static char	*expand_single_token(char *token, char quote_type, t_shell *shell,
		int is_heredoc)
{
	if (is_heredoc)
		return (ft_strdup(token));
	if (has_mixed_quotes(token))
		return (process_mixed_quotes_token(token, shell));
	else
		return (process_regular_token(token, quote_type, shell));
}

static int	is_heredoc_delimiter(char **tokens, int index)
{
	if (index > 0 && ft_strcmp(tokens[index - 1], "<<") == 0)
		return (1);
	return (0);
}

static int	is_assignment(const char *token)
{
	int	i;

	if (!token)
		return (0);
	i = 0;
	while (token[i] && token[i] != '=')
	{
		if (!ft_isalnum(token[i]) && token[i] != '_')
			return (0);
		i++;
	}
	return (token[i] == '=');
}

static int	should_split_this_token(char **tokens, char *quote_types, int i,
		char *expanded_token)
{
	if (is_assignment(tokens[i]))
		return (0);
	if (should_split_token(quote_types[i], expanded_token))
		return (1);
	if (has_mixed_quotes(tokens[i])
		&& should_split_mixed_quotes(tokens[i], expanded_token))
		return (1);
	return (0);
}

static int	count_expanded_tokens(char **tokens,
		char *quote_types, t_shell *shell)
{
	int		total_count;
	int		i;
	char	*expanded_token;
	char	**split_words;
	int		word_count;

	total_count = 0;
	i = 0;
	while (tokens[i])
	{
		expanded_token = expand_single_token(tokens[i], quote_types[i], shell,
				is_heredoc_delimiter(tokens, i));
		if (should_split_this_token(tokens, quote_types, i, expanded_token))
		{
			split_words = split_expanded_token(expanded_token);
			if (split_words)
			{
				word_count = 0;
				while (split_words[word_count])
					word_count++;
				total_count += word_count;
				while (--word_count >= 0)
					free(split_words[word_count]);
				free(split_words);
			}
			else
				total_count++;
		}
		else
			total_count++;
		free(expanded_token);
		i++;
	}
	return (total_count);
}

static void	add_split_tokens(char ***expanded, int *out_idx,
		char *expanded_token)
{
	char	**split_words;
	int		j;

	split_words = split_expanded_token(expanded_token);
	if (split_words)
	{
		j = 0;
		while (split_words[j])
		{
			(*expanded)[(*out_idx)++] = ft_strdup(split_words[j]);
			j++;
		}
		j = 0;
		while (split_words[j])
			free(split_words[j++]);
		free(split_words);
	}
	else
		(*expanded)[(*out_idx)++] = ft_strdup(expanded_token);
}

char	**expand_tokens(char **tokens, char *quote_types, t_shell *shell)
{
	char		**expanded;
	char		*expanded_token;
	int			total_count;
	int			i;
	int			out_idx;

	if (!tokens || !tokens[0])
		return (NULL);
	total_count = count_expanded_tokens(tokens, quote_types, shell);
	expanded = malloc(sizeof(char *) * (total_count + 1));
	if (!expanded)
		return (NULL);
	i = 0;
	out_idx = 0;
	while (tokens[i])
	{
		expanded_token = expand_single_token(tokens[i], quote_types[i], shell,
				is_heredoc_delimiter(tokens, i));
		if (should_split_this_token(tokens, quote_types, i, expanded_token))
			add_split_tokens(&expanded, &out_idx, expanded_token);
		else
			expanded[out_idx++] = ft_strdup(expanded_token);
		free(expanded_token);
		i++;
	}
	expanded[out_idx] = NULL;
	return (expanded);
}
