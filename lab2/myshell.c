#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/types.h>
#include <pwd.h>
#define BUFFER_LEN 1024

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
 char* argv[100]; 
 char* bin= "/bin/";
 char path[1024]; 
 int argc; 

	while(1){
		long size;
		char *buf;
		char *ptr;
		
		const char *homedir;

		if ((homedir = getenv("HOME")) == NULL) {
    		homedir = getpwuid(getuid())->pw_dir;
		}
		/*printf("%s\n", homedir);*/
		if (chdir(homedir)==-1) printf("Cannot start from %s \n", homedir);
		/*else printf("We are at %s\n", homedir);*/

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
 		
 		if (strcmp(line, "exit") == 0) exit(0); 

 		
 		argc = build_args (line,argv); 
		
		set_program_path (path,bin,argv[0]); 
 		
 		int pid= fork(); 
 			
 		if(pid==0){ 
 				execve(path,argv,0); 
 				fprintf(stderr, "Child process could not do execve\n");
 			}
 			else wait(NULL); 
		}
	return 0;
}

