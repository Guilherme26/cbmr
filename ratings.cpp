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


double predict(unordered_map<string, item_type> &items, unordered_map<string, int> &consumed_items, string item_unknown){
	/*--- If there were no consumed items returns some value ---*/
	if(consumed_items.size() == 0 or items.find(item_unknown) == items.end())
		return 7.0;

	double prediction = 0.0, sum_of_weights = 0.0;
	for(auto& iterator : consumed_items){
		string consumed_id = iterator.first;
		int consumed_rate = iterator.second;

		double cur_weight = sim(items[item_unknown], items[consumed_id]);
		sum_of_weights += cur_weight;
		prediction += (cur_weight * consumed_rate);
	}
	sum_of_weights = sum_of_weights > 0.0 ? sum_of_weights : 1.0;
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