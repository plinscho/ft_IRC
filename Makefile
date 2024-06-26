MAKEFLAGS += --no-print-directory
NAME = ircserv
CFLAGS = -Wall -Werror -Wextra -std=c++98
OBJECTS = src/irc.cpp \
			src/client/Client.cpp \
			src/server/Server.cpp \
			src/server/ServerUtils.cpp \
			src/server/Cmd.cpp \
			src/server/CmdUtils.cpp
#			src/


HEADERS = src/server/Server.hpp \
			src/client/Client.hpp

all: $(NAME)

$(NAME): $(OBJECTS) $(HEADERS) Makefile clean
	@c++ $(CFLAGS) $(OBJECTS) -o $(NAME)

fclean: 
	@rm -rf ${NAME}

clean:
	@rm -f ${NAME}

re: fclean all

git: fclean
	@git add .
	@TYPE=$(shell gum choose "fix:" "feat:" "refactor:" "test:" "chore:" "revert:"); \
	DESCRIPTION=$(shell gum input --placeholder "Details of this change (Add '')"); \
	if gum confirm "Another commit!"; then \
		git commit -m "$$TYPE $$DESCRIPTION"; \
		gum spin --spinner meter --title "Pushing to repository" -- git push; \
		gum style --foreground 212 --border-foreground 213 --align center --width 70 --margin "0 2" \
		--padding "2 4" --border double 'Git push complete!' | lolcat; \
	else \
		printf "Changes not committed."; \
	fi

.PHONY: clean all fclean re main
