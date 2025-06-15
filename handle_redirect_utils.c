/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirect_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 11:19:57 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/15 11:40:53 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_input_redirect(t_redirect_params *params, t_shell *shell)
{
	//int check:
	
	//check = 0;
	if (!shell->is_counting)
	{
		process_input_file(params, shell);
		update_input_files(params, shell);
	}
	params->indices->i += 2;
	return (0);
}

void	handle_redirect(t_redirect_context *ctx)
{
	const char			*token;
	t_redirect_params	params;
	t_output_params		out_params;

	token = ctx->args[ctx->indices->i];
	if (validate_redirect_syntax(ctx->args, ctx->indices, ctx->shell) == -1)
		return ;
	params.args = ctx->args;
	params.data = ctx->data;
	params.indices = ctx->indices;
	params.cmd_index = ctx->cmd_index;
	if (ft_strcmp(token, "<") == 0)
		handle_input_redirect(&params, ctx->shell);
	else if (ft_strcmp(token, ">") == 0 || ft_strcmp(token, ">>") == 0)
	{
		out_params.base = params;
		out_params.token = token;
		handle_output_redirect(&out_params, ctx->shell);
	}
	else
	{
		print_error_simple("syntax error near redirect", 2, ctx->shell);
		ctx->indices->i++;
	}
}

int	set_redirection_data(t_output_params *params)
{
	int		idx;
	char	*filename;

	idx = params->base.data->num_out_redirs[params->base.cmd_index];
	filename = process_filename_token(params->base.args[params->base.indices->i
			+ 1]);
	params->base.data->out_redirs[params->base.cmd_index][idx].file = filename;
	if (ft_strcmp(params->token, ">>") == 0)
		params->base.data->out_redirs[params->base.cmd_index][idx].append = 1;
	else
		params->base.data->out_redirs[params->base.cmd_index][idx].append = 0;
	if (!filename)
		return (-1);
	params->base.data->num_out_redirs[params->base.cmd_index]++;
	return (0);
}

int	process_input_file(t_redirect_params *params, t_shell *shell)
{
	int		fd;
	char	*filename;

	//printf("%dis safe to open\n", shell->is_safe_to_open);
	if (shell->exit_status == 0 || shell->is_safe_to_open == 0)
	{
		filename = process_filename_token(params->args[params->indices->i + 1]);
		if (!filename)
			return (-1);
		fd = open(filename, O_RDONLY);
		if (fd < 0)
		{
			if (shell->valid_flag == 0)
			{
				ft_putstr_fd("mini1shell: ", 2);
				perror(filename);
				shell->valid_flag = 1;
			}
			shell->exit_status = 1;
			shell->is_safe_to_open = 1;
			free(filename);
			return (1);
		}
		else
			close(fd);
		free(filename);
	}
	return (0);
}

int	update_input_files(t_redirect_params *params, t_shell *shell)
{
	char	*filename;

/* 	if (check == 1)
		cleanup_command_data */
	if (params->data->input_files)
	{
		free(params->data->input_files[params->cmd_index]);
		filename = process_filename_token(params->args[params->indices->i + 1]);
		params->data->input_files[params->cmd_index] = filename;
		if (!filename)
		{
			if (shell->exit_status != 2)
				shell->exit_status = 1;
		}
	}
	return (0);
}
