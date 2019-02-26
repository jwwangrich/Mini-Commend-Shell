#ifndef __PARSESETCOMMAND_H__
#define __PARSESETCOMMAND_H__
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

void ParseSetCommand(std::vector<std::string> & parsedCmd,
		     std::string & inputCmd,
		     std::map<std::string, std::string> & myMap);

int CheckVar(std::string myName);

#endif
