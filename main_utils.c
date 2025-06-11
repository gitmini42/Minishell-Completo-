/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:25:17 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/11 03:35:18 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_shell(t_shell *shell, char **envp)
{
	char	*shlvl_str;
	char	*new_shlvl;
	int		shlvl;

	shell->envp = copy_envp(envp);
	shell->vars = NULL;
	shell->exit_status = 0;
	g_signal = 0;
	shlvl_str = get_env_value(shell->envp, "SHLVL");
	if (shlvl_str)
		shlvl = ft_atoi(shlvl_str) + 1;
	else
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	if (new_shlvl)
	{
		export_var("SHLVL", new_shlvl, shell);
		free(new_shlvl);
	}
	set_signals_interactive();
}

void	finalize_shell(t_shell *shell)
{
	free_args(shell->envp, NULL);
	free_all_vars(&shell->vars);
	clear_history();
}

char	**cleanup_parse_data(t_parse_result *parsed, char **expanded_args,
		char **filtered_args)
{
	if (parsed->args)
		free_args(parsed->args, NULL);
	if (expanded_args)
		free_args(expanded_args, NULL);
	if (filtered_args)
		free_args(filtered_args, NULL);
	if (parsed->quote_types)
		free(parsed->quote_types);
	if (parsed->operator_flags)
		free(parsed->operator_flags);
	return (NULL);
}
