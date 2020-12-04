#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
 
static GtkWidget *place;
static GtkWidget *result;
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
 
char *concatenate(char *a, char *b, char *c)
{
  int size = strlen(a) + strlen(b) + strlen(c) + 1;
  char *str = malloc(size);
  strcpy (str, a);
  strcat (str, b);
  strcat (str, c); 

  return str;
}

void do_calculate(GtkWidget *calculate, gpointer data) {

    char *buffer;
    buffer = (char *)gtk_entry_get_text(GTK_ENTRY(place));

    char *ch1 = "https://api.foursquare.com/v2/venues/explore?client_id=ODFO3RLC1RT3MSJHH03G0G1AEYUYVIMRHTTJIHZWU10T42KV&client_secret=A2O30KZYRMRXHV4UI5E1KUWCJLTU5XNHGFYVMOOGTM5ZUQ0P&v=20180323&ll=40.730610,-73.935242&query=";
    char *ch2 = "&limit=100";
    ch = concatenate(ch1, buffer, ch2);
    
    
    //gtk_label_set_text(GTK_LABEL(result), buffer);
}

int main(int argc, char **argv)
{
  GtkWidget *window, *grid, *calculate, *label;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window), grid);
  label = gtk_label_new ("Enter the type of place");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
  place = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), place, 1, 0, 1, 1);

    
  calculate = gtk_button_new_with_label("Fetch");
  g_signal_connect(calculate, "clicked", G_CALLBACK(do_calculate), NULL);
  gtk_grid_attach(GTK_GRID(grid), calculate, 2, 0, 1, 1);
  gtk_widget_show_all(window);
  gtk_main();
    
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
    printf("%s bytes retrieved\n", chunk.memory);
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);
 
  free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
  return 0;
}
