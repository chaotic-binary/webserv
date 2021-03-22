NAME		=	webserv

SRC			=	main.cpp\
				Server.cpp \
				Parser.cpp \
				ServConfig.cpp \
				Location.cpp \
				utils.cpp \
				Cgi.cpp \
				Request.cpp \
				Client.cpp \
				MimeTypes.cpp \
				response.cpp \
				method_utils.cpp \
				GetMethod.cpp \
				methods.cpp \
				PostMethod.cpp \
				methods.cpp \
				HeadMethod.cpp \
				PutMethod.cpp

BIN			=	./bin
OBJ			=	$(addprefix $(BIN)/, $(SRC:cpp=o))
DEP			=	$(OBJ:%.o=%.d)
CC			=	clang++
FLAGS		=	-Wall -Wextra -Werror -std=c++98

.PHONY: all clean fclean re

all: $(NAME)

$(BIN)/%.o:./src/%.cpp  | $(BIN)
	$(CC) $(FLAGS) -MMD -I./include -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $^ -o $@

-include $(DEP)

$(BIN):
	@mkdir $(BIN)

clean:
	@rm -rf $(BIN)
	@echo "$(NAME) object files deleted"

fclean: clean
	@rm -f $(NAME)

re: fclean all