#include <iostream>
#include "api/hyd_api.h"
#include <vector>
#include <sstream>
using namespace std;
using namespace hyd;

int main() {
	cout << "lol nope\n";
}

extern "C" vector< pair<string, string> > getinfo() {
	vector< pair<string, string> > info;
	info.push_back(pair<string, string>("print", "print"));
	info.push_back(pair<string, string>("println", "println"));
	info.push_back(pair<string, string>("read", "read"));
	info.push_back(pair<string, string>("readline", "readline"));
	return info;
}



extern "C" mem::Value print(vector<mem::Value> args, err::ErrorStack *es) {
	for (int c = 0; c < args.size(); c++) {
		cout << args.at(c).toString();
	}
	return mem::null_value;
}

extern "C" mem::Value println(vector<mem::Value> args, err::ErrorStack *es) {
	print(args, es);
	cout << "\n";
	return mem::null_value;
}

extern "C" mem::Value read(vector<mem::Value> args, err::ErrorStack *es) {
	if (args.size() > 0) {
		cout << args.at(0).toString();
	}
	string tmp;
	cin >> tmp;
	return mem::Value(tmp);
}

extern "C" mem::Value readline(vector<mem::Value> args, err::ErrorStack *es) {
	if (args.size() > 0) {
		cout << args.at(0).toString();
	}
	string tmp;
	getline(cin, tmp);
	return mem::Value(tmp);
}

