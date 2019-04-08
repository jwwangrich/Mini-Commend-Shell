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
 
//this function is to parse the command line argument into a vector of strings by using space to separate
//this function put the line into a stringstream and extract into a string everytime encoutering a space
//The '\ ' is not counted as separation. Therefore, need to check last char in the string as '\\'
//if yes, we immediately change '\\' into ' ', then the extraction should not go to a new string, instead, should append to the last extraction

void parse(std::vector<std::string> & parsedCmd, std::string & inputCmd) {
  std::string::iterator it = inputCmd.begin();
  std::string tempStr;  
  while (it != inputCmd.end()) {
    std::string token(1, *it);
    if (token == " ") {
      if (tempStr != "") {
	parsedCmd.push_back(tempStr);
	tempStr = "";
      }
    } else if (token == "\\") {
      ++it;
      if (it != inputCmd.end()) {
	tempStr = tempStr + *it;
      }
    } else {
      tempStr = tempStr + token;
    }
    ++it;
  }
  if (tempStr != "") {
    parsedCmd.push_back(tempStr);
    tempStr = "";
  }
  return;
}

void parseVar (std::string & inputCmd,
	       std::map<std::string, std::string> & myMap) {
  //looking for '$' for varname
  size_t pos = inputCmd.find("$");
  //find $
  if (pos != std::string::npos) {
    //if found keyName(variable),put the var name into map  
    //search for var value in map
    //if found, replace the value corresponding to the keyName(variable) 
    //if not found var value, put value into keyName in myMap 
    size_t spaPos = inputCmd.find(' ', pos + 1);
    if (spaPos == 0) {
      std::cerr << "$ should have potential changing variable value" << std::endl;
      return;
    }
    std::string keyName = inputCmd.substr(pos + 1, spaPos - 1 - pos);
    std::string valName = inputCmd.substr(spaPos + 1);
    if (myMap.find(keyName) != myMap.end()) {
      myMap.at(keyName) = valName;     
    }
    else {
      myMap[keyName] = valName;
    }
  }
  //if not found $, add the keyName into map
  else {
    size_t first_spaPos = inputCmd.find(' ');
    size_t second_spaPos = inputCmd.find_first_of(' ', first_spaPos + 1);
    std::string keyName;
    std::string valName;
    if (second_spaPos == 0) {
      keyName = inputCmd.substr(first_spaPos + 1);
      valName = "";
    }
    else {
      keyName = inputCmd.substr(first_spaPos + 1, second_spaPos - 1 - first_spaPos);
      valName = inputCmd.substr(second_spaPos + 1);
      myMap[keyName] = valName;
    }
    std::cout << "debug: success in parseVar" << std::endl;
    // for(auto it = myMap.cbegin(); it != myMap.cend(); ++it) {
    //   std::cout << it->first << " :debug key<<myMap>>value " << it->second << std::endl;
    //   std::cout <<"sizeof: "<< it->first << "|||||" << it->first.size() << std::endl;
    // }    
  }
}


//int 1 >> valid, int -1 >> invalid
int checkVar(std::string myName) {
  if ((myName[0] >= 'a' && myName[0] <= 'z') ||
      (myName[0] >= 'A' && myName[0] <= 'Z') ||
      (myName[0] == '_' ||
       myName[0] == '$')) {
    for (size_t i = 1; i < myName.size(); i++) {
      if ((myName[i] >= 'a' && myName[i] <= 'z') ||
	  (myName[i] >= 'A' && myName[i] <= 'Z') ||
	  (myName[i] == '_')) {
	return 1;
      }
      else {
	return -1;
      }
    }
  }
  else {
    return -1;
  }
  return 1;
}


void builtInFunction (std::vector<std::string> & parsedCmd,
		      std::string & inputCmd,
		      std::map<std::string, std::string> & myMap) {
  //cd 
  if (parsedCmd[0] == "cd") {
    if (parsedCmd.size() == 2) {
      std::string cdPATH = parsedCmd[1];
      if (cdPATH == "~") {
	cdPATH = getenv("HOME");
      }
      else {
	int cdID = chdir(cdPATH.c_str());
	if (cdID == -1) {
	  std::cerr << "ERROR usage: fail in cd command." << std::endl;
	}
      }
    }
    else {
      std::cerr << "ERROR usage: cd should have 1 argument." << std::endl;
    }
    //this is tricky one
    char * cwd = get_current_dir_name();
    setenv("PWD", cwd, 1);
    free(cwd);    
  }
  
  //set
  if (parsedCmd[0] == "set") {
    if (parsedCmd.size() == 3) {
      int k = checkVar(parsedCmd[1]);
      if (k == -1) {
	std::cerr << "invald variable name" << std::endl;
	exit(EXIT_FAILURE);
      }
      else {
	parseVar(inputCmd, myMap);
      }
    }
    else if (parsedCmd.size() == 2) {
      parseVar(inputCmd, myMap);
      //myMap[parsedCmd[1]] = "";
    }
    else {
      std::cerr << "ERROR usage: set should have 1 or 2 arguments" << std::endl;      
    }
  }	
  
  //export
  if (parsedCmd[0] == "export") {
    if (parsedCmd.size() != 2) {
      std::cerr << "ERROR usage: export should have only 1 arguments" << std::endl;
    }
    else {
      std::map<std::string, std::string>::iterator it;
      // std::cout << "sizeof: " << parsedCmd[1] << "|||||" << parsedCmd[1].size() << std::endl;
      
      // for(auto it = myMap.cbegin(); it != myMap.cend(); ++it) {
      // 	std::cout << it->first << "--debug key<<myMap>>value--" << it->second << std::endl;
      // }     
      it = myMap.find(parsedCmd[1]);
      if (it != myMap.end()) {
	setenv(it->first.c_str(), it->second.c_str(), 1);
	myMap.erase(it);
      }
      else {
	std::cerr << "ERROR usage--export: variable not exist" << std::endl;
      }
    }
  }
}

      
bool readCommand(std::string & inputCmd) {
  getline(std::cin, inputCmd);
  if (std::cin.eof()) {
    std::cerr << "End Of File" << std::endl;
    return false;
  }
  if (inputCmd == "exit") {
    std::cout << "Command is exit" << std::endl;
    return false;
  }
  return true;
}

void ChildPro(std::vector<std::string> & parsedCmd) {
  std::vector<char *> argCmd(parsedCmd.size() + 1);    // one extra for the null
  for (size_t i = 0; i != parsedCmd.size(); i++) {
      argCmd[i] = &parsedCmd[i][0];
  }
  argCmd[parsedCmd.size()] = NULL;
  if (execvp(argCmd[0], argCmd.data()) == -1) {
    std::cout<< "Command " << argCmd[0] << " not found" << std::endl;
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < argCmd.size() + 1; i++) {
    delete[] (argCmd[i]);
  }
  argCmd.clear();
  exit(EXIT_SUCCESS);
}

void envinMap(std::map<std::string, std::string> & myMap) {
  //char * curr = environ[0];
  for (int i = 0; environ[i] != NULL; i++) {
    std::string var = std::string(environ[i]);
    std::string key = "";
    std::string value = "";
    size_t pos = var.find("=");
    if (pos != std::string::npos) {
    key = var.substr(0, pos);
    value = var.substr(pos + 1);
    myMap[key] = value;
    }
  }
}

int main(int argc, char * argv[]) {
  std::map<std::string, std::string> myMap;
  envinMap(myMap);
  while (true) {
    std::cout << "myShell:" << getenv("PWD") << "$";
    std::vector<std::string> parsedCmd;
    std::string inputCmd;
    if(!readCommand(inputCmd)) {
      exit(EXIT_SUCCESS);
      continue;
    }
    std::cout << "after readcommand : " << inputCmd << std::endl;
    parse(parsedCmd, inputCmd);
    // std::cout << "after parse :" << parsedCmd[0] << std::endl;
    // for(auto it = myMap.cbegin(); it != myMap.cend(); ++it) {
    // std::cout << it->first << " :debug key<<myMap>>value " << it->second << std::endl;
    // }
    if (parsedCmd[0] == "cd" ||
    	parsedCmd[0] == "set" ||
    	parsedCmd[0] == "export") {
      builtInFunction(parsedCmd, inputCmd, myMap);
      continue;
    }
    //searchPath(myPath);
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
      printf("Child PID is %ld\n", (long) getpid());      
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
  exit(EXIT_SUCCESS);
}
