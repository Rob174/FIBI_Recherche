#pragma once
#include <vector>
#include <string>
#include <map>
#include <tuple>

void run_restart(function<vector<pair<string, double>>(Args, const unique_ptr < set<int>>& )>f, Args arguments, const unique_ptr < set<int>>& missing) {
	f(arguments, missing);
}

using namespace std;

class ProductIterator
{
public:
    int current = 0;
    int maxIndex = 1;
    vector<vector<int>> params_poss;
    vector<int> index;

    ProductIterator() : params_poss(vector<vector<int>> {}), current(0) {};

    ProductIterator(vector<vector<int>> params_poss) : current(0), params_poss(params_poss)
    {
        this->index = vector<int>(params_poss.size(), 0);
        for (int i = 0; i < params_poss.size(); i++)
        {
			maxIndex *= params_poss[i].size();
		}
        maxIndex--; // Max index not number of possibilites
    };

    void restart() {
        current = 0;
    }

    vector<int> next()
    {
        int res = current;
        if (current > maxIndex)
        {
            return vector<int>{};
        }
        vector<int> values_poss(index.size());
        int k = 0;
        for (int idx : index)
        {
            values_poss[k] = params_poss[k][idx];
            k++;
        }
        // Increment index
        for (int i = index.size()-1; i >= 0 ; i--)
        {
            if (index[i] < params_poss[i].size() - 1) {
                index[i]++;
                break;
            } else {
                index[i] = 0;
            }
        }
        current++;
        return values_poss;
    }
};

set<int>* read_missing() {
    ifstream file("missing.txt");
    set<int> *missing = new set<int>();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        int num;
        while (ss >> num) {
            missing->insert(num);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
    }
    file.close();
    return missing;
}