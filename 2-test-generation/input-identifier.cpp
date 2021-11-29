#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>

std::vector<std::string> SplitString(std::string &s, const std::string &delimiter) {
	size_t pos = 0;
	std::string token;
	std::vector<std::string> ret;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		//std::cout << token << std::endl;
		ret.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	ret.push_back(s);
	return ret;
}

int main(int argc , char * argv[])
{
	//arguments in the target function
    char *argument[10];

    //read the original application	
    std::ifstream in;
	std::string line;
	in.open("input-test.c");
	while(std::getline(in, line))
	{
		if(line.find("kernel")!=std::string::npos){
			//printf("%s\n", line.c_str());
			int start = line.find("(");
			int end = line.find(")");
			std::string arguments = line.substr(start+1, end-start-1);
			std::cout << arguments <<std::endl;

			std::vector<std::string>  argument_list = SplitString(arguments,",");
			for(auto arg:argument_list) {
				std::cout<<arg<<std::endl;
			}
		}
	}
	in.close();

    return 0;
}