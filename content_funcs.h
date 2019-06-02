#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <cstdio>
#include <climits>

using namespace rapidjson;
using namespace std;

#define BFFR_SIZE 65535
#define MAX_IMDB_RATE 10
#define NORM_FACTOR 10

typedef struct item_type{
	int year;
	int runtime;
	double imdb_rating;
	unordered_map<string, int> word_freq;
	string director;
	string language;

	/*--- This variable was created in order to save time when calculating the similarities ---*/
	double norm;

	item_type(){}

	item_type(int _year, int _runtime, double _imdb_rating, unordered_map<string, int> _word_freq, 
				string _director, string _language, double _norm){
		year = _year;
		runtime = _runtime;
		imdb_rating = _imdb_rating;
		director = _director;
		language = _language;
		word_freq = _word_freq;
		norm = _norm;
	}
} item_type;


/*
	In order to calculate the similarities between items it is 
	necessary to sum up only normalized values, that is, numbers 
	in [0-1]. Hence, as a way to normalize some values it is needed 
	to find out max and min values
*/
void set_global_constants(unordered_map<string, item_type> items);

set<string> get_stopwords(const char *stopwords_file);

/*--- This function receives a regular expression pattern and then tokenizes the string ---*/
vector<string> tokenize(string plot);

unordered_map<string, int> count_freq(set<string> stopwords, vector<string> tokens);

/*--- This function receives a vector in a dense representation and calculate its norm ---*/
double calc_norm(unordered_map<string, int> v);

double dot(item_type &item1, item_type &item2);

double cos_similarity(item_type &item1, item_type &item2);

double absolute_value(double x);

/*--- Returns a similarity measure that lays in [0-1] and considers the common features and the textual features ---*/
double sim(item_type &item1, item_type &item2);

unordered_map<string, item_type> read_content(set<string> stopwords, const char *contents_file);
