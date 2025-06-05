/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_expanded_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scarlos- <scarlos-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/06 07:01:36 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/05 20:30:12 by scarlos-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	fill_quotes(char *dest, const char *src, t_indices *indices,
	char quote_type)
{
	indices->i++;
	while (src[indices->i] && src[indices->i] != quote_type)
	{
		dest[indices->j++] = src[indices->i++];
	}
	if (src[indices->i] == quote_type)
		indices->i++;
	dest[indices->j] = '\0';
}

void	fill_exit_status(char *dest, t_indices *indices, t_shell *shell)
{
	size_t	len;

	len = handle_exit_status(&dest[indices->j], 1, &indices->i, shell);
	indices->j += len;
}

static void	handle_invalid_var(char *dest, t_indices *indices)
{
	dest[indices->j++] = '$';
}

void	fill_var_name(char *dest, const char *src, t_indices *indices,
	t_shell *shell)
{
	size_t	start;
	char	*key;
	char	*val;
	int		k;

	if (!dest || !src || !indices || !shell)
		return ;

	indices->i++;
	start = indices->i;

	if (!isalpha(src[start]) && src[start] != '_')
		return (handle_invalid_var(dest, indices));

	while (isalnum(src[indices->i]) || src[indices->i] == '_')
		indices->i++;

	if (indices->i == start)
		return (handle_invalid_var(dest, indices));

	key = ft_strndup(&src[start], indices->i - start);
	if (!key)
		return ;
	printf("dest %s\n", dest);
	printf("src %s\n", src);
	val = get_var_value_helper(key, shell->vars, shell->envp);
	if (val)
	{
		// Skip leading spaces
		while (*val == ' ')
			val++;

		k = 0;
		while (val[k])
		{
			// Copy character
			if (!(val[k] == ' ' && val[k + 1] == ' '))
			{
				dest[indices->j++] = val[k];
			}

			k++;
		}
		while (indices->j > 0 && dest[indices->j - 1] == ' ')
			indices->j--;
	}

	free(key);
}
