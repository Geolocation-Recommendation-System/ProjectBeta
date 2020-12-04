void calc()
{
    	FILE *fp;
	long lSize;
	char *buffer;
	struct json_object *parsed_json;
	struct json_object *response;
	struct json_object *groups;
	struct json_object *item;
	struct json_object *items;
	struct json_object *items_refined;
	struct json_object *venue;
	struct json_object *location;
	struct json_object *lat;
	struct json_object *lng;
	struct json_object *id;
	size_t n_groups, n_items;
	fp = fopen ( "coffee.json" , "rb" );
	if( !fp ) perror("coffee.json"),exit(1);

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );
	buffer = calloc( 1, lSize+1 );
	if( !buffer ) fclose(fp),fputs("memory alloc fails",stderr),exit(1);
	if( 1!=fread( buffer , lSize, 1 , fp) )
	  fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);

	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, "response", &response);
	json_object_object_get_ex(response, "groups", &groups);
	n_groups = json_object_array_length(groups);
	
	for(int i=0;i<n_groups;i++) {
		item = json_object_array_get_idx(groups, i);
	}
	json_object_object_get_ex(item, "items", &items);
	
	n_items = json_object_array_length(items);
	//printf("Found %lu groups\n",n_items);
	printf("Dataset : \n");
	FILE *file, *file2;
	file = fopen("data.txt", "w");
	file2 = fopen("id.txt", "w");
	if( !fp ) perror("data.txt"),exit(1);
	for(int i=0;i<n_items;i++) {
                items_refined = json_object_array_get_idx(items, i);
        	json_object_object_get_ex(items_refined, "venue", &venue);
		json_object_object_get_ex(venue, "id", &id);
		json_object_object_get_ex(venue, "location", &location);
		json_object_object_get_ex(location, "lat", &lat);
		json_object_object_get_ex(location, "lng", &lng);
		printf("%f\t%f\n",json_object_get_double(lat), json_object_get_double(lng));
		fprintf(file2,"%s\n",json_object_get_string(id));
		fprintf(file,"%f,%f\n",json_object_get_double(lat),json_object_get_double(lng));
	}
	fclose(file);
	
	//printf("%s\n",json_object_get_string(items_refined));
	fclose(fp);
	free(buffer);

}
