#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <regex>
#include <cstdio>
#include <cstdlib>

using namespace rapidjson;
using namespace std;

#define BUFFER_SIZE 65535
char BUFFER[BUFFER_SIZE];

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
vector<string> tokenize(string plot){
	regex word_pattern("[\\w]+");
	sregex_token_iterator iterator(plot.begin(), plot.end(), word_pattern);
	vector<string> tokens(iterator, sregex_token_iterator());

	return tokens;
}


map<string, int> count_freq(vector<string> tokens){
	map<string, int> word_freq;

	for(int i = 0; i < tokens.size(); i++){
		if(word_freq.find(tokens[i]) == word_freq.end())
			word_freq[tokens[i]] = 1;
		else
			word_freq[tokens[i]] += 1;
	}

	return word_freq;
}


float dot(item_type item1, item_type item2){
	map<string, int>::iterator it;
	float sum = 0.0;

	for(it = item1.word_freq.begin(); it != item1.word_freq.end(); it++){
		string first_token = it->first;
		int first_freq = it->second;

		/*--- Then the current token exists in the dict ---*/
		if(item2.word_freq.find(first_token) != item2.word_freq.end()){
			int second_freq = item2.word_freq[first_token];
			sum += (first_freq * second_freq);
		}
	}

	return sum;
}


/*--- This function receives a vector in a dense representation and calculate its norm ---*/
float calc_norm(map<string, int> v){
	map<string, int>::iterator it;
	float sum = 0.0;
	
	for(it = v.begin(); it != v.end(); it++)
		sum += (it->second * it->second);

	return sqrt(sum);
}


float cos_similarity(item_type item1, item_type item2){
	float dot_prod = dot(item1, item2);
	float norms_prod = item1.norm * item2.norm;
	
	return dot_prod / norms_prod;
}


vector<item_type> read_content(const char *content_file){
	FILE* content_stream = fopen(content_file, "r");
	if(content_stream == NULL){
		fprintf(stderr, "Error Openning Content File");
		exit(1);
	}


	Document item_doc;
	int cur_index = 0;
	vector<item_type> items;

	/*--- Scape the header ItemId,Content ---*/
	fgets(BUFFER, BUFFER_SIZE, content_stream);
	while(fgets(BUFFER, BUFFER_SIZE, content_stream)){
		string content = BUFFER;

		string item_id = content.substr(0,8);
		string item_content = content.substr(9);
		ParseResult ok = item_doc.Parse(item_content.c_str());

		if(ok){
			int year = atof(item_doc["Year"].GetString());
			
			char runtime_str[128];
			strcpy(runtime_str, item_doc["Runtime"].GetString());
			int runtime = atoi(strtok(runtime_str, " "));
			
			float imdb_rating = atof(item_doc["imdbRating"].GetString());
			string plot = item_doc["Plot"].GetString();
			string director = item_doc["Director"].GetString();
			string language = item_doc["Language"].GetString();
			
			transform(plot.begin(), plot.end(), plot.begin(), ::tolower);
			vector<string> tokens = tokenize(plot);
			map<string, int> word_freq = count_freq(tokens);
			
			float norm = calc_norm(word_freq);

			items.push_back(item_type(cur_index, year, runtime, imdb_rating, word_freq, director, language, norm));

			cur_index += 1;
		}
	}

	fclose(content_stream);
	return items;
}

int main(){

	read_content("data/content.csv");

	return 0;
}