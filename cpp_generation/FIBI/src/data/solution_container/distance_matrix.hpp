#pragma once
#include <vector>
#include <iostream>

using namespace std;

class DistanceMatrix
{
private:
	const int num_towns;
	const vector<double>& towns_pos;
	vector<vector<double>> distances;

public:
	DistanceMatrix(const int num_towns, const int num_dim, const vector<double>& towns_pos) : num_towns(num_towns), towns_pos(towns_pos) {
		vector<vector<double>> d(num_towns - 1);
		for (int i_pt = 1; i_pt < num_towns; i_pt++)
		{ // 1 not an error (distance from a point to the same point trivial)
			d.at(i_pt - 1) = vector<double>(i_pt);
			for (int j_pt = 0; j_pt < i_pt; j_pt++)
			{ // distance from a point to the same point trivial so excluded here
				d.at(i_pt - 1).at(j_pt) = dist(&towns_pos[i_pt * num_dim], &towns_pos[j_pt * num_dim], num_dim);
			}
		}
		this->distances = d;
	}
	const double get(const town_in_tour_id_t i, const town_in_tour_id_t j) const {
		int id1, id2;
		if (j == i)
			return 0;
		else if (j > i)
		{
			id1 = j;
			id2 = i;
		}
		else
		{
			id1 = i;
			id2 = j;
		}
		// Correct ids due to the fact that we erased the first line of the distance matrix
		id1--;
		return this->distances.at(id1).at(id2);
	}
	const vector <double>& get_town_pos()const { return towns_pos; }
	void print() {
		for (int i_town_1 = 0; i_town_1 < num_towns; i_town_1++) {
			for (int i_town_2 = 0; i_town_2 < num_towns; i_town_2++) {
				cout << this->get(i_town_1, i_town_2);
				if (i_town_2 < num_towns - 1) {
					cout << ",";
				}
			}
			cout << endl;
		}
	}
};