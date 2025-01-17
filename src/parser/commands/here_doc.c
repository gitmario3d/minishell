/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dparada <dparada@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 14:11:39 by dparada           #+#    #+#             */
/*   Updated: 2024/07/12 14:00:09 by dparada          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./../../../inc/minishell.h"

void	check_heredoc_line(char *line, t_minishell *minishell, int fd)
{
	ft_putendl_fd(expansion(line, minishell), fd);
	free(line);
}

void	ft_sig_here_doc(int signal)
{
	(void)signal;
	ft_putstr_fd("\n", 1);
	exit (4);
}

void	here_doc_loop(t_token *token, t_minishell *minishell, int fd)
{
	char	*line;

	signal(SIGINT, ft_sig_here_doc);
	line = readline("> ");
	while (line)
	{
		if (ft_strcmp(line, token->content) == 0)
		{
			free(line);
			break ;
		}
		check_heredoc_line(line, minishell, fd);
		line = readline("> ");
	}
	if (!line)
	{
		ft_free_minishell(minishell, 1);
		exit (0);
	}
	ft_free_minishell(minishell, 1);
	exit (1);
}

void	here_doc_wait(t_token *token, t_cmds *cmds, t_minishell *mini, int fd)
{
	int	status;

	status = ft_wait();
	if (status == 4)
		mini->flag = 1;
	else if (status == 0)
	{
		ft_putstr_fd("minishell: warning: ", 1);
		ft_putstr_fd("here_document delimited by end-of-file (wanted '", 1);
		ft_putstr_fd(token->content, 1);
		ft_putendl_fd("')", 1);
	}
	close (fd);
	cmds->fd_in = open (".here_doc.tmp", O_RDONLY);
	cmds->executor = 1;
	signal(SIGINT, control_c);
}

t_token	*here_doc(t_token *token, t_cmds *cmds, t_minishell *minishell)
{
	pid_t	pid;
	int		fd;

	if (cmds->error == 0)
	{
		if (cmds->fd_in != 0)
			close(cmds->fd_in);
		fd = open (".here_doc.tmp", O_RDWR | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
			msj_error_fd(1, ERROR_FD, cmds, minishell);
		signal(SIGINT, SIG_IGN);
		pid = fork();
		if (pid == 0)
			here_doc_loop(token, minishell, fd);
		else if (pid > 0)
			here_doc_wait(token, cmds, minishell, fd);
	}
	return (token);
}
