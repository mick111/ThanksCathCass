# Indiquer quel compilateur est à utiliser
CC      = gcc

# Spécifier les options du compilateur
CFLAGS  =
LDFLAGS = 
LDLIBS  = 

# Nom de l'exécutable
PROG  = ThanksCathCass 

# Liste de fichiers objets nécessaires pour le programme final
SOURCES = main.c MessageBox.c MessageDB.c 
HEADERS = MessageBox.h MessageDB.h
 
all: $(PROG)

# Etape de compilation et d'éditions de liens
$(PROG): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(PROG) $(SOURCES)

clean:
	rm $(PROG)