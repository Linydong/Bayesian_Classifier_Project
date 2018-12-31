#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>  // vector is a "dynamic array" with mutable size, it is not a "vector" by definition
#include <map>  // map is a mapping between an unique key and a value
#include <array> // fixed size array class, array<double,3> is basically double[3]
#include <iomanip>
#include <cstdlib>


#define _USE_MATH_DEFINES  // using std math constants
#include <math.h>

using namespace std;

// split the string into several segments by a separator
// for example, if we call split on "1,2,3,4,5" then the output would be
// ["1", "2", "3", "4", "5"]
// @param str: the string to be splitted
// @param sep: the separator
vector<string> Split(const string &str, char sep) {
	vector<string> result;
	size_t left = 0, right = 0;
	while (right < str.size()) {
		if (str[right] == sep) {
			result.push_back(str.substr(left, right - left));
			right++;
			left = right;
		}
		else {
			right++;
		}
	}
	if (left != right) {
		result.push_back(str.substr(left, right - left));
	}
	return result;
}

// clean redundant linebreaks or spaces around a string
// for example, if we call strip on "abc \n", then the output would be "abc"
// @param str: the string to be stripped
string Strip(const string &str) {
	if (str.empty()) {
		return string{};
	}
	size_t left = 0, right = str.size() - 1;
	vector<char> garbage{' ', '\n', '\r'};

	while (left <= right) {
		if (find(garbage.begin(), garbage.end(), str[left]) == garbage.end()) {
			break;
		}
		left++;
	}

	while (right > left) {
		if (find(garbage.begin(), garbage.end(), str[right]) == garbage.end()) {
			break;
		}
		right--;
	}

	if (left > right) {
		return string{};
	}

	return str.substr(left, right - left + 1);
}

// read each line of training data into the memory
// @param filename: the filename of the training data
// @return: an array containg all valid lines in the training data
vector<vector<string>> LoadTraining(const string &filename) {
	fstream infile;
	infile.open(filename, ios::in | ios::binary);

	if (!infile.good()) {
		cerr << "error occured opening " << filename << "\n";
		throw runtime_error{ "io error" };
	}

	vector<vector<string>> result;
	string temp;
	while (!infile.eof()) {
		getline(infile, temp);
		auto cleanLine = Strip(temp);
		if (!cleanLine.empty()) {
			result.emplace_back(Split(cleanLine, ','));
		}
	}

	return result;
}

// analyze the number of labels in the training set and assign each label an unique integer
// we can then use the integer to represent its category
// @return: the mapping from a string (the label) to an integer
map<string, int> AnalyzeLabel(const vector<vector<string>> &trainingRaw) {
	map<string, int> result;
	int counter = 0;
	for (size_t i = 0; i < trainingRaw.size(); ++i) {
		auto labelText = trainingRaw[i].back();
		if (result.find(labelText) == result.end()) {
			// if we cannot find the label inside the map, add it
			result[labelText] = counter;
			counter++;
		}
	}
	return result;
}

// the wrapping class for training samples
struct TrainTuple {
	vector<double> data;  // the training data
	int label; // the corresponding label
};

// parse the training data from string to floating point numbers for training
// @param numFeature: the number of features
// @param trainingRaw: the unparsed training data
// @param labelMap: the mapping between string labels and integer labels
// @return: an array of training tuples
vector<TrainTuple> ParseTraining(
	int numFeature,
	const vector<vector<string>> &trainingRaw, 
	const map<string, int> &labelMap) {
	vector<TrainTuple> result;

	for (size_t i = 0; i < trainingRaw.size(); ++i) {
		const auto &raw = trainingRaw[i];

		TrainTuple train;
		train.data.resize(numFeature);
		for (auto j = 0; j < numFeature; ++j) {
			// stod is a dedicated function to convert string to double precision floting point number
			train.data[j] = stod(raw[j]);
		}
		
		train.label = labelMap.at(raw[numFeature]);

		result.emplace_back(move(train));
	}

	return result;
}

// given a set of data points, return the maximum likelihood estimate of the mean and the variance
// @param points: the data points
// @return: an array of floating point numbers, namely mean, variance and standard deviation
array<double, 3> FitGaussian(vector<double> points) {
	// the maximum likelihood estimate of the mean is their averaged value
	double mean = 0.0;
	for (size_t i = 0; i < points.size(); ++i) {
		mean += points[i];
	}
	mean /= points.size();

	double variance = 0.0;
	for (size_t i = 0; i < points.size(); ++i) {
		double diff = points[i] - mean;
		variance += diff * diff;
	}
	variance /= points.size();

	double sd = sqrt(variance);

	return array<double, 3>{mean, variance, sd};
}


const double __gaussian_factor = 1.0 / sqrt(2.0 * M_PI);
// compute the probability density funcion of the Gaussian distribution
// @param parameter: the parameters of the gaussian distribution, the first one is mean, and the second one is variance
// @param at: the point where we want to compute
// @return: the value of the density funcion
double Gaussian(const array<double, 3> &parameter, double at) {
	double diff = at - parameter[0];
	double exponent = -(diff * diff) / (2.0 * parameter[1]);
	return __gaussian_factor * exp(exponent) / parameter[2];
}


// the parameter class of the Gaussian Naive Bayes classifier
struct GNBParameter {
	int numFeature;
	int numLabel;

	// the distributions of each component
	// the first dimension is the category
	// the second dimension is the number of features
	// the third dimension contains the parameters of its corresponding Gaussian distribution
	vector<vector<array<double, 3>>> dist;

	// the probabilities of each label
	vector<double> prob;
};


// compute the parameters for Gaussian Naive Bayes classifier
GNBParameter FitGaussianNaiveBayes(int numFeature, int numLabel, const vector<TrainTuple> &training) {
	GNBParameter result;
	result.numFeature = numFeature;
	result.numLabel = numLabel;
	result.dist.resize(numLabel);
	for (auto i = 0; i < numLabel; ++i) {
		// the parameters are initialized with standard normal distribution
		// just in case some label might be missing
		result.dist[i].resize(numFeature, array<double, 3>{0.0, 1.0, 1.0});
	}

	result.prob.resize(numLabel, 0.0);

	vector<vector<TrainTuple>> trainingByLabel(numLabel);
	// group training samples by their labels
	for (size_t i = 0; i < training.size(); ++i) {
		const auto &sample = training[i];
		trainingByLabel[sample.label].push_back(sample);
	}

	// compute parameters
	for (size_t i = 0; i < trainingByLabel.size(); ++i) {
		const auto &samples = trainingByLabel[i];
		if (samples.empty()) {
			continue;
		}

		// compute the distribution for each component
		for (auto j = 0; j < numFeature; ++j) {
			vector<double> points;
			// extract all values of jth component in ith label class
			for (size_t k = 0; k < samples.size(); ++k) {
				points.push_back(samples[k].data[j]);
			}
			result.dist[i][j] = move(FitGaussian(points));
		}
	}

	for (auto i = 0; i < numLabel; ++i) {
		result.prob[i] = (double)trainingByLabel[i].size() / training.size();
	}

	return result;
}

// predict the probability of a sample being in each class
vector<double> PredictProb(const GNBParameter &param, const vector<double> &sample) {
	if (sample.size() != param.numFeature) {
		cout << "invalid sample size\n";
		throw runtime_error{ "invalid sample size" };
	}

	vector<double> result(param.numLabel);
	// the denominator of the probability
	double denom = 0.0;
	for (auto i = 0; i < param.numLabel; ++i) {
		// in practice, we do not compute the product of probabilities directly because repetitive
		// multiplication of small values will incur underflow
		// (the values will be very close to zero eventually)
		// which will lead to loss of precision
		// instead, we apply logarithm to probabities and compute their sum
		// the product would be the exponentiation of the sum
		double logSum = 0.0;
		for (auto j = 0; j < param.numFeature; ++j) {
			auto test = Gaussian(param.dist[i][j], sample[j]);
			logSum += log(Gaussian(param.dist[i][j], sample[j]));
		}

		double prod = exp(logSum);
		double numerator = param.prob[i] * prod;
		denom += numerator;
		result[i] = numerator;
	}

	for (auto i = 0; i < param.numLabel; ++i) {
		result[i] /= denom;
	}

	return result;
}

// predict the label of a sample
int PredictLabel(const GNBParameter &param, const vector<double> &sample) {
	auto prob = PredictProb(param, sample);
	int label = 0;
	double maxProb = prob[0];
	for (size_t i = 0; i < prob.size(); ++i) {
		if (prob[i] > maxProb) {
			label = i;
			maxProb = prob[i];
		}
	}
	return label;
}

int main(int argc, const char * argv[]) {
	cout << "my filename is " << argv[0] << "\n";
	cout << "make sure the training data is in the correct folder\n\n";

	cout << "please enter filename: ";
	string filenameInput;
	cin >> filenameInput;
	cout << "trying to read training data from " << filenameInput << "\n\n";

	auto trainingRaw = LoadTraining(filenameInput);
	if (trainingRaw.empty()) {
		cout << "unable to read any valid training data\n";
		throw runtime_error{ "no training data" };
	}

	cout << trainingRaw.size() << " training samples loaded\n";
	// analyze the first training sample and assume all remaining samples are of the same format
	// the last field is the label, so the number of features is subtracted by one
	cout << "each training sample has " << trainingRaw.front().size() - 1 << " features\n\n";

	auto numFeature = (int)trainingRaw.front().size() - 1;
	if (numFeature < 1) {
		cout << "invalid number of features: " << numFeature << "\n";
		throw runtime_error{ "feature error" };
	}
	
	// generate string to integer lable mapping
	auto labelMap = AnalyzeLabel(trainingRaw);
	auto numLabel = (int)labelMap.size();

	cout << "training Gaussian Naive Bayes classifier...\n";
	// parse the training data
	auto training = ParseTraining(numFeature, trainingRaw, labelMap);
	cout << "training finished\n\n";

	// fit the model
	auto gnbParam = FitGaussianNaiveBayes(numFeature, numLabel, training);

	// create inverse label transformation
	vector<string> invLabelMap(numLabel);
	for (const auto &iter : labelMap) {
		invLabelMap[iter.second] = iter.first;
	}

	cout << "performance evaluation on training set\n";
	cout << "data\tground truth\tpredicted\n";
	auto incorrectCount = 0;
	for (size_t i = 0; i < training.size(); ++i) {
		const auto &sample = training[i];
		for (size_t j = 0; j< sample.data.size(); ++j) {
			cout << sample.data[j];
			if (j != sample.data.size() - 1) {
				cout << ",";
			}
		}
		cout << "\t";
		cout << "\t" << invLabelMap[sample.label];
		auto predicted = PredictLabel(gnbParam, sample.data);
		cout << "\t" << invLabelMap[predicted];
		if (predicted != sample.label) {
			incorrectCount++;
			cout << "\t(x)";
		}
		cout << "\n";
	}
	cout << "total: " << training.size() << "\tcorrect: " << training.size() - incorrectCount;
	cout << "\tincorrect: " << incorrectCount << "\n";
	cout << "accuracy: " << (double)(training.size() - incorrectCount) / training.size() << "\n\n";

	system("pause");
	return 0;
}