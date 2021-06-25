NAME		=	webserv
NAME_B		=	webserv_bonus

SRC_M		=	main.cpp\
				ServConfig.cpp\
				Server.cpp\
				Parser.cpp\
				Location.cpp\
				utils.cpp\
				Cgi.cpp\
				Request.cpp\
				Client.cpp\
				MimeTypes.cpp\
				response.cpp\
				GetMethod.cpp\
				PostMethod.cpp\
				PutMethod.cpp\
				response_generator.cpp\
				base64.cpp \
				DeleteMethod.cpp

SRC_B		=	main.cpp\
				ServConfig_bonus.cpp \
				Server.cpp\
				Parser.cpp\
				Location.cpp\
				utils.cpp\
				Cgi.cpp\
				Request.cpp\
				Client.cpp\
				MimeTypes.cpp\
				response.cpp\
				GetMethod.cpp\
				PostMethod.cpp\
				PutMethod.cpp\
				response_generator.cpp\
				base64.cpp \
				DeleteMethod.cpp

BIN			=	./bin
OBJ_M		=	$(addprefix $(BIN)/, $(SRC_M:cpp=o))
OBJ_B		=	$(addprefix $(BIN)/, $(SRC_B:cpp=o))
DEP			=	$(OBJ_M:%.o=%.d) \
				$(OBJ_B:%.o=%.d)
CC			=	clang++
FLAGS		=	-Wall -Wextra -Werror -std=c++98

.PHONY: all clean fclean re

all: $(NAME)

$(BIN)/%.o:./src/%.cpp  | $(BIN)
	$(CC) $(FLAGS) -MMD -I./include -c $< -o $@

$(NAME): $(OBJ_M)
	$(CC) $(FLAGS) $^ -o $@

bonus: $(NAME_B)

$(NAME_B): $(OBJ_B)
	$(CC) $(FLAGS) $^ -o $@

-include $(DEP)

$(BIN):
	@mkdir $(BIN)

clean:
	@rm -rf $(BIN)
	@echo "$(NAME) object files deleted"

fclean: clean
	@rm -f $(NAME)
	@rm -f $(NAME_B)

test: run
	./tester http://localhost:1234

run: all
	./webserv config/mizola.conf &
	./tester http://localhost:1234

re: fclean all