#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
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

int main(void)
{
  CURL *curl_handle;
  CURLcode res;

  struct MemoryStruct chunk;

  chunk.memory = (char *)malloc(1); 
  chunk.size = 0; 

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://worldtimeapi.org/api/ip/106.208.115.156.txt");
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    // printf("%s\n", chunk.memory);
    for(int i=0;i<200;i++){
      if(chunk.memory[i]==':' && chunk.memory[i-1]=='e' && chunk.memory[i-2]=='m' && chunk.memory[i-8]=='d'){
        printf("Date : ");
        for(int j=i+2;j<i+12;j++){
          printf("%c", chunk.memory[j]);
        }
        printf("\n");
        printf("Time : ");
        for(int j=i+13;j<i+21;j++){
          printf("%c", chunk.memory[j]);
        }
        printf("\n");
        break;
      }
    }
    // printf("\n%lu bytes retrieved\n", (unsigned long)chunk.size);
  }

  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();

  return 0;
}