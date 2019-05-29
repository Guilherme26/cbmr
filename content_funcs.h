#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <regex>
#include <cstdio>

using namespace rapidjson;
using namespace std;

#define BFFR_SIZE 65535

typedef struct item_type{
	/*--- This variable is responsible to keep the item's index in similarity matrix ---*/
	int index;
	
	int year;
	int runtime;
	float imdb_rating;
	map<string, int> word_freq;
	string director;
	string language;

	/*--- This variable was created in order to save time when calculating the similarities ---*/
	float norm;

	item_type(int _index, int _year, int _runtime, float _imdb_rating, map<string, int> _word_freq, 
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

/*--- This function receives a regular expression pattern and then tokenizes the string ---*/
vector<string> tokenize(string plot);

map<string, int> count_freq(vector<string> tokens);

float dot(item_type item1, item_type item2);

/*--- This function receives a vector in a dense representation and calculate its norm ---*/
float calc_norm(map<string, int> v);

float cos_similarity(item_type item1, item_type item2);

vector<item_type> read_content(const char *content_file);