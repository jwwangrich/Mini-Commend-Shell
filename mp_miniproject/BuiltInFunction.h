#ifndef __BUILTINFUNCTION_H__
#define __BUILTINFUNCTION_H__
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <map>

void BuiltInFunction(std::vector<std::string> & parsedCmd,
		     std::string & inputCmd,
		     std::map<std::string, std::string> & myMap);

#endif
