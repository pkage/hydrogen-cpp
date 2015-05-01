#include <iostream>
#include "api/hyd_api.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace hyd;

int main() {
}

extern "C" vector< pair<string, string> > getinfo() {
	vector< pair<string, string> > info;
	info.push_back(pair<string, string>("diagnostic", "diagnostic"));
	return info;
}

string ftos(float f) {
	stringstream ss; ss << f;
	return ss.str();
}
extern "C" mem::Value diagnostic(vector<mem::Value> args, err::ErrorStack *es) {
	cout << "\bCalled @reflect.diagnostic\n";
	cout << "\tArgument length: " << args.size() << "\n";
	cout << "\tArgument breakdown: \n";
	for (int c = 0; c < args.size(); c++) {
		cout << "\t\t\t" << c << ": [" << ((args[c].getType() == mem::types::STRING) ? "STRING" : "NUMBER") << "]"
			<< ((args[c].getType() == mem::types::STRING) ? args[c].gets() : ftos(args[c].getn())) << " (string: "
			<< args[c].toString() << ")\n";
	}
	cout << "\tError stack: " << ((!es) ? "invalid" : "valid") << "\n";
	return mem::null_value;
}


