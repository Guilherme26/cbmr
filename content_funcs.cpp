#include "content_funcs.h"
#include <ctime>

int MAX_RUNTIME = 0;
int LAST_YEAR = 0;
int MIN_RUNTIME = INT_MAX;
int FIRST_YEAR = INT_MAX;

void set_global_constants(vector<item_type> items){
	for(int i = 0; i < items.size(); i++){
		MAX_RUNTIME = (items[i].runtime > MAX_RUNTIME) ? items[i].runtime : MAX_RUNTIME;
		LAST_YEAR = (items[i].year > LAST_YEAR) ? items[i].year : LAST_YEAR;

		MIN_RUNTIME = (items[i].runtime < MIN_RUNTIME) ? items[i].runtime : MIN_RUNTIME;
		FIRST_YEAR = (items[i].year < FIRST_YEAR) ? items[i].year : FIRST_YEAR;
	}
}


vector<string> tokenize(string plot){
	regex word_pattern("[\\w]+");
	sregex_token_iterator iterator(plot.begin(), plot.end(), word_pattern);
	vector<string> tokens(iterator, sregex_token_iterator());

	return tokens;
}


unordered_map<string, int> count_freq(vector<string> tokens){
	unordered_map<string, int> word_freq;

	for(int i = 0; i < tokens.size(); i++){
		if(word_freq.find(tokens[i]) == word_freq.end())
			word_freq[tokens[i]] = 1;
		else
			word_freq[tokens[i]] += 1;
	}

	return word_freq;
}


float dot(item_type item1, item_type item2){
	unordered_map<string, int>::iterator it;
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


float calc_norm(unordered_map<string, int> v){
	unordered_map<string, int>::iterator it;
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

float absolute_value(float x){
	if(x < 0.0)
		return -x;
	return x;
}

float similarity(item_type item1, item_type item2){
	float sum = 0.0;

	sum += (item1.director.compare(item2.director) == 0) ? 1 : 0;
	sum += (item1.language.compare(item2.language) == 0) ? 1 : 0;	
	sum += (absolute_value(item1.imdb_rating - item2.imdb_rating) / MAX_IMDB_RATE);
	sum += (absolute_value(item1.year - item2.year) / (LAST_YEAR - FIRST_YEAR));
	sum += (absolute_value(item1.runtime - item2.runtime) / (MAX_RUNTIME - MIN_RUNTIME));
	/*--- The sum of the common features' values is then normalized in [0-1] ---*/
	sum /= NUM_FEATURES;
	/*--- Sums the common features similarity value with cosine similarity and normalizes ---*/
	sum = (sum + cos_similarity(item1, item2)) / 2;

	return sum;
}

vector<item_type> read_content(const char *contents_file){
	FILE* content_stream = fopen(contents_file, "r");
	if(content_stream == NULL){
		fprintf(stderr, "Error Openning Content File");
		exit(1);
	}


	Document item_doc;
	char BFFR[BFFR_SIZE];
	int cur_index = 0;
	vector<item_type> items;

	/*--- Scape the header ItemId,Content ---*/
	char *scape = fgets(BFFR, BFFR_SIZE, content_stream);
	while(fgets(BFFR, BFFR_SIZE, content_stream)){
		string content = BFFR;

		string item_id = content.substr(0,8);
		string item_content = content.substr(9);
		item_doc.Parse(item_content.c_str());

		if(item_doc.HasMember("Plot")){
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
			unordered_map<string, int> word_freq = count_freq(tokens);
			

			float norm = calc_norm(word_freq);

			items.push_back(item_type(cur_index, year, runtime, imdb_rating, word_freq, director, language, norm));

			cur_index += 1;
		}
	}

	fclose(content_stream);
	return items;
}


unordered_map<int, unordered_map<int, float> > build_similarity_matrix(vector<item_type> items){
	unordered_map<int, unordered_map<int, float> > similarity_matrix;

	for(int i = 0; i < items.size(); i++){
		unordered_map<int, float> js;
		
		printf("%d\n", i);
		/*--- As the matrix is mirrored it is only insert from i index forth ---*/
		for(int j = i+1; j < items.size(); j++)
			js.insert(make_pair(j, similarity(items[i], items[j])));

		similarity_matrix.insert(make_pair(i, js));
	}

	return similarity_matrix;
}