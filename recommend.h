
struct MemoryStruct {
  char *memory;
  size_t size;
};
  char *ch;
  static size_t
  WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
  {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

char *concatenate(char *a, char b[], char *c)
{
  int size = strlen(a) + strlen(b) + strlen(c) + 1;
  char *str = malloc(size);
  strcpy (str, a);
  strcat (str, b);
  strcat (str, c); 
  return str;
}

void do_calculate(char buffer[]) {

    
    char *ch1 = "https://api.foursquare.com/v2/venues/";
    char *ch2 = "/likes?client_id=ODFO3RLC1RT3MSJHH03G0G1AEYUYVIMRHTTJIHZWU10T42KV&client_secret=A2O30KZYRMRXHV4UI5E1KUWCJLTU5XNHGFYVMOOGTM5ZUQ0P&v=20201130";
    
    ch = concatenate(ch1, buffer, ch2);
    
}

int calcu(char *chunk, int arr[])
{
	struct json_object *parsed_json;
	struct json_object *response;
	struct json_object *likes;
	struct json_object *count;
	int i=0;
	parsed_json = json_tokener_parse(chunk);
	json_object_object_get_ex(parsed_json, "response", &response);
	json_object_object_get_ex(response, "likes", &likes);
	json_object_object_get_ex(likes, "count", &count);
	
	return json_object_get_int(count);
	
}

int getLikes(int arr[]){
  
  FILE *fptr;
  int i=0;
    //double x[SIZE],y[SIZE];
    //float cent[K][2];
    if ((fptr = fopen("id.txt", "r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    char c[1000];
    while((fgets (c, 1000, fptr))!= NULL) {
        c[24]='\0';
        do_calculate(c);
        
        CURL *curl_handle;
        CURLcode res;
        struct MemoryStruct chunk;
        chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
        chunk.size = 0;    /* no data at this point */ 
        curl_global_init(CURL_GLOBAL_ALL);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_URL, ch);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        res = curl_easy_perform(curl_handle);
        if(res != CURLE_OK) {
           fprintf(stderr, "curl_easy_perform() failed: %s\n",
           curl_easy_strerror(res));
        }
        else {
           arr[i] = calcu(chunk.memory, arr);
        }
 
       /* cleanup curl stuff */ 
       i++;
       curl_easy_cleanup(curl_handle);
       free(chunk.memory);
       /* we're done with libcurl, so clean it up */ 
       curl_global_cleanup();
       ch="";
    } 
  return 0;
}
