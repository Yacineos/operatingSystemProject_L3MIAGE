#BINOMES :Oukkal Yacine N° étudiant: 12213245 || Monachon Vivian N° étudiant: 11907866

# Nom du projet

PROJET = projetSys

# Fichiers sources

SRC = projetSys.c projectFunctions.c table.c

# Fichiers objets

OBJ = $(SRC:.c=.o)

# Compilateur

CC = gcc

# Options de compilation

CFLAGS = -Wall -Wextra -g

# RÃ¨gles de compilation

all: $(PROJET)

$(PROJET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(PROJET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(PROJET)

run: $(PROJET)
	./$(PROJET)