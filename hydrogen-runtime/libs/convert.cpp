#include <iostream>
#include "api/hyd_api.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace hyd;

int main() {}

extern "C" vector< pair<string, string> > getinfo() {
	vector< pair<string, string> > info;
	info.push_back(pair<string, string>("ntos", "ntos"));
	info.push_back(pair<string, string>("ston", "ston"));
	return info;
}

extern "C" mem::Value ntos(vector<mem::Value> args, err::ErrorStack *es) {
	if (args.size() > 0 && args[0].getType() == mem::types::NUMBER) {
		float f = args[0].getn();
		stringstream ss; ss << f;
		return mem::Value(ss.str());
	}
	return mem::null_value;
}


extern "C" mem::Value ston(vector<mem::Value> args, err::ErrorStack *es) {
	if (args.size() > 0 && args[0].getType() == mem::types::STRING) {
		float f;
		try {
			f = (double)atof(args[0].gets().c_str());
		} catch (exception &e) {
			es->add(err::Error("@math", "ston", "Invalid conversion from string to number!" ));
			return mem::null_value;
		}
		return mem::Value(f);
	}
	return mem::null_value;
}

