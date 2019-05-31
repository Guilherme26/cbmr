#include "ratings.h"

unordered_map<string, unordered_map<string, int> > read_ratings(const char *ratings_file){
	unordered_map<string, unordered_map<string, int> > user_to_items;

	FILE *in_stream = fopen(ratings_file, "r");
	char line[BUFFER_SIZE];

	/*--- Scape the header ---*/
	int status = fscanf(in_stream, "%s", line);
	while(fscanf(in_stream, "%s", line) != EOF){
		int rating;

		string user_id = string(strtok(line, ":"));
		string item_id = string(strtok(NULL, ","));
		rating = atoi(strtok(NULL, ","));

		if(user_to_items.find(user_id) == user_to_items.end()){	
			unordered_map<string, int> items_to_rating;
			user_to_items.insert(make_pair(user_id, items_to_rating));
		}
		user_to_items[user_id].insert(make_pair(item_id, rating));
	}
	fclose(in_stream);

	return user_to_items;
}


float predict(unordered_map<string, item_type> items, unordered_map<string, int> consumed_items, string item_unknown){
	float prediction = 0.0, sum_of_weights = 0.0;
	
	for(auto& iterator : items){
		string consumed_id = iterator.first;
		item_type consumed = iterator.second;

		float cur_weight = similarity(items[item_unknown], consumed);
		sum_of_weights += cur_weight;
		prediction += (cur_weight * consumed_items[consumed_id]);
	}

	return prediction / sum_of_weights;
}


void build_submission_file(unordered_map<string, item_type> items, 
							unordered_map<string, unordered_map<string, int> > user_to_items, 
							const char *targets_file){
	FILE *in_stream = fopen(targets_file, "r");
	char line[BUFFER_SIZE];

	/*--- Creates file's header ---*/
	printf("UserId:ItemId,Prediction\n");

	/*--- Scape the header ---*/
	int status = fscanf(in_stream, "%s", line);
	while(fscanf(in_stream, "%s", line) != EOF){
		string user_id = string(strtok(line, ":"));
		string item_id = string(strtok(NULL, ","));

		printf("%s:%s,%f\n", user_id.c_str(), item_id.c_str(), predict(items, user_to_items[user_id], item_id));
	}
	fclose(in_stream);
}