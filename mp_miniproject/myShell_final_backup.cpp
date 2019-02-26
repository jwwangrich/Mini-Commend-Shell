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


// /*---------------------------------------------ParseCommand-------------------------------------------------*/
// /*This function is to parse the command line argument into a vector of strings by using space to separate.

//   This function put the line in the temperary string and use iterator to check the token ' '(space) and '\\'.

//   The '\ ' is not counted as separation. This function also check the last char in the string as '\\'.
// */

// void ParseCommand(std::vector<std::string> & parsedCmd, std::string & inputCmd) {
//   std::string::iterator it = inputCmd.begin();
//   std::string tempStr;  

//   while (it != inputCmd.end()) {
//     std::string token(1, *it);

//     if (token == " ") {
//       if (tempStr != "") {
// 	parsedCmd.push_back(tempStr);
// 	tempStr = "";
//       }
//     } else if (token == "\\") {
//       ++it;
//       if (it != inputCmd.end()) {
// 	tempStr = tempStr + *it;
//       }
//     } else {
//       tempStr = tempStr + token;
//     }
//     ++it;
//   }
//   if (tempStr != "") {
//     parsedCmd.push_back(tempStr);
//     tempStr = "";
//   }
//   return;
// }

// /*-----------------------------------------ParseSetCommand----------------------------------------------*/
// /*Looking for '$' in variable!! 

//   If found keyName(variable), search for variable value in myMap.                                                      

//   1. If found, replace the valName(value) corresponding to the keyName(variable) in myMap.                                
  
//   2. If not found valName(value), create valName(value) with corresponding keyName in myMap.
  
//   If not found '$', add the keyName(variable) into myMap.
  
//   1. If no value provided, valName would be "".

//   2. Otherwise, create a new keyName(variable) with valName(value) in myMap. 
// */

// void ParseSetCommand(std::vector<std::string> & parsedCmd,
// 		     std::string & inputCmd,
// 		     std::map<std::string, std::string> & myMap) {
//   size_t pos = inputCmd.find_first_of("$");
//   std::string setCmd ("set");
//   std::string varCmd (parsedCmd[1]);
//   size_t varPos = inputCmd.find(varCmd);
//   if (pos != std::string::npos) {
//     if (parsedCmd.size() == 2) {
//       std::cerr << "$ should have potential changing variable value" << std::endl;
//       return;
//     }
//     size_t keyName_len = parsedCmd[1].length();
//     std::string keyName = inputCmd.substr(pos + 1, keyName_len - 1); //$ does not count
//     size_t valPos = inputCmd.find(varCmd) + keyName_len + 1;
//     std::string valName = inputCmd.substr(valPos);
//     if (myMap.find(keyName) != myMap.end()) {
//       myMap.at(keyName) = valName;     
//     }
//     else {
//       myMap[keyName] = valName;
//     }
//   }
//   else {
//     size_t first_spaPos = inputCmd.find(' ', varPos - 1);
//     size_t second_spaPos = inputCmd.find_first_of(' ', first_spaPos + 1);
//     std::string keyName;
//     std::string valName;
//     if (parsedCmd.size() == 2) {
//       keyName = inputCmd.substr(first_spaPos + 1);
//       valName = "";
//       myMap[keyName] = valName;
//     }
//     else {
//       keyName = inputCmd.substr(first_spaPos + 1, second_spaPos - 1 - first_spaPos);
//       valName = inputCmd.substr(second_spaPos + 1);
//       myMap[keyName] = valName;
//     }
//     //std::cout << "debug: success in parseVar" << std::endl;
//   }
// }

// /*--------------------------------------------CheckVar--------------------------------------------------*/
// /*To check whether the variable name is a valid name.
  
//   The first char in myName string(variable name) can be '$'.
  
//   The valid names include 'a'->'z', 'A'->'Z' and '_'
  
//   If not in this range, this function drive invalid name return to int "-1", which is the failure sign.

//   On the contrary, if variable name is valid, this function returns to int "1".
// */

// int CheckVar(std::string myName) {
//   if ((myName[0] >= 'a' && myName[0] <= 'z') ||
//       (myName[0] >= 'A' && myName[0] <= 'Z') ||
//       (myName[0] == '_' ||
//        myName[0] == '$')) {
//     for (size_t i = 1; i < myName.size(); i++) {
//       if ((myName[i] >= 'a' && myName[i] <= 'z') ||
// 	  (myName[i] >= 'A' && myName[i] <= 'Z') ||
// 	  (myName[i] == '_')) {
// 	return 1;
//       }
//       else {
// 	return -1;
//       }
//     }
//   }
//   else {
//     return -1;
//   }
//   return 1;
// }

// /*------------------------------------------BuiltInFunction---------------------------------------------*/
// /*This function is used to separate three built-in functions: cd, set and export

//   (1) "cd": when the command is "cd", this function would check whether the potential changing directory is 
//         "~".

//   If is "~", this program would lead to "HOME" directory.

//   (2) "set": when the command is "set", this function check how many arguments inputted.

//   If is 3 arguments, check if the variable names are valid. Then execute set command.
  
//   If is 2 arguments, execute set command by ParseSetCommand function.

//   (3) "export": when the command is "export", this function checks how many arguments are inputted.

//   If not 2 arguments, it is an error.

//   If is 2 arguments, find the variable and put its value into environment for other programs.

//   If 2 arguments but cannot find the variable, it will print variable does not exist.
// */

// void BuiltInFunction(std::vector<std::string> & parsedCmd,
// 		     std::string & inputCmd,
// 		     std::map<std::string, std::string> & myMap) {
//   //cd 
//   if (parsedCmd[0] == "cd") {
//     if (parsedCmd.size() == 2) {
//       std::string cdPATH = parsedCmd[1];
//       if (cdPATH == "~") {
// 	cdPATH = getenv("HOME");
//       }
//       else {
// 	int cdID = chdir(cdPATH.c_str());
// 	if (cdID == -1) {
// 	  std::cerr << "ERROR usage: fail in cd command--No such file or directory" << std::endl;
// 	}
//       }
//     }
//     else {
//       std::cerr << "ERROR usage: cd should have 1 argument." << std::endl;
//     }
//     //this is tricky one
//     char * cwd = get_current_dir_name();
//     setenv("PWD", cwd, 1);
//     free(cwd);    
//   }
  
//   //set
//   if (parsedCmd[0] == "set") {
//     if (parsedCmd.size() > 2) {
//       int k = CheckVar(parsedCmd[1]);
//       if (k == -1) {
// 	std::cerr << "invald variable name" << std::endl;
// 	exit(EXIT_FAILURE);
//       }
//       else {
// 	ParseSetCommand(parsedCmd, inputCmd, myMap);
//       }
//     }
//     else if (parsedCmd.size() == 2) {
//       ParseSetCommand(parsedCmd, inputCmd, myMap);
//       //myMap[parsedCmd[1]] = "";
//     }
//     else {
//       std::cerr << "ERROR usage: set should have 1 or 2 arguments" << std::endl;      
//     }
//   }	
  
//   //export
//   if (parsedCmd[0] == "export") {
//     if (parsedCmd.size() != 2) {
//       std::cerr << "ERROR usage: export should have only 1 arguments" << std::endl;
//     }
//     else {
//       std::map<std::string, std::string>::iterator it;
//       it = myMap.find(parsedCmd[1]);
//       if (it != myMap.end()) {
// 	setenv(it->first.c_str(), it->second.c_str(), 1);
// 	myMap.erase(it);
//       }
//       else {
// 	std::cerr << "ERROR usage--export: variable not exist" << std::endl;
//       }
//     }
//   }
// }

// /*-------------------------------------------------ReadCommand--------------------------------------------------*/
// /*Read command, if is "eof", return false and if is "exit", return false as well.

//   Otherwise, return true.
// */

// bool ReadCommand(std::string & inputCmd,
// 		 std::vector<std::string> & parsedCmd) {
//   getline(std::cin, inputCmd);
//   if (std::cin.eof()) {
//     std::cerr << "End Of File" << std::endl;
//     return false;
//   }
//   ParseCommand(parsedCmd, inputCmd);
//   if (parsedCmd[0] == "exit") {
//     std::cout << "Command is exit" << std::endl;
//     return false;
//   }
  
//   return true;
// }

// /*-------------------------------------------------ChildPro-----------------------------------------------------*/
// /*Execute child process. Use execvp to execute the command.
//  */

// void ChildPro(std::vector<std::string> & parsedCmd) {
//   std::vector<char *> argCmd(parsedCmd.size() + 1);    // one extra for the null
//   for (size_t i = 0; i != parsedCmd.size(); i++) {
//       argCmd[i] = &parsedCmd[i][0];
//   }
//   argCmd[parsedCmd.size()] = NULL;

//   if (execvp(argCmd[0], argCmd.data()) == -1) {
//     std::cout<< "Command " << argCmd[0] << " not found" << std::endl;
//     exit(EXIT_FAILURE);
//   }

//   for (size_t i = 0; i < argCmd.size() + 1; i++) {
//     delete[] (argCmd[i]);
//   }
//   argCmd.clear();
//   exit(EXIT_SUCCESS);
// }

// /*--------------------------------------------------EnvInMap---------------------------------------------------*/
// /*Put all the default environment variables in myMap.
// */

// void EnvInMap(std::map<std::string, std::string> & myMap) {
//   for (int i = 0; environ[i] != NULL; i++) {
//     std::string var = std::string(environ[i]);
//     std::string key = "";
//     std::string value = "";
//     size_t pos = var.find("=");

//     if (pos != std::string::npos) {
//     key = var.substr(0, pos);
//     value = var.substr(pos + 1);
//     myMap[key] = value;
//     }
//   }
// }

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
