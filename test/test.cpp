//
// Created by Mahmud Jego on 3/3/21.
//

#include "includes.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
int main()
{
/*	DIR		*dir;
	char	*tmp;
	struct dirent * dir_el;
	if ((dir = opendir("/")))
	{
		while ((dir_el = readdir(dir)))
		{
			std::cout << dir_el->d_name << std::endl;
		}
		if (closedir(dir))
			exit(13);
	}*/
	int i = mkdir("hello", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	printf("%s\n", strerror(errno));
	if (errno == EACCES)
		printf("not");
	if (errno == EEXIST)
		printf("ude est");
	printf("%d\n", i);

}