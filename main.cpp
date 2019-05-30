#include "content_funcs.h"

int main(int argc, char *argv[]){
	if(argc != 4){
		fprintf(stderr, "Incorrect Number of Parameters");
		exit(1);
	}

	const char *contents_file = argv[1];
	const char *ratings_file = argv[1];
	const char *targets_file = argv[1];

	vector<item_type> items = read_content(contents_file);
	set_global_constants(items);
	unordered_map<int, unordered_map<int, float> > similarities = build_similarity_matrix(items);

	return 0;
}