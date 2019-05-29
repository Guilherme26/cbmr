run: all 
	./recommender content.csv ratings.csv targets.csv > submission.csv

all: clean recommender

recommender: content_funcs.o
	g++ -w main.cpp -std=c++11 -O3 -o recommender content_funcs.o

content_funcs.o: content_funcs.cpp
	g++ -c -std=c++11 -O3 content_funcs.cpp

clean:
	-rm -f recommender
	-rm -f content_funcs.o