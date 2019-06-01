#include "content_funcs.h"
#include <unordered_map>
#include <cstdio>
#include <string>
#include <cstring>

using namespace std;

#define BUFFER_SIZE 65535

unordered_map<string, unordered_map<string, int> > read_ratings(const char *ratings_file);

double predict(unordered_map<string, item_type> &items, unordered_map<string, int> &consumed_items, string item_unknown);

void build_submission_file(unordered_map<string, item_type> items, 
							unordered_map<string, unordered_map<string, int> > user_to_items, 
							const char *targets_file);
