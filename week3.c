//cc -o hw program1.c
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/stat.h>

void doHelp()
{
	printf("\n");
	printf("This is the help screen:\n\n");
	printf("The -h or --help option will always bring you to this help screen regardless of how many other options are set. Here you can see the options available to you and examples for a proper command.\n\n");
	printf("Options List:\n");
	printf(" -h or --help: Brings you to help screen\n");
	printf(" -g or --get: Executes GET request using URL provided by user.\n");
	printf(" -o or --post: Exectues POST request using URL and string provided by user\n");
	printf(" -p or --put: Executes PUT request using URL and string provided by user.\n");
	printf(" -d or --delete: Executes DELETE request using URL provided by user\n");

	printf("\n");
	printf("Enter a URL with the -u or --url option:\n");
	printf("-u http://url.com or --url http://url.com\n");
	printf("\n");
	printf("The string MUST be the last value entered.  It must be 1 word without spaces, or if it contains spaces the entire string must be wrapped in double-quotes (\"This is an example of a string\")\n\n");

	printf("Below is an example of a proper CLI command:\n");
	printf("./<filename> -g -u <url>\n");
	printf("\n");
	printf("POST, PUT and DELETE need strings as the last option of the command.  So the command would look like:\n");
	printf("./<filename> -p -u <url> <string>\n\n");

	printf("NOTE: The string you provide may be a maximum length of 20 characters. It must be a single word, or if it includes spaces, it must be wrapped in double-quotes (\").\n");

	printf("\n");

	printf("The following banned characters are not allowed, using them will terminate the program: !@#$%^&*(){}[];'<>?~`\n");
}

void doPost(char * url, char * myString)
{
	CURL * curl;
	CURLcode res;
	char str[4] = "str=";
	char message[100];
	strcat(message, str);
	strcat(message, myString);

	curl = curl_easy_init();
	if(curl){
	
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
}

size_t got_data(char *buffer, size_t itemSize, size_t nItems, void* ignore)
{
	size_t bytes = itemSize * nItems;

	int line = 1;
	printf("%d:\t", line);
	for(int i = 0; i < bytes; i++)
	{
		printf("%c", buffer[i]);
		if(buffer[i] == '\n')
		{
			line++;
			printf("%d:\t", line);
		}

	}
	printf("\n\n");
	return bytes;

}




void doGet(char* url)
{
	curl_global_init(CURL_GLOBAL_ALL);


	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();

	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	

	curl_easy_cleanup(curl);	
	}
}

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t retcode;
	curl_off_t nread;

	retcode = fread(ptr, size, nmemb, stream);
	nread = (curl_off_t)retcode;
	
	fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T " bytes from file\n", nread);

	return retcode;
}


void doPut(char* url, char* str)
{
	CURL *curl;
	CURLcode res;
	FILE * src;
	struct stat file_info;

	char * file;
	file = "myFile.txt";

	stat(file, &file_info);

	src = fopen(file, "r+");
	curl = curl_easy_init();

	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_READDATA, src);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		curl_easy_cleanup(curl);

	}

	fclose(src);
	curl_global_cleanup();


}

void doDelete(char * url, char * str)
{
	CURL * curl;
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"key\": \"value\"}");
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "content-type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		
	}
}


int checkSpecialChars(char str[], int n){
   int i, flag = 0;
   for (i = 0; i < n; ++i){
      //checking each character of the string for special character.
      if(str[i] == '!' || str[i] == '@' || str[i] == '#' || str[i] == '$'
      || str[i] == '%' || str[i] == '^' || str[i] == '&' || str[i] == '*'
      || str[i] == '(' || str[i] == ')' || str[i] == '{'
      || str[i] == '}' || str[i] == '[' || str[i] == ']'
      || str[i] == ';' || str[i] == '\'' || str[i] == '<'
      || str[i] == '>'  || str[i] == '?'
      || str[i] == '~' || str[i] == '`' ){
         printf("Special chars in string is not allowed:\n");
	 printf("%s\n",str);
         flag = 1;
         break;
      }
   }
   //if there is no special charcter

   if(flag == 1)
	   printf("For a full list of the characters not allowed, please use the -h or --help option\n");
  

   return flag;
}



int checkStrLength(char * str)
{
	int counter = 0; 
	for(int i = 0 ; i < strlen(str); i++)
	{
		counter++;
	}

	int val = 0;
	if(counter > 20)
		val = 222;
	else
		val = 111;

}

int main(int argc, char *argv[])
{

		//List of all possible options
	char optionList[10][8] = {"-o","--post","-g","--get","-p","--put","-d","--delete","-h","--help"};

	char * str = argv[argc - 1];
	int strCheck = checkStrLength(str);
	if(strCheck == 222)
	{
		printf("The string provided may only be a maximum of 20 characters\n");
		return 0;
	}


	int strLength = strlen(str);
	int charCheckString =  checkSpecialChars(str, strLength);

	if(charCheckString == 1)
	{
		printf("Terminated.\n");
		return 0;
	}
	


	char* verb;
	int code = 0;
	int count = 0;
	int verbChars = 0;
	int verbCheck = 0;
	//Loop through 'optionList'
	for(int i = 0 ; i < 10; i++)
	{
		//Loop through argv to find verb
		for(int j = 0; j < argc; j++)
		{
			if(strcmp(optionList[i], argv[j]) == 0)
			{
				verb = optionList[i];
				verbChars = strlen(verb);
				verbCheck = checkSpecialChars(verb, verbChars);
				if(verbCheck == 1)
				{
					printf("Terminated.\n");
					code = 911;
					break;
				}
				count++;
				//Assign verb a code
				if(strcmp(verb,"-h") == 0 || strcmp(verb,"--help") == 0)
				{
					code = 1;
				
				}

				if(strcmp(verb,"-o") == 0 || strcmp(verb,"--post") == 0)
				{
					code = 2;
				}

				if(strcmp(verb,"-g") == 0 || strcmp(verb,"--get") == 0)
				{
					code = 3;
				}

				if(strcmp(verb,"-p") == 0 || strcmp(verb,"--put") == 0)
				{
					code = 4;
				}

				if(strcmp(verb,"-d") == 0 || strcmp(verb,"--delete") == 0)
				{
					code = 5;
				}

			}
		}
		if(code == 911)
			break;

	}

	if(code == 911)
		return 0;

	//Too many verbs
	if(count > 1 && code != 1)
		code = 6;

	//Too few verbs
	if(count == 0 && code != 1)
		code = 7;


	//Get URL
	char* url;
	int urlCount = 0;
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "--url") == 0 || strcmp(argv[i], "-u") == 0)
		{
			url = argv[i+1];	
			urlCount++;
			if(urlCount > 1)
			{
				printf("You may only enter one URL. Use -h or --help options to view help screen.");
				return 0;
			}
		}

	}



	//Handle verb code
	switch(code)
	{
		case 1:
			doHelp();
			break;

		case 2:
			doPost(url, str);
			break;

		case 3:
			doGet(url);
			break;

		case 4:
			doPut(url, str);
			break;

		case 5: 
			doDelete(url, str);
			break;

		case 6: 
			printf("Enter only one option (-o, --post, -g, --get, -p, --put, -d, --delete) at a time\n");
			break;

		case 7:
			printf("You must enter an option(-o, --post, -g, --get, -p, --put, -d, --delete)\n");
			break;


		default:
			break;
	}


	return 0;
}	
