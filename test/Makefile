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

SERVER_NAME = serverV1
SERVER_SRC = ../src/Server.cpp testServer.cpp ../src/Parser.cpp \
			../src/ServConfig.cpp ../src/Location.cpp \
			../src/utils.cpp ../src/FD.cpp

SERVER_0SRC = $(SERVER_SRC:.cpp=.o)
INCLUDES = ../include

FLAGS =		clang++ -I$(INCLUDES) #-Wall -Wextra -Werror

all:		$(SERVER_NAME)

$(SERVER_NAME):	$(SERVER_0SRC)
			$(FLAGS) $(SERVER_0SRC) -o $(SERVER_NAME)

%.o:		%.cpp
			$(FLAGS) -I$(INCLUDES) -c $< -o $@

clean:
			rm -rf  $(SERVER_0SRC)

fclean:		clean
			rm -rf $(SERVER_NAME)

re:			fclean all

s:
			wc *.cpp *.h *.hpp

.PHONY:		all clean fclean re s server