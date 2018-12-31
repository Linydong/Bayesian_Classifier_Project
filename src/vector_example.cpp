#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

template<typename T>
void PrintSize(const T &t) {
	cout << "my size is :" << t.size() << "\n";
}

int main() {

	vector<double> a1;
	PrintSize(a1);


	a1.resize(1);
	PrintSize(a1);

	a1.resize(10);
	PrintSize(a1);

	a1.clear();
	PrintSize(a1);

	for (auto i = 0; i < 20; ++i) {
		a1.push_back(i / 10.0);
	}

	PrintSize(a1);

	for (auto i = 0; i < 20; ++i) {
		cout << a1[i] << " ";
	}
	cout << "\n";

	vector<vector<double>> a2;
	a2.resize(10);
	for (auto i = 0; i < a2.size(); ++i) {
		a2[i].resize(8, 1.2);
	}

	for (auto i = 0; i < a2.size(); ++i) {
		for (auto j = 0; j < a2[i].size(); ++j) {
			cout << a2[i][j] << " ";
		}
		cout << "\n";
	}

	system("pause");

	return 0;
}