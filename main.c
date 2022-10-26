
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "Libft/libft.h"

typedef struct s_source
{
	char			*file;
	struct s_source	*next;
}	t_source;

typedef struct s_make
{
	int			fd;
	char		*name;
	t_source	*sources;
	int			type;
	int			fd_cache;
	char		buffer[1];
	char		file_name[256];
	int			libft;
	int			mlx;
	char		mlx_dir[256];
	char		libft_dir[256];
}	t_make;

int ft_error(char *error)
{
	while (error && *error)
		write(1,error++,1);
	return (1);
}

t_source *new_source(char *s)
{
	t_source *new;

	new = malloc(sizeof(t_source));
	new->next = NULL;
	new->file = ft_strdup(s);
	return (new);
}

void add_front(t_source **src, t_source *new)
{
		new->next = *src;
		*src = new;
}

void write_makefile(t_make make)
{
	write(make.fd, "SRCS\t=\t", 7);
	if (make.libft)
	{
		write(make.fd, make.libft_dir, ft_strlen(make.libft_dir));
		write(make.fd, "/libft.a \\\n\t\t\t\t", 15);
	}

	if (make.mlx)
	{
		write(make.fd, make.mlx_dir, ft_strlen(make.mlx_dir) );
		write(make.fd, "/libmlx.a \\\n\t\t\t\t", 16);
	}
	while (make.sources)
	{
		while (make.sources->file && *make.sources->file)
			write(make.fd, make.sources->file++, 1);
		if (make.sources->next && make.sources->next->file)
			write(make.fd, " \\\n\t\t\t\t", 7);
		make.sources = make.sources->next;
	}
	write(make.fd, "\n\n", 2);
	write(make.fd, "NAME\t=\t", 7);
	write(make.fd, make.name, ft_strlen(make.name));
	write(make.fd, "\n\n", 2);
	if (make.mlx)
	{
		write(make.fd, ft_strjoin("FLAGS\t=\t-Wall -Wextra -Werror -I",make.mlx_dir), 29 +
				ft_strlen(make.mlx_dir));
		write(make.fd, "\n\n", 2);
	}
	else
		write(make.fd, "FLAGS\t=\t-Wall -Wextra -Werror\n\n", 31);
	if (make.libft)
	{
		write(make.fd, "lib\t:\n\t@make -C ", 16);
		write(make.fd, make.libft_dir, ft_strlen(make.libft_dir));
		write(make.fd, "\n\n", 2);
	}
	if (make.mlx)
	{
		write(make.fd, "mlx\t:\n\t@make -C ", 16);
		write(make.fd, make.mlx_dir, ft_strlen(make.mlx_dir));
		write(make.fd, "\n\n", 2);
	}
	if (!make.type)
	{
		if (make.mlx)
			write(make.fd, "GCC\t=\tgcc -Lmlx -lmlx -framework OpenGL -framework AppKit\n\n", 59);
		else
			write(make.fd, "GCC\t=\tgcc\n\n", 11);
	}
	else
		write(make.fd, "ARRCS\t=\tar rcs\n\n", 16);
	write(make.fd, "OBJS\t=\t${SRCS:.c=.o}\n\n", 22);
	write(make.fd, ".c.o:\n\t${GCC} ${FLAGS} -c $< -o ${<:.c=.o}\n\n", 44);
	write(make.fd, "all: ${NAME}\n\n", 14);
	if (make.type)
	{
		write(make.fd, "${NAME}: ${OBJS} ", 17);
		write(make.fd, "\n\t\t${ARRCS} ${NAME} ${OBJS}\n\n", 29);
	}
	else
	{
		write(make.fd, "${NAME}: ${OBJS} ", 16);
		if (make.libft)
			write(make.fd, "lib", 3);
		if (make.mlx)
			write(make.fd, " mlx", 4);
		write(make.fd, "\n\t${GCC} ${FLAGS} ${OBJS} -o ${NAME}\n\n", 38);
	}
	write(make.fd, "clean:\n\t\trm -rf ${OBJS}\n\nfclean: clean\n\t\trm -rf ${NAME}\n\nre:\tfclean all\n\n",  73);
}

int main(int ac, char **av)
{
	t_make make;
	int i;

	make.sources = malloc(sizeof(t_source));
	i = 0;
	if (ac < 2)
		return (ft_error("Error: Invalid number of arguments\n"));
	system("rm -f Makefile");
	make.fd = open("Makefile", O_RDWR | O_CREAT | O_EXCL, 0644);
	make.name = av[1];
	if (ac > 2)
		make.type = ft_strncmp("lib", av[2], 3) == 0;
	else
		make.type = 0;
	system("ls *.c > cache");
	make.fd_cache = open("cache", O_RDWR);
	while (read(make.fd_cache, make.buffer, 1))
	{
		if (*make.buffer == '\n')
		{
			make.file_name[i] = 0;
			add_front(&make.sources, new_source(make.file_name));
			i = 0;
		}
		else
			make.file_name[i++] = *make.buffer;
	}
	close(make.fd_cache);
	system("ls */mlx.h | cut -f1 -d '/' > cache");
	make.fd_cache = open("cache", O_RDWR);
	i = 0;
	while (read(make.fd_cache, make.buffer, 1) && make.buffer[0] != '/' && make.buffer[0] != '\n')
	{
		make.mlx_dir[i++] = make.buffer[0];
		make.mlx_dir[i] = 0;
	}
	make.mlx_dir[i] = 0;
	make.mlx = make.mlx_dir[0];
	close(make.fd_cache);
	system("ls */libft.h | cut -f1 -d '/' > cache");
	make.fd_cache = open("cache", O_RDWR);
	i = 0;
	while (read(make.fd_cache, make.buffer, 1) && make.buffer[0] != '/' && make.buffer[0] != '\n')
	{
		make.libft_dir[i++] = make.buffer[0];
		make.libft_dir[i] = 0;
	}
	make.libft_dir[i] = 0;
	make.libft = make.libft_dir[0];
	write_makefile(make);
	system("rm -f cache");
	return (0);
}