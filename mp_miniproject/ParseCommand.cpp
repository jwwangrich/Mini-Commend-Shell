#include "ParseCommand.h"

/*---------------------------------------------ParseCommand-------------------------------------------------*/
/*This function is to parse the command line argument into a vector of strings by using space to separate.                                                                        
  This function put the line in the temperary string and use iterator to check the token ' '(space) and '\\'.                                    
  
  The '\ ' is not counted as separation. This function also check the last char in the string as '\\'.                                                
*/

void ParseCommand(std::vector<std::string> & parsedCmd, std::string & inputCmd) {
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
