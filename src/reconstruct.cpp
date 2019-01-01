#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <map>
#include <iomanip>

#define _USE_MATH_DEFINES  // using std math constants
#include <math.h>

using namespace std;

struct InputData {
	vector<double> num;
	string text;
	int textId;
};

int numFeature = 0, numClass = 0;

InputData Split(const string &str) {
	if (str.empty()) {
		return InputData{};
	}

	vector<int> splitPos;  // store the position of comma
	splitPos.push_back(-1);
	for (auto i = 0; i < str.size(); ++i) {
		if (str[i] == ',') {
			splitPos.push_back(i);
		}
	}
	splitPos.push_back(str.size());

	vector<string> tempResult;

	for (auto i = 0; i < (int)splitPos.size() - 1; ++i) {
		auto len = splitPos[i + 1] - splitPos[i] - 1;
		if (len > 0) {
			tempResult.push_back(str.substr(splitPos[i] + 1, len));
		}
	}

	InputData result;
	
	for (auto i = 0; i < (int)tempResult.size() - 1; ++i) {
		result.num.push_back(stod(tempResult[i]));
	}
	result.text = tempResult.back();


	return result;
}

map<string, int> GetLabelMap(const vector<InputData> &data) {
	map<string, int> labelMap;

	for (auto i = 0; i < data.size(); ++i) {
		auto findRes = labelMap.find(data[i].text);
		if (findRes == labelMap.end()) {
			labelMap[data[i].text] = labelMap.size();
		}
	}
	return labelMap;
}

void AssignLabel(const map<string, int> &labelMap, vector<InputData> &data){
	for(auto i = 0; i < data.size(); ++i){
		data[i].textId = labelMap.at(data[i].text);
	}
}

vector<InputData> Input(const string &filename) {
	fstream infile{ filename, ios::in };
	
	vector<InputData> result;

	string thisLine;
	while (!infile.eof()) {
		getline(infile, thisLine);
		if (!thisLine.empty()) {
			result.emplace_back(Split(thisLine));
		}
	}

	return result;
}


struct Gauss {
	double mean;
	double var;
};

Gauss FitGauss(const vector<double> &observation) {
	double mean = 0.0;
	for (auto i = 0; i < observation.size(); ++i) {
		mean += observation[i];
	}
	mean /= observation.size();

	double var = 0.0;
	for (auto i = 0; i < observation.size(); ++i) {
		var += (observation[i] - mean) * (observation[i] - mean);
	}
	var /= observation.size();

	Gauss result;
	result.mean = mean;
	result.var = var;

	return result;
}

struct NaiveBayes {
	// (num class, num feature)
	vector<vector<Gauss>> dist;
	vector<double> prob;
};


NaiveBayes FitNaiveBayes(const vector<InputData> &input) {
	NaiveBayes result;
	
	result.dist.resize(numClass);
	for (auto i = 0; i < result.dist.size(); ++i) {
		result.dist[i].resize(numFeature);
	}

	result.prob.resize(numClass);

	vector<vector<InputData>> catSample;
	catSample.resize(numClass);
	for (auto i = 0; i < input.size(); ++i) {
		catSample[input[i].textId].push_back(input[i]);
	}

	for (auto i = 0; i < numClass; ++i) {
		result.prob[i] = (double)catSample[i].size() / input.size();
	}

	for (auto i = 0; i < numClass; ++i) {
		for (auto j = 0; j < numFeature; ++j) {
			vector<double> observation;
			for (auto k = 0; k < catSample[i].size(); ++k) {
				observation.push_back(catSample[i][k].num[j]);
			}
			result.dist[i][j] = FitGauss(observation);
		}
	}

	return result;
}

// Probability Density Function
// Portable Document Format
double GaussPDF(const Gauss &para, double x) {
	double front = 1.0 / (sqrt(2.0 * M_PI * para.var));
	double exponent = -(x - para.mean) * (x - para.mean) / (2.0 * para.var);
	return front * exp(exponent);
}

vector<double> PredictProb(const NaiveBayes &para, const vector<double> &num) {
	vector<double> result;
	result.resize(numClass);

	double denom = 0.0;
	for (auto i = 0; i < numClass; ++i) {
		double logSum = 0.0;
		for (auto j = 0; j < numFeature; ++j) {
			logSum += log(GaussPDF(para.dist[i][j], num[j]));
		}
		result[i] = exp(logSum) * para.prob[i];
		denom += result[i];
	}

	for (auto i = 0; i < numClass; ++i) {
		result[i] /= denom;
	}

	return result;
}

int PredictClass(const NaiveBayes &para, const vector<double> &num) {
	auto prob = PredictProb(para, num);
	double maxProb = prob.front();
	int maxId = 0;

	for (auto i = 0; i < prob.size(); ++i) {
		if (prob[i] > maxProb) {
			maxId = i;
			maxProb = prob[i];
		}
	}

	return maxId;
}


void ShowStat(const NaiveBayes &para, const vector<string> &invLabelMap, const vector<InputData> &data){
	int correct = 0;
	cout << "feature\tprediction\tground truth\terror\n";
	for(auto i = 0; i < data.size(); ++i){
		auto predict = PredictClass(para, data[i].num);
		bool isCorrect = predict == data[i].textId;
		if(isCorrect){correct++;}
		for(auto j = 0; j < numFeature; ++j){
			cout << fixed << setprecision(2) << data[i].num[j];
			if(j < numFeature - 1){
				cout << ",";
			}
		}
		cout << "\t" << invLabelMap[predict];
		cout << "\t" << invLabelMap[data[i].textId];
		if(!isCorrect){
			cout << "\t(X)";
		}
		cout << "\n";
	}
	cout << "total: " << data.size() << "\tcorrect: " << correct << "\taccuracy: ";
	cout << (double)correct/data.size() << "\n";
}

int main() {

	auto train = Input("Iristraindata.txt");
	auto labelMap = GetLabelMap(train);

	numClass = labelMap.size();
	numFeature = train.front().num.size();

	AssignLabel(labelMap, train);

	auto para = FitNaiveBayes(train);

	auto test = Input("Iristestdata.txt");
	AssignLabel(labelMap, test);

	vector<string> invLabelMap;
	invLabelMap.resize(numClass);
	for(auto iter = labelMap.begin(); iter != labelMap.end(); ++iter){
		invLabelMap[iter->second] = iter->first;
	}

	cout << "training set:\n";
	ShowStat(para, invLabelMap, train);
	cout << "testing set:\n";
	ShowStat(para, invLabelMap, test);

	system("pause");

	return 0;
}