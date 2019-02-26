#include "ReadCommand.h"
#include "ParseCommand.h"

/*-------------------------------------------------ReadCommand--------------------------------------------------*/
/*Read command, if is "eof", return false and if is "exit", return false as well.                                                                   
  
  Otherwise, return true.                                                                                                                           */

bool ReadCommand(std::string & inputCmd,
		 std::vector<std::string> & parsedCmd) {
  getline(std::cin, inputCmd);
  if (std::cin.eof()) {
    std::cerr << "End Of File" << std::endl;
    return false;
  }
  ParseCommand(parsedCmd, inputCmd);
  if (parsedCmd[0] == "exit") {
    std::cout << "Command is exit" << std::endl;
    return false;
  }

  return true;
}


/*-------------------------------------------------ChildPro-----------------------------------------------------*/
/*Execute child process. Use execvp to execute the command.                                                                                          */

void ChildPro(std::vector<std::string> & parsedCmd) {
  std::vector<char *> argCmd(parsedCmd.size() + 1);    // one extra for the null
  // std::vector<string> argCmd(parsedCmd.size() + 1);
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


/*--------------------------------------------------EnvInMap---------------------------------------------------*/
/*Put all the default environment variables in myMap.                                                                                                */

void EnvInMap(std::map<std::string, std::string> & myMap) {
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
