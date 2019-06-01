#include "ratings.h"

int main(int argc, char *argv[]){
	if(argc != 4){
		fprintf(stderr, "Incorrect Number of Parameters");
		exit(1);
	}

	const char *contents_file = argv[1];
	const char *ratings_file = argv[2];
	const char *targets_file = argv[3];

	set<string> stopwords = get_stopwords("stopwords");
	unordered_map<string, item_type> items = read_content(stopwords, contents_file);
	set_global_constants(items);
	unordered_map<string, unordered_map<string, int> > user_to_items = read_ratings(ratings_file);
	build_submission_file(items, user_to_items, targets_file);

	return 0;
}