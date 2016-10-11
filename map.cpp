#include <map>
#include <iostream>
using namespace std;

int main() {
	map<string, string> header;
	string attr("content-length");
	header[attr] = "1234";
	cout << header[attr] << endl;
	return 0;
}

