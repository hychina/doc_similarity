#include <vector>
#include <unordered_set>

typedef unsigned int uint;

class Matrix {
private:
	int m_last_row;
	vector<uint> m_cols;
	unordered_set<uint> m_cols_set;
	vector<double> m_values;
	vector<uint> m_row_start;

	void csr2csc(uint col_start[], uint rows[], double values[]);
public:
	Matrix();

	const double value_at(uint index) const;
	const uint col_at(uint index) const;
	const uint row_start(uint row) const;
	const uint num_elements() const;
	const uint num_rows() const;
	const uint num_cols() const;

	void add(uint row, uint col, double value);
	void update(uint index, double new_value);
	auto multiply_by_col() -> double**;
};


