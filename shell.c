#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

void handler(int){
    pid_t pid =getpid();
    kill(pid,SIGKILL);
}
char* printDirectory(){
    size_t maxbuf = 1000;
    char path[maxbuf];
    char* cwd;
    cwd = getcwd(path,maxbuf);
    return cwd;

}

char* read_line(){
    char* buffer;
    size_t bufsize = 100;
    size_t line;
    buffer = (char*)malloc(bufsize * sizeof(char));
    if(buffer == NULL){
        printf("Couldnt allocate memory.\n");
        exit(1);
    }
    line = getline(&buffer,&bufsize,stdin);

    return buffer;
}

char** parse(char* input){
    int all_tokens_buffer = 100;
    char** all_tokens = (char**)malloc(all_tokens_buffer * sizeof(char*));
    int all_tokens_counter = 0;

    int current_token_buffer = 1024;
    char* current_token = (char*)malloc(current_token_buffer * sizeof(char));
    int current_token_counter = 0;

    for(int i=0;i<strlen(input);i++){
        char character = input[i];
        if(character == ' '){
            if(all_tokens_counter + 1 > all_tokens_buffer){
                all_tokens_buffer += 100;
                all_tokens = (char**)realloc(all_tokens,all_tokens_buffer * sizeof(char*));
            }
            if(current_token_counter + 1 > current_token_buffer){
                current_token_buffer += 1024;
                current_token = (char*)realloc(current_token,current_token_buffer);

            }
            all_tokens[all_tokens_counter] =  current_token;
            all_tokens_counter++;

            current_token = (char*)malloc(current_token_buffer * sizeof(char*));
            current_token_counter = 0;
        }
        else if(character == '\n'){
            if(all_tokens_counter + 2 > all_tokens_buffer){
                all_tokens_buffer += 100;
                all_tokens = (char**)realloc(all_tokens,all_tokens_buffer*sizeof(char*));
            }
            if(current_token_counter + 1 > current_token_buffer){
                current_token_buffer += 1024;
                current_token = (char*)realloc(current_token,current_token_buffer);
            }
            all_tokens[all_tokens_counter] = current_token;
            all_tokens_counter++;

            all_tokens[all_tokens_counter] = NULL;
            break;
        }else{
            if(current_token_counter + 1 > current_token_buffer){
                current_token_buffer += 1024;
                current_token = (char*)realloc(current_token,current_token_buffer);
            }
            current_token[current_token_counter] = character;
            current_token_counter++;
        }
    }
    return all_tokens;
}

void execute(char** args){
    pid_t childpid;
    int status;

    if(strcmp(args[0],"cd") == 0){
        chdir(args[1]);
        printf("Changing directories to: %s\n",args[1]);


    }else{

        if((childpid =fork()) == -1){
                fprintf(stderr,"cant fork");
                exit(0);           }
        if(childpid == 0){
            execvp(args[0],args);
            exit(1);
        }
        else{
            waitpid(childpid,&status,WUNTRACED | WCONTINUED);
        }
    }
 }


void printPrompt(){
    char* cwd = printDirectory();
    printf("%s>", cwd);
}


int main(){


    while(1){
        printPrompt();
        signal(SIGINT,handler);
        char* input = read_line();
        char** parsed = parse(input);

        execute(parsed);
        wait(NULL);

        free(input);
        free(parsed);


    }
     return 0;



}
