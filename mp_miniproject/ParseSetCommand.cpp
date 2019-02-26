#include "ParseSetCommand.h"

/*-----------------------------------------ParseSetCommand----------------------------------------------*/
/*Looking for '$' in variable!!                                                                                                                       
  
  If found keyName(variable), search for variable value in myMap.                                                                                     
  
  1. If found, replace the valName(value) corresponding to the keyName(variable) in myMap.                                                            
  
  2. If not found valName(value), create valName(value) with corresponding keyName in myMap.                                                          
  
  If not found '$', add the keyName(variable) into myMap.                                                                                             
  
  1. If no value provided, valName would be "".                                                                                                                                                                                                                                     
  2. Otherwise, create a new keyName(variable) with valName(value) in myMap.                                                                        
*/

void ParseSetCommand(std::vector<std::string> & parsedCmd,
		     std::string & inputCmd,
		     std::map<std::string, std::string> & myMap) {
  size_t pos = inputCmd.find_first_of("$");
  std::string setCmd ("set");
  std::string varCmd (parsedCmd[1]);
  size_t varPos = inputCmd.find(varCmd);
  if (pos != std::string::npos) {
    if (parsedCmd.size() == 2) {
      std::cerr << "$ should have potential changing variable value" << std::endl;
      return;
    }
    size_t keyName_len = parsedCmd[1].length();
    std::string keyName = inputCmd.substr(pos + 1, keyName_len - 1); //$ does not count
    size_t valPos = inputCmd.find(varCmd) + keyName_len + 1;
    std::string valName = inputCmd.substr(valPos);
    if (myMap.find(keyName) != myMap.end()) {
      myMap.at(keyName) = valName;
    }
    else {
      myMap[keyName] = valName;
    }
  }
  else {
    size_t first_spaPos = inputCmd.find(' ', varPos - 1);
    size_t second_spaPos = inputCmd.find_first_of(' ', first_spaPos + 1);
    std::string keyName;
    std::string valName;
    if (parsedCmd.size() == 2) {
      keyName = inputCmd.substr(first_spaPos + 1);
      valName = "";
      myMap[keyName] = valName;
    }
    else {
      keyName = inputCmd.substr(first_spaPos + 1, second_spaPos - 1 - first_spaPos);
      valName = inputCmd.substr(second_spaPos + 1);
      myMap[keyName] = valName;
    }
    //std::cout << "debug: success in parseVar" << std::endl;
  }
}


/*--------------------------------------------CheckVar--------------------------------------------------*/
/*To check whether the variable name is a valid name.                                                                                         
                                                                                                                                              
  The first char in myName string(variable name) can be '$'.                                                                                  
                                                                                                                                              
  The valid names include 'a'->'z', 'A'->'Z' and '_'                                                                                          
                                                                                                                                              
  If not in this range, this function drive invalid name return to int "-1", which is the failure sign.                                       
                                                                                                                                              
  On the contrary, if variable name is valid, this function returns to int "1".                                                               
*/

int CheckVar(std::string myName) {
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
