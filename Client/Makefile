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

CLIENT_NAME = clientV1
CLIENT_SRC = main.cpp Client.cpp
CLIENT_0SRC = $(CLIENT_SRC:.cpp=.o)

FLAGS =		clang++ #-Wall -Wextra -Werror

all:		$(CLIENT_NAME)

$(CLIENT_NAME):	$(CLIENT_0SRC)
			$(FLAGS) $(CLIENT_0SRC) -o $(CLIENT_NAME)

%.o:		%.cpp
			$(FLAGS) -c $< -o $@

clean:
			rm -rf  $(CLIENT_0SRC)

fclean:		clean
			rm -rf $(CLIENT_NAME)

re:			fclean all

s:
			wc *.cpp *.h *.hpp

.PHONY:		all clean fclean re s server