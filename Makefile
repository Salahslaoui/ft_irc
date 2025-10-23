NAME = exec
SRC = server.cpp channel.cpp commands/join.cpp commands/mode.cpp commands/privmsg.cpp commands/invite.cpp helpers/helpers.cpp
OBJ = $(SRC:%.cpp=%.o)
INCLUDE = includes/channel.hpp includes/helper.hpp

CC = c++ -g

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)
	@echo "Executable linked successfully!"

%.o: %.cpp $(INCLUDE)
	@$(CC) -c $< -o $@
	@echo "Compiled $<"

clean:
	@rm -f $(OBJ)
	@echo "Cleaned object files"

fclean: clean
	@rm -f $(NAME)
	@echo "Cleaned executables"

re: fclean all