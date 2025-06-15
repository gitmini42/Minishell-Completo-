/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_command_pipeline.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 22:48:34 by pviegas-          #+#    #+#             */
/*   Updated: 2025/06/15 11:01:59 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	has_failed_heredoc(t_command_data *data)
{
	int	i;

	if (!data->heredoc_delims || !data->heredoc_fds)
		return (0);
	i = 0;
	while (i < data->num_commands)
	{
		if (data->heredoc_delims[i] && data->heredoc_fds[i] == -1)
			return (1);
		i++;
	}
	return (0);
}

static int	should_skip_execution(t_command_data *data, t_shell *shell)
{
	int	i;

	if (shell->is_safe_to_open == 1)
		return (0);
	if (shell->exit_status == 2)
		return (0);
	i = 0;
	while (i < data->num_commands)
	{
		if ((!data->commands[i] || !data->commands[i][0]) &&
			(!data->out_redirs || !data->out_redirs[i]
				|| data->num_out_redirs[i] == 0) &&
			(!data->input_files || !data->input_files[i]) &&
			(!data->heredoc_delims || !data->heredoc_delims[i]))
		{
			shell->exit_status = 0;
			return (1);
		}
		if (data->commands[i] && data->commands[i][0]
			&& validate_command(&data->commands[i], shell) == 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

/* void	print_command_data_struct(t_command_data *data)
{
	int	i;
	int	j;

	i = 0;
	while (i < data->num_commands)
	{
		printf("Command[%d]: %s\n", i, data->commands[i]);
		if (data->arguments && data->arguments[i])
		{
			j = 0;
			while (data->arguments[i][j])
			{
				printf("  Arg[%d][%d]: %s\n", i, j, data->arguments[i][j]);
				j++;
			}
		}
		if (data->input_files && data->input_files[i])
			printf("  Input file[%d]: %s\n", i, data->input_files[i]);
		i++;
	}
} */

void	execute_command_pipeline(t_command_data *data, t_shell *shell)
{
	if (has_failed_heredoc(data))
	{
		cleanup_command_data(data);
		return ;
	}
	if (should_skip_execution(data, shell))
	{
		cleanup_command_data(data);
		return ;
	}
	//print_command_data_struct(data);
	prepare_command_data_for_execution(data);
	execute_commands(data, shell);
	cleanup_command_data(data);
}
