#ifndef __READCOMMAND_H__
#define __READCOMMAND_H__
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>

bool ReadCommand(std::string & inputCmd, std::vector<std::string> & parsedCmd);

void ChildPro(std::vector<std::string> & parsedCmd);

void EnvInMap(std::map<std::string, std::string> & myMap);

#endif
