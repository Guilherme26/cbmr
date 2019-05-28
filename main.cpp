#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace rapidjson;
using namespace std;

#define BUFFER_SIZE 65535
char BUFFER[BUFFER_SIZE];

void read_content(const char *content_file){
	FILE* content_stream = fopen(content_file, "r");
	if(content_stream == NULL){
		fprintf(stderr, "Error Openning Content File");
		exit(1);
	}

	Document item_doc;
	/*--- Scape the header ItemId,Content ---*/
	fgets(BUFFER, BUFFER_SIZE, content_stream);
	while(fgets(BUFFER, BUFFER_SIZE, content_stream)){
		string content = BUFFER;

		string item_id = content.substr(0,8);
		string item_content = content.substr(9);
		item_doc.Parse(item_content.c_str());

		if(item_doc.HasName("Error"))
			continue;
	}

	fclose(content_stream);
}

int main(){

	read_content("data/content.csv");

	return 0;
}