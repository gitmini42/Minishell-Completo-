/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pviegas- <pviegas-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 11:40:55 by scarlos-          #+#    #+#             */
/*   Updated: 2025/06/07 01:07:56 by pviegas-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	init_heredoc_pipe(int *pipefd)
{
	int	result;

	result = pipe(pipefd);
	if (result == -1)
	{
		perror("pipe");
		return (-1);
	}
	return (0);
}

static char	*read_heredoc_line(void)
{
	char	*line;
	size_t	len;

	if (g_signal == SIGINT)
		return (NULL);
	write(1, "> ", 2);
	line = get_next_line(STDIN_FILENO);
	if (g_signal == SIGINT)
	{
		if (line)
			free(line);
		return (NULL);
	}
	if (line)
	{
		len = ft_strlen(line);
		if (len > 0 && line[len - 1] == '\n')
			line[len - 1] = '\0';
	}
	return (line);
}

static void	write_heredoc_line(int pipefd, char *line,
				int suppress_expansion, t_shell *shell)
{
	char	*expanded_line;

	expanded_line = NULL;
	if (suppress_expansion != 0)
	{
		expanded_line = ft_strdup(line);
	}
	else
	{
		expanded_line = expand_variables(line, '\0', shell);
	}
	if (expanded_line != NULL)
	{
		write(pipefd, expanded_line, ft_strlen(expanded_line));
		write(pipefd, "\n", 1);
		free(expanded_line);
	}
}

int	setup_heredoc(const char *delimiter, t_shell *shell, int suppress_expansion)
{
	int		pipefd[2];
	char	*line;

	if (delimiter == NULL || init_heredoc_pipe(pipefd) == -1)
		return (-1);
	g_signal = 0;
	set_signals_heredoc();
	while (1)
	{
		if (g_signal == SIGINT)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			g_signal = 0;
			set_signals_interactive();
			shell->exit_status = 130;
			return (-1);
		}
		line = read_heredoc_line();
		if (g_signal == SIGINT)
		{
			if (line)
				free(line);
			close(pipefd[0]);
			close(pipefd[1]);
			g_signal = 0;
			set_signals_interactive();
			shell->exit_status = 130;
			return (-1);
		}
		if (line == NULL)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			g_signal = 0;
			set_signals_interactive();
			if (isatty(STDIN_FILENO))
				write(2, "warning: here-document delimited by end-of-file\n", 49);
			return (-1);
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			close(pipefd[1]);
			g_signal = 0;
			set_signals_interactive();
			return (pipefd[0]);
		}
		write_heredoc_line(pipefd[1], line, suppress_expansion, shell);
		free(line);
	}
}
