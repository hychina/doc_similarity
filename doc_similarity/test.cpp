#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

int& retbyvalue() {
	int i = 100;
	return i;
}

int xxxmain() {
	int i = retbyvalue();
	int &j = retbyvalue();
	cout << i << endl;
	cout << j << endl;
}