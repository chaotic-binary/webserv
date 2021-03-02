# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mizola <mizola@student.21-school.ru>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/01 09:16:47 by mizola            #+#    #+#              #
#    Updated: 2020/11/19 05:25:01 by mizola           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all:
			make -C test
			make -C Client
			mv test/serverV1 ./
			mv Client/clientV1 ./

clean:
			make -C test clean
			make -C Client clean
			rm -rf  *.o

fclean:
			make -C test fclean
			make -C Client fclean
			rm -rf serverV1 clientV1 a.out

free: fclean
			rm -rf *.o src/*.o

run: all
			./serverV1 config/test.conf
s:
			wc *.cpp *.h *.hpp

.PHONY:		all clean fclean re s server