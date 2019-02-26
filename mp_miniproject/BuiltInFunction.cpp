#include "BuiltInFunction.h"
#include "ParseSetCommand.h"

/*------------------------------------------BuiltInFunction---------------------------------------------*/
/*This function is used to separate three built-in functions: cd, set and export                                                                     
  (1) "cd": when the command is "cd", this function would check whether the potential changing directory is
      "~".

  If is "~", this program would lead to "HOME" directory.                                                                                            
  
  (2) "set": when the command is "set", this function check how many arguments inputted.                                                            
  
  If is 3 arguments, check if the variable names are valid. Then execute set command.                                                                 
  
  If is 2 arguments, execute set command by ParseSetCommand function.                                                                                 
  
  (3) "export": when the command is "export", this function checks how many arguments are inputted.                                                   
  
  If not 2 arguments, it is an error.                                                                                                                 
  
  If is 2 arguments, find the variable and put its value into environment for other programs.                                                         
  
  If 2 arguments but cannot find the variable, it will print variable does not exist.                                                               */

void BuiltInFunction(std::vector<std::string> & parsedCmd,
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
      int k = CheckVar(parsedCmd[1]);
      if (k == -1) {
	std::cerr << "invald variable name" << std::endl;
	exit(EXIT_FAILURE);
      }
      else {
	ParseSetCommand(parsedCmd, inputCmd, myMap);
      }
    }
    else if (parsedCmd.size() == 2) {
      ParseSetCommand(parsedCmd, inputCmd, myMap);
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
