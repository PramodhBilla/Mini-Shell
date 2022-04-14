#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;


#define MAXLIST 100 // maximum number of letters to be supported
#define MAXCOM 1000 // maximum number of commands to be supported

void prtDir(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s:~$ ", cwd);
}

void prthistory(){
    string line;
    ifstream infile;
    infile.open("temp/history.txt");
    while(infile>>line){
        cout<<line<<endl;
    }
    infile.close();
}
void clearhistory(){
    ofstream outfile;
    outfile.open("temp/history.txt" );
    outfile<<"";
}


// Function where the system command is executed
void execArgs(char** parsed)
{
    // child Forking
    pid_t pid = fork();

    if (pid == 0)
    {    // for the child process:

        // function for redirection ( '<' , '>' , '>>' )
        int fd0,fd1,i,in=0,out=0,append=0;
        char input[64],output[64],outappend[64];

        // finds where '<' , '>' or '>>' occurs and make that parsed[i] = NULL , to ensure that command wont't read it

        for(i=0;parsed[i]!=NULL;i++)
        {

              if(strcmp(parsed[i],">>")==0)
            {
                parsed[i]=NULL;
                strcpy(outappend,parsed[i+1]);
                append=2;
            }

            if(strcmp(parsed[i],">")==0)
            {
                parsed[i]=NULL;
                strcpy(output,parsed[i+1]);
                out=2;
            }
            if(strcmp(parsed[i],"<")==0)
            {
                parsed[i]=NULL;
                strcpy(input,parsed[i+1]);
                in=2;
            }


        }

        //if '>>' char was found in string inputted by user
        if (append)
        {
            int fd2 ;
            if ((fd2 = open(outappend , O_WRONLY|O_CREAT|O_APPEND, 0644)) < 0) {
                perror("Couldn't open the output file");
                exit(0);
            }
            dup2(fd2, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
            close(fd2);
        }

        //if '<' char was found in string inputted by user
        if(in)
        {
            // fdo is file-descriptor
            int fd0;
            if ((fd0 = open(input, O_RDONLY, 0)) < 0) {
                perror("Couldn't open input file");
                exit(0);
            }
            // dup2() copies content of fdo in input of preceeding file
            dup2(fd0, 0); // STDIN_FILENO here can be replaced by 0
            close(fd0);
        }

        //if '>' char was found in string inputted by user
        if (out)
        {
            int fd1 ;
            if ((fd1 = creat(output , 0644)) < 0) {
                perror("Couldn't open the output file");
                exit(0);
            }
            dup2(fd1, STDOUT_FILENO); // 1 here can be replaced by STDOUT_FILENO
            close(fd1);
        }

        execvp(parsed[0], parsed);
        perror("execvp");
        _exit(1);


    }

    else if((pid) < 0)
    {
        printf("fork() failed!\n");
        exit(1);
    }

    else {
        while (!(wait(NULL) == pid)) ;
    }

}

// Function in which  piped system commands is executed
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }

    if (p1 != 0) {

        p2 = fork();

        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
            exit(EXIT_SUCCESS);
        } else {
            // parent executed, waiting for two children
            //waiting for both children
            wait(NULL);
            //one child executed
            wait(NULL);
            //both children executed
        }


    } else {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
        exit(EXIT_SUCCESS);

    }

}


// Help command :builtin
void openHelp()
{
    puts("\n***WELCOME TO MY SHELL HELP***"
        "\n-Use the shell at your own risk..."
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling");

    return;
}

void setEnvVar(char** parsed){

}
void PrintEnv(char** parsed){
    cout<<getenv(parsed[1]);

    cout<<endl;
}


// Function to execute own commands
int ownCmdHandler(char** parsed)
{
    int NoOfOwnCmds = 14, i, switchOwnArg = 0;
    string list[NoOfOwnCmds]={
        "exit","quit","x","cd","history",
        "HOME","USER","SHELL","TERM","help","hello","setenv","prtenv"
    };
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;

    for(i = 0; i < NoOfOwnCmds; i++)
         ListOfOwnCmds[i] = &list[i][0];

    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }


        if(switchOwnArg==1){
            cout<<"GoodBye"<<endl;
            exit(0);

        }
         else if(switchOwnArg==2){
             cout<<"GoodBye"<<endl;
            exit(0);}
       else if(switchOwnArg==3){
             cout<<"GoodBye"<<endl;
            exit(0);}

        else if(switchOwnArg==4){
            if(parsed[1]==NULL){chdir(getenv("HOME"));}
            chdir(&parsed[1][0]);
            return 1;}
        else if(switchOwnArg==5){
            openHelp();
            return 1;}
        else if(switchOwnArg==6){
            username = getenv("USER");
            printf("Hello %s.\n",username);
            return 1;}
        else if(switchOwnArg==7){
            setEnvVar(parsed);
            return 1;}
        else if(switchOwnArg==8){
            PrintEnv(parsed);
            return 1;}

            if(switchOwnArg==9){
            prthistory();
            return 1;}
        else if(switchOwnArg==10){
            cout<<getenv("HOME")<<endl;
            return 1;}
        else if(switchOwnArg==11){
            cout<<getenv("USER")<<endl;
            return 1;}
        else if(switchOwnArg==12){
            cout<<getenv("SHELL")<<endl;
            return 1;}
        else if(switchOwnArg==13){
            cout<<getenv("TERM")<<endl;
            return 1;}

     return 0;
    }




// function for finding pipe
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] != NULL)
        return 1;
    else {
        return 0;//pipe not found!
    }
}


void parseSpace(char* str, char** parsed)
{
    int i=0;

    while(i<MAXLIST) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0){
            i--;}
            i++;
    }
}

int processString(char* str, char** parsed, char** parsedpipe){
    char* stripped[2];
    int piped=0;

    piped = parsePipe(str, stripped);

    if (piped) {
        parseSpace(stripped[0],parsed);
        parseSpace(stripped[1],parsedpipe);
    }else{
        parseSpace(str, parsed);
    }

    if (!ownCmdHandler(parsed)){
        return 1 + piped;}
    else
         {return 0;}
}

int main(){
    string input;
    int execFlag = 0;
    char *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    clearhistory();
    while (true)
    {

        prtDir();

        getline(cin,input);

        execFlag= processString(&input[0], parsedArgs, parsedArgsPiped);

        if (execFlag ==1){
            execArgs(parsedArgs);}

        if (execFlag == 2){
            execArgsPiped(parsedArgs, parsedArgsPiped);}

        ofstream outfile;
        outfile.open("temp/history.txt",std::ios_base::app );
        outfile<<input<<endl;
        outfile.close();
    }

    return 0;
}
