#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

struct suffix {
	int index;
	int rank[2]; //own rank and next rank
};

void radixSort(vector<suffix>& arr, int sortBy) {

	if (sortBy < 0) return;
	int counter[257] = { 0 }; //the number of ascii characters
	vector<suffix> temp;
	for (int i = 0; i < arr.size(); i++) {
		counter[arr[i].rank[sortBy] + 1]++; //+1 is for the next ranks that are -1
	}

	for (int i = 1; i < 256; i++) {
		counter[i] += counter[i-1];
	}
	for (int i = 0; i < arr.size(); i++) {
		suffix s;
		temp.push_back(s);
	}

	for(int i = arr.size() - 1; i >= 0; i--) {
		temp[counter[arr[i].rank[sortBy] + 1] - 1] = arr[i];
		counter[arr[i].rank[sortBy] + 1]--;
	}
	for (int i = 0; i < arr.size(); i++) {
		arr[i] = temp[i];
	}

	radixSort(arr, sortBy - 1);
}
vector<int> builtSuffixArray(const string& text, int len) {
	
	vector<suffix> suffixes;
	vector<int> keepRankByIndex(len, 0);
	
	for (int i = 0; i < len; i++) {
		suffix s;
		suffixes.push_back(s);
		suffixes[i].index = i; //we keep the beggining of every suffix
		suffixes[i].rank[0] = text[i]; //keep the ascii code of the beginning of every suffix, thus making the own rank
		
		if (i + 1 >= len) {
			suffixes[i].rank[1] = -1;
		}
		else suffixes[i].rank[1] = text[i + 1];
	}
	
	radixSort(suffixes, 1);

	//at this point, we have sorted the first two characters
	//we want to sort them by the first 4 characters, then 8 and so on
	//we make a loop that runs up to 2*len, because if it to len
	//cases like len = 7 will be sorted according to the first 4 characters, leaving the remaining 3 not looked at

	for (int step = 4; step < 2 * len; step *= 2) { // it will make log(len) iterations
		int rank = 0; //we will start to assign the new ranks
		int previousRank = suffixes[0].rank[0]; // we will keep the current previous rank so we can compare
		suffixes[0].rank[0] = rank; // we assign 0 as own rank of the current smallest element
		
		keepRankByIndex[suffixes[0].index] = rank;
		//we start to assign the new ranks
		//if the subsequent rank pairs are equal, we give them equal own rank
		//if they're different, we increase the rank by one and assign the new rank to the latter

		for (int i = 1; i < len; i++) {
			
			if (suffixes[i].rank[0] == previousRank && suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
				suffixes[i].rank[0] = rank;
			}
			else {
				previousRank = suffixes[i].rank[0];
				rank++;
				suffixes[i].rank[0] = rank;
			}
			keepRankByIndex[suffixes[i].index] = rank;
		}
		//we assign the new next rank
		//at this point, the suffixes are sorted by the first step/2 characters
		//we want to sort them by the first step characters
		//for every index i of the text, the next step/2 characters are sorted and are of no interest
		//so we take the rank of the suffix that begins on index i + step/2 as next
		for (int i = 0; i < len; i++) {
			if (suffixes[i].index + step / 2 >= len) suffixes[i].rank[1] = -1;
			else {
				
				suffixes[i].rank[1] = keepRankByIndex[suffixes[i].index + step / 2];
			}
		}
		
		radixSort(suffixes, 1);
	}

	vector<int> suffixArray;
	for (int i = 0; i < len; i++) {
		suffixArray.push_back(suffixes[i].index);
	}

	cout << endl;
	return suffixArray;
}

void printSuffixArray(const vector<int>& suffixArray) {
	cout << "The suffix array is: ";
	for (size_t i = 0; i < suffixArray.size(); i++) {
		cout << suffixArray[i] << " ";
	}
	cout << endl;
}

int findFirstOccurence(int left, int right, const string& pattern, const string& text, const vector<int>& suffixArray) {
	
	int middle = (left + right) / 2;
	if (left > right) return -1;

	int lenArr = suffixArray.size();
	int lenPat = pattern.size();

	if (text.substr(suffixArray[middle], lenPat) > pattern) findFirstOccurence(left, middle - 1, pattern, text, suffixArray);
	else if (text.substr(suffixArray[middle], lenPat) < pattern) findFirstOccurence(middle+1, right, pattern, text, suffixArray);
	else {
		
		if (middle != 0) {
			if (text.substr(suffixArray[middle - 1], lenPat) == pattern) findFirstOccurence(left, middle - 1, pattern, text, suffixArray);
			else { return middle; }
		}
		else return middle;
	}
}

int findLastOccurence(int left, int right, const string& pattern, const string& text, const vector<int>& suffixArray) {
	
	int middle = (left + right) / 2;
	if (left > right) return -1;

	int lenArr = suffixArray.size();
	int lenPat = pattern.size();

	if (text.substr(suffixArray[middle], lenPat) > pattern) findLastOccurence(left, middle - 1, pattern, text, suffixArray);
	else if (text.substr(suffixArray[middle], lenPat) < pattern) findLastOccurence(middle + 1, right, pattern, text, suffixArray);
	else {
		if (middle != lenArr - 1) {
			if (text.substr(suffixArray[middle + 1], lenPat) == pattern) findLastOccurence(middle + 1, right, pattern, text, suffixArray);
			else { return middle; }
		}
		else return middle;
	}
}

pair<int,int> getNumberOfOccurences(const string& pattern, const string& text, const vector<int>& suffixArray) {
	
	int first = findFirstOccurence(0, suffixArray.size() - 1, pattern, text, suffixArray);
	int last = findLastOccurence(0, suffixArray.size() - 1, pattern, text, suffixArray);
	return { first, last };
}

void printAllOccurences(const string& pattern, const string& text, const vector<int>& suffixArray) {
	pair<int, int> firstLast = getNumberOfOccurences(pattern, text, suffixArray);

	if (firstLast.first == -1 || firstLast.second == -1) cout << "There are 0 occurences of this pattern." << endl;
	else {
		int count = firstLast.second - firstLast.first + 1;
		cout << "There are " << count << " occurrences of this pattern and they are on indexes: ";
		for (int i = firstLast.first; i <= firstLast.second; i++) {
			cout << suffixArray[i] << " ";
		}
		cout << endl;
	}
}
int main() {
	string text = "";
	string line;
	ifstream textFile("textFile.txt");
	if (textFile.is_open())
	{
		while (getline(textFile, line))
		{
			text += line;
		}
		textFile.close();
	}

	cout << text << endl;
	vector<int> suffixArray = builtSuffixArray(text, text.length());
	printSuffixArray(suffixArray);
	printAllOccurences("the", text, suffixArray);
}