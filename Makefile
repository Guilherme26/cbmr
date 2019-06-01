run: all 
	./recommender content.csv ratings.csv targets.csv > submission.csv

all: clean recommender

recommender: content_funcs.o ratings.o
	g++ -w main.cpp -std=c++11 -O3 -o recommender content_funcs.o ratings.o

content_funcs.o: content_funcs.cpp
	g++ -c -std=c++11 -O3 content_funcs.cpp

ratings.o: ratings.cpp
	g++ -c -std=c++11 -O3 ratings.cpp

clean:
	-rm -f recommender
	-rm -f *.o