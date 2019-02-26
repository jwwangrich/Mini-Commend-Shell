#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <sys/types.h>
#include <map>

#include "BuiltInFunction.h"
#include "ReadCommand.h"
#include "ParseSetCommand.h"
#include "ParseCommand.h"

/*------------------------------------------myShell main function---------------------------------------------*/
/*Step 1: Set up myMap.
  
  Step 2: In Loop, print myShell: "PWD"$
  
  Step 3: Read command to check not "EOF", if not, parse command and save into string vector. If it is "exit",
          this program will terminate like encountering "EOF".

  Step 4: If is Built-In-Function, execute them.

  Step 5: fork child process and waitpid to execute Child Process.
*/

int main(int argc, char * argv[]) {
  std::map<std::string, std::string> myMap;
  EnvInMap(myMap);

  while (true) {
    std::cout << "myShell:" << getenv("PWD") << "$";
    std::vector<std::string> parsedCmd;
    std::string inputCmd;
    
    if (!ReadCommand(inputCmd, parsedCmd)) {
      exit(EXIT_SUCCESS);
      continue;
    }

    if (parsedCmd[0] == "cd" ||
	parsedCmd[0] == "set" ||
	parsedCmd[0] == "export") {
      BuiltInFunction(parsedCmd, inputCmd, myMap);
      continue;
    }
    
    pid_t cpid;
    pid_t w;
    int status;
    
    cpid = fork();
     
    if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    //execute child process
    if (cpid == 0) {            /* Code executed by child */
      //printf("Child PID is %ld\n", (long) getpid());      
      ChildPro(parsedCmd);
      if (argc == 1)
	pause();                    /* Wait for signals */
      _exit(atoi(argv[1]));
    }
    else {                    /* Code executed by parent */
      w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
      if (w == -1) {
	perror("waitpid");
	exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
	printf("Program exited with status %d\n", WEXITSTATUS(status));
      }
      else if (WIFSIGNALED(status)) {
	printf("Program was killed by signal %d\n", WTERMSIG(status));
      }
      else if (WIFSTOPPED(status)) {
	printf("stopped by signal %d\n", WSTOPSIG(status));
      }
      else if (WIFCONTINUED(status)) {
	printf("continued\n");
      }
    }while (!WIFEXITED(status) && !WIFSIGNALED(status));    
  }
  // for (std::map<std::string, std::string>::iterator it = myMap.begin();
  //      it != myMap.end();
  //      it++) {
  //   it.clear();
  // }
  //  std::string::~string();
  exit(EXIT_SUCCESS);
}
