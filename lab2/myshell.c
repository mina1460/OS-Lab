//fix env variable display using echo $var
//word expansion



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>
#define BUFFER_LEN 1024

extern char **environ;


char* replaceWord(const char* s, const char* oldW, 
                  const char* newW) 
{ 
    char* result; 
    int i, cnt = 0; 
    int newWlen = strlen(newW); 
    int oldWlen = strlen(oldW); 
  
    // Counting the number of times old word 
    // occur in the string 
    for (i = 0; s[i] != '\0'; i++) { 
        if (strstr(&s[i], oldW) == &s[i]) { 
            cnt++; 
  
            // Jumping to index after the old word. 
            i += oldWlen - 1; 
        } 
    } 
  
    // Making new string of enough length 
    result = (char*)malloc(i + cnt * (newWlen - oldWlen) + 1); 
  
    i = 0; 
    while (*s) { 
        // compare the substring with the result 
        if (strstr(s, oldW) == s) { 
            strcpy(&result[i], newW); 
            i += newWlen; 
            s += oldWlen; 
        } 
        else
            result[i++] = *s++; 
    } 
  
    result[i] = '\0'; 
    return result; 
} 

void set_cmd(int argc, char *argv[])
{
	int i;
	if (argc != 1)
	printf("Extra args\n");
	else
	for (i = 0; environ[i] != NULL; i++)
	printf("%s\n", environ[i]);
}
void asg(int argc, char *argv[])
{
	char *name, *val;
	if (argc != 1)
		printf("Extra args\n");
	else {
		name = strtok(argv[0], "=");
		val = strtok(NULL, ""); /* get all that's left */
		if (name == NULL || val == NULL)
			printf("Bad command\n");
		else
			setenv(name, val, true);
	}
return;
}

size_t read_command(char *cmd) 
{
	if(!fgets(cmd, BUFFER_LEN, stdin)) 
 		return 0; 
 	
 	size_t length = strlen(cmd); 
 	
 	if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; 
	
	return strlen(cmd); 
}

int build_args(char * cmd, char ** argv) {
 char *token; 
 token = strtok(cmd," ");
 int i=0;
 while(token!=NULL){
 	argv[i]=token; 
 	token = strtok(NULL," "); 
 	i++; 
 }
 argv[i]=NULL; 
	return i; 
}


void set_program_path (char * path, char * bin, char * prog) {
	
	memset (path,0,1024); 
	strcpy(path, bin); 
 	strcat(path, prog); 
	for(int i=0; i<strlen(path); i++) 
		if(path[i]=='\n') path[i]='\0';
}


int main(){
 char line[BUFFER_LEN]; 
 char line2[BUFFER_LEN];
 char* argv[100]; 
 char* bin= "/bin/";
 char path[1024]; 
 int argc; 

 int special_count = 3;
 char *commands[] = {
  "cd", 
  "clear", 
  "set"
};
		const char *homedir;

		if ((homedir = getenv("HOME")) == NULL) {
    		homedir = getpwuid(getuid())->pw_dir;
		}
		/*printf("%s\n", homedir);*/
		if (chdir(homedir)==-1) printf("Cannot start from %s \n", homedir);
		/*else printf("We are at %s\n", homedir);*/

	while(1){
		long size;
		char *buf;
		char *ptr;
		
		
		size = pathconf(".", _PC_PATH_MAX);
		if ((buf = (char *)malloc((size_t)size)) != NULL)
    		ptr = getcwd(buf, (size_t)size);
		char * user_name = getenv("USER");

		printf("[%s@My shell]——[%s]\n$ ", user_name, ptr); 
		if (read_command(line) == 0 )
		{
			printf("\n"); 
			exit(0);
		} 
 		strcpy(line2, line); 
 		
 		if (strcmp(line, "exit") == 0) exit(0); 

 		//replace $var with its values and remove $var from the string 

 		if(strchr(line, '$') != NULL){
			char *bef, *var;
			char* content;
			bef = strtok(line2, "$");
			var = strtok(NULL, " "); 
			//printf("before: %s\n", bef);
			//printf("after: %s\n", var);
			content = getenv(var);
			//printf("var content: %s\n", content);
			char v[] = "$";
			strcat(v, var);
			//printf("v: %s\n", v); 
			//printf("%s\n", line);
			//printf("%s\n", replaceWord(line, v, content));
			strcpy( line,replaceWord(line, v, content));
			//printf("new command: %s\n", line);
		}
//check for input redirection
		strcpy(line2, line);
		char *infile;
		if(strchr(line2, '<') != NULL){
			
			char* bef;
			bef = strtok(line2, "<");
			//printf("bef: %s\n", bef);
			infile = strtok(NULL, " "); 
			//printf("filename: %s\n", file);			
		
			strcpy( line,replaceWord(line, "<", " "));
			strcpy( line,replaceWord(line, infile, " "));
			//printf("here f2: %s\n", line);
		}
		

		char* outfile;
		strcpy(line2, line);

		if(strstr(line2, ">>") != NULL){
			
			char* bef;
			
			outfile = strtok(strstr(line2, ">>")+2, " "); 
			//printf("filename: %s\n", outfile);			
		
			strcpy( line,replaceWord(line, ">>", " "));
			strcpy( line,replaceWord(line, outfile, " "));
			//printf("new command: %s\n", line);
		}
		else if (strchr(line2, '>') != NULL){
			
			char* bef;
			bef = strtok(line2, ">");
			//printf("bef: %s\n", bef);
			outfile = strtok(NULL, " "); 
			//printf("filename: %s\n", file);			
		
			strcpy( line,replaceWord(line, ">", " "));
			strcpy( line,replaceWord(line, outfile, " "));
			//printf("here f2: %s\n", line);
		}
		
		
 		
 		argc = build_args (line,argv); 
 		
 		//printf("%s\n",argv[0]);
 		int i = 0;
 		bool  set = false;
		for (i = 0; i < special_count; ++i)
		{
			if(strcmp(argv[0],commands[i]) == 0){
				set = true;
				break;
			}
		}
		int x;
		if (set)
		{
			switch(i){
			case 0: 
			
				if (argv[1] == NULL)
					x = chdir(homedir);
				
				else
					x = chdir(argv[1]);
				
					if (x != 0)
						perror("could not change directory\n");
				break;
				case 1:
					system("cls||clear");
					break;
				case 2:
					set_cmd(argc, argv);
					break;
			default:
				break;

		}
		}
		else{
		
		set_program_path (path,bin,argv[0]); 
 		
		if (strchr(argv[0], '=') != NULL)
			asg(argc, argv);
		
 		else{
 			int pid= fork(); 	
 			if(pid==0)
 			{ 
 				execve(path,argv,0); 
 				fprintf(stderr, "Child process could not do execve\n");
 			}
 			else wait(NULL); 
		}
	}
		}
	return 0;
}

