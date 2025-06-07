/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:19:34 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/07 01:55:58 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// d is data
void	handle_heredoc(char **args, t_command_data *d, t_indices *indices,
	t_shell *shell)
{
	if (validate_heredoc_args(args, indices, shell) == -1)
		return ;
	if (d && d->heredoc_delim)
	{
		free(d->heredoc_delim);
		d->heredoc_delim = NULL;
	}
	set_heredoc_delimiter(d, args[indices->i + 1]);
	if (d->heredoc_delim == NULL)
	{
		shell->exit_status = 1;
		indices->i += 2;
		return ;
	}
	d->heredoc_fd = setup_heredoc(d->heredoc_delim, shell, d->heredoc_quoted);
	if (d->heredoc_fd == -1)
	{
		if (g_signal == SIGINT)
		{
			shell->exit_status = 130;
			g_signal = 0;
		}
		else
			shell->exit_status = 1;
	}
	indices->i += 2;
}
