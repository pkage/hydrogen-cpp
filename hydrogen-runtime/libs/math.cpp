#include <iostream>
#include "api/hyd_api.h"
#include <vector>
#include <cmath>
using namespace std;
using namespace hyd;

int main() {}

extern "C" vector< pair<string, string> > getinfo() {
	vector< pair<string, string> > info;
	info.push_back(pair<string, string>("sqrt", "_sqrt"));
	return info;
}

extern "C" mem::Value  _sqrt(vector<mem::Value> args, err::ErrorStack *es) {
	if (args.size() > 0 && args[0].getType() == mem::types::NUMBER) {
		float f = sqrt(args[0].getn());
		cout << "running sqrt";
		mem::Value ret(mem::types::NUMBER);
		ret.set(f);
		return ret;
	}
	return mem::null_value;
}

