CPPFILES = barnes_hut.cpp


all: 
	g++ $(CPPFILES) -o barnes_hut -ltbb -ltbbmalloc

clean:
	rm -f barnes_hut
