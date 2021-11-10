#include "daemonize.h"

void	ft_freetab(char **str, size_t s)
{
	size_t i;

	i = 0;
	while (i < s){
	    free(str[i]);
	    i++;
	}
	free(str);
	str = NULL;//	free(str);
}


size_t		lw(char const *s, char c)
{
	int		i;
	size_t	r;

	i = 0;
	r = 0;
	while (s[i] && s[i] != c)
	{
		++i;
		++r;
	}
	return (r);
}

size_t		count_words(char const *str, char c)
{
	size_t i;

	i = 0;
	while (*str)
	{
		while (*str && *str == c)
			++str;
		if (*str)
			++i;
		while (*str && *str != c)
			++str;
		i++;
	}
	return (i);
}

char		*cp(char const *s, char c)
{
	char	*ju;
	size_t	i;

	if (!(ju = malloc(sizeof(char**) * (lw(s, c) + 1))))
		return (NULL);
	i = 0;
	while (*s && *s != c)
		ju[i++] = *s++;
	ju[i] = '\0';
	return (ju);
}

char		**ft_split(char const *s, char c)
{
	int		y;
	char	**ju;

	y = 0;
	if (!(s) || !(ju = malloc(sizeof(char*) * (count_words(s, c) + 1))))
		return (NULL);
	while (*s)
	{
		while (*s == c && *s)
			++s;
		if (*s != c && *s)
		{
			if (!(ju[y++] = cp(s, c)))
			{
				ft_freetab(ju, count_words(s, c));
				return (NULL);
			}
			s = s + lw(s, c);
		}
	}
	ju[y] = NULL;
	return (ju);
}