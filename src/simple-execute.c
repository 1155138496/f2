#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int shell_execute(char ** args, int argc)
{
	int child_pid, wait_return, status;

	if ( strcmp(args[0], "EXIT") == 0 )
		return -1; 

		if ( strcmp(args[0], "cd") == 0 ){
			if(chdir(args[1])<=0)
			printf("cd: %s: %s", args[1],strerror(errno));
		return 0;}
	
	
	if( (child_pid = fork()) < 0 ){
		printf("fork() error \n");
	}else if (child_pid == 0 ){
		if ( execvp(args[0], args) < 0){ 
			printf("execvp() error \n");
			exit(-1);
		}
	}else{
		if ( (wait_return = wait(&status) ) < 0 )
			printf("wait() error \n"); 
	}



int main(int argc, char *argv[]) {
  char line[1024];

  while (1) {
    printf("$ ");
    if (fgets(line, 1024, stdin) == NULL) {
      break;
    }

    int i;
    for (i = 0; i < strlen(line); i++) {
      if (line[i] == '|') {
        line[i] = '\0';
        break;
      }
    }

    char *command1 = line;
    char *command2 = &line[i + 1];

    int pipefd[2];
    if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(1);
    }

    int pid1 = fork();
    if (pid1 == -1) {
      perror("fork");
      exit(1);
    }

    if (pid1 == 0) {
      close(pipefd[0]);
      dup2(pipefd[1], 1);
      close(pipefd[1]);
      execlp(command1, command1, (char *) NULL);
      perror("execlp");
      exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
      perror("fork");
      exit(1);
    }

    if (pid2 == 0) {
      close(pipefd[1]);
      dup2(pipefd[0], 0);
      close(pipefd[0]);
      execlp(command2, command2, (char *) NULL);
      perror("execlp");
      exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
  }



  

// Parses the input string and returns the command and its arguments
int parse_input(char *input, char **command1, char **command2) {
  int i, j;
  for (i = 0; input[i] != '\0' && input[i] != '|'; i++) {
    if (input[i] == ' ') {
      input[i] = '\0';
      break;
    }
  }
  if (input[i] == '\0') {
    *command1 = input;
    *command2 = NULL;
    return 0;
  }
  input[i] = '\0';
  *command1 = input;
  *command2 = input + i + 1;
  return 1;
}

int main(int argc, char *argv[]) {
  char input[MAX_LINE_SIZE];
  char *command1, *command2;
  int pipe_present;

  while (1) {
    printf(">>> ");
    if (fgets(input, MAX_LINE_SIZE, stdin) == NULL) {
      break;
    }

    // Removing the newline character from the input string
    input[strlen(input) - 1] = '\0';

    // Parsing the input string
    pipe_present = parse_input(input, &command1, &command2);

    // If there's no pipe, we can simply run the command as usual
    if (!pipe_present) {
      pid_t pid = fork();
      if (pid == 0) {
        // This is the child process
        char *args[MAX_ARG_NUM];
        int i;
        args[0] = command1;
        for (i = 1; i < MAX_ARG_NUM; i++) {
          args[i] = strtok(NULL, " ");
          if (args[i] == NULL) {
            break;
          }
        }
        execvp(command1, args);
        exit(1);
      } else {
        // This is the parent process
        wait(NULL);
      }
    } else {
      // If there's a pipe, we need to create two processes and connect their
      // stdout and stdin using a pipe
      int fd[2];
      if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(1);
      }

      pid_t pid1 = fork();
      if (pid1 == 0) {
        // This is the first child process
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        char *args[MAX_ARG_NUM];
        int i;
        args[0] = command1;
        for (i = 1; i < MAX_ARG_NUM; i++) {
          args[i] = strtok(NULL, " ");
          if (args[i] == NULL) {
            break;
          }
        }
        execvp(command1, args);
        exit(1);
      } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {
          // This is the second child process
          close(fd[1]);
          dup2(fd[0], STDIN_FILENO);
          close(fd[0]);

          char *args[MAX_ARG_NUM];
          int i;
          args[0] = command2;
          for (i = 1; i < MAX_ARG_NUM; i++) {
            args[i] = strtok(NULL, " ");
            if (args[i] == NULL) {
              break;
            }
          }
          execvp(command2, args);
          exit(1);
        } else {
          // This is the parent process
          close(fd[0]);
          close(fd[1]);
          wait(NULL);
          wait(NULL);
        }
      }
    }
  }


}



	/* wait for child process to terminate */
	while((wait_return = wait(&status)) > 0);		
	return 0;

}
