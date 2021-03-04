//
// Created by Mahmud Jego on 3/3/21.
//

#include "includes.h"
#include <dirent.h>

int main()
{
	DIR		*dir;
	char	*tmp;
	struct dirent * dir_el;
	if ((dir = opendir("/Users/mizola/webservt/www")))
	{
		while ((dir_el = readdir(dir)))
		{
			std::cout << dir_el->d_name << std::endl;
		}
		if (closedir(dir))
			exit(13);
	}
}