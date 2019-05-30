#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <unordered_map>
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
#define NUM_FEATURES 6

typedef struct item_type{
	/*--- This variable is responsible to keep the item's index in similarity matrix ---*/
	int index;
	
	int year;
	int runtime;
	float imdb_rating;
	unordered_map<string, int> word_freq;
	string director;
	string language;

	/*--- This variable was created in order to save time when calculating the similarities ---*/
	float norm;

	item_type(int _index, int _year, int _runtime, float _imdb_rating, unordered_map<string, int> _word_freq, 
				string _director, string _language, float _norm){
		index = _index;
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
void set_global_constants(vector<item_type> items);

/*--- This function receives a regular expression pattern and then tokenizes the string ---*/
vector<string> tokenize(string plot);

unordered_map<string, int> count_freq(vector<string> tokens);

float dot(item_type item1, item_type item2);

/*--- This function receives a vector in a dense representation and calculate its norm ---*/
float calc_norm(unordered_map<string, int> v);

float cos_similarity(item_type item1, item_type item2);

float absolute_value(float x);

/*--- Returns a similarity measure that lays in [0-1] and considers the common features and the textual features ---*/
float similarity(item_type item1, item_type item2);

vector<item_type> read_content(const char *contents_file);

unordered_map<int, unordered_map<int, float> > build_similarity_matrix(vector<item_type> items);
