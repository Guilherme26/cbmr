#include "content_funcs.h"
#include <ctime>

int MAX_RUNTIME = 0;
int LAST_YEAR = 0;
int MIN_RUNTIME = INT_MAX;
int FIRST_YEAR = INT_MAX;

void set_global_constants(unordered_map<string, item_type> items){
	for(auto& iterator: items){
		item_type item = iterator.second;

		MAX_RUNTIME = (item.runtime > MAX_RUNTIME) ? item.runtime : MAX_RUNTIME;
		LAST_YEAR = (item.year > LAST_YEAR) ? item.year : LAST_YEAR;

		MIN_RUNTIME = (item.runtime < MIN_RUNTIME) ? item.runtime : MIN_RUNTIME;
		FIRST_YEAR = (item.year < FIRST_YEAR) ? item.year : FIRST_YEAR;
	}
}


set<string> get_stopwords(const char *stopwords_file){
	FILE *in_stream = fopen(stopwords_file, "r");
	set<string> stopwords;
	char str[128];
	
	while(fscanf(in_stream, "%s", str) != EOF)
		stopwords.insert(string(str));

	return stopwords;
}


vector<string> tokenize(string plot){
	regex word_pattern("[\\w]+");
	sregex_token_iterator iterator(plot.begin(), plot.end(), word_pattern);
	vector<string> tokens(iterator, sregex_token_iterator());

	return tokens;
}


unordered_map<string, int> count_freq(set<string> stopwords, vector<string> tokens){
	unordered_map<string, int> word_freq;

	for(int i = 0; i < tokens.size(); i++){
		if(stopwords.find(tokens[i]) != stopwords.end()){
			if(word_freq.find(tokens[i]) == word_freq.end())
				word_freq[tokens[i]] = 1;
			else
				word_freq[tokens[i]] += 1;
		}
	}

	return word_freq;
}


double calc_norm(unordered_map<string, int> v){
	unordered_map<string, int>::iterator it;
	double sum = 0.0;
	
	for(it = v.begin(); it != v.end(); it++)
		sum += (it->second * it->second);

	return sqrt(sum);
}


double dot(item_type &item1, item_type &item2){
	double dot_prod = 0.0;

	for(auto& iterator : item1.word_freq){
		string first_token = iterator.first;
		int first_freq = iterator.second;

		/*--- Then the current token exists in the dict ---*/
		if(item2.word_freq.find(first_token) != item2.word_freq.end()){
			int second_freq = item2.word_freq[first_token];
			dot_prod += (first_freq * second_freq);
		}
	}

	return dot_prod;
}


double cos_similarity(item_type &item1, item_type &item2){
	double dot_prod = dot(item1, item2);
	double norms_prod = item1.norm * item2.norm;
	
	return dot_prod / norms_prod;
}

double absolute_value(double x){
	if(x < 0.0)
		return -x;
	return x;
}

double sim(item_type &item1, item_type &item2){
	double sum = 0.0;

	sum += (item1.director.compare(item2.director) == 0) ? 1 : 0;
	sum += (item1.language.compare(item2.language) == 0) ? 1 : 0;	
	sum += (absolute_value(item1.imdb_rating - item2.imdb_rating) / MAX_IMDB_RATE);
	sum += (absolute_value(item1.year - item2.year) / (LAST_YEAR - FIRST_YEAR));
	sum += (absolute_value(item1.runtime - item2.runtime) / (MAX_RUNTIME - MIN_RUNTIME));
	/*--- The sum of the common features' values is then normalized in [0-1] ---*/
	sum /= NUM_FEATURES;
	/*--- Sums the common features similarity value with cosine similarity and normalizes ---*/
	sum = (sum + cos_similarity(item1, item2)) / 2;

	return isnan(sum) ? 0.0 : sum;
}

unordered_map<string, item_type> read_content(set<string> stopwords, const char *contents_file){
	FILE* in_stream = fopen(contents_file, "r");
	if(in_stream == NULL){
		fprintf(stderr, "Error Openning Content File");
		exit(1);
	}


	Document item_doc;
	char BFFR[BFFR_SIZE];
	int cur_index = 0;
	unordered_map<string, item_type> items;

	/*--- Scape the header ItemId,Content ---*/
	char *scape = fgets(BFFR, BFFR_SIZE, in_stream);
	while(fgets(BFFR, BFFR_SIZE, in_stream)){
		string content = BFFR;

		string index = content.substr(0,8);
		string item_content = content.substr(9);
		item_doc.Parse(item_content.c_str());

		if(item_doc.HasMember("Plot")){
			int year = atof(item_doc["Year"].GetString());
			
			char runtime_str[128];
			strcpy(runtime_str, item_doc["Runtime"].GetString());
			int runtime = atoi(strtok(runtime_str, " "));
			
			double imdb_rating = atof(item_doc["imdbRating"].GetString());
			string plot = item_doc["Plot"].GetString();
			string director = item_doc["Director"].GetString();
			string language = item_doc["Language"].GetString();
			
			transform(plot.begin(), plot.end(), plot.begin(), ::tolower);
			vector<string> tokens = tokenize(plot);
			unordered_map<string, int> word_freq = count_freq(stopwords, tokens);

			double norm = calc_norm(word_freq);

			items.insert(make_pair(index, item_type(year, runtime, imdb_rating, word_freq, director, language, norm)));
		}
	}

	fclose(in_stream);
	return items;
}