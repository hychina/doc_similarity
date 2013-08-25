#include "maxtrix.h"

Matrix::Matrix() : m_last_row(-1), m_row_start(1, 0) {}

const uint Matrix::num_rows() const {
	return this->m_row_start.size() - 1;
}

const uint Matrix::num_cols() const {
	return this->m_cols_set.size();
}

const uint Matrix::num_elements() const {
	return this->m_values.size();
}

const uint Matrix::col_at(uint index) const {
	return this->m_cols[index];
}

const double Matrix::value_at(uint index) const {
	return this->m_values[index];
}

const uint Matrix::row_start(uint row) const {
	return this->m_row_start[row];
}

void Matrix::add(uint row, uint col, double value) {
	if (row != m_last_row) {
		m_row_start.insert(m_row_start.end() - 1, m_values.size());
		m_last_row = row;
	}

	m_cols.push_back(col);
	m_values.push_back(value);
	m_row_start.back() = m_values.size();

	m_cols_set.insert(col);
}

void Matrix::update(uint index, double new_value) {
	this->m_values[index] = new_value;
}

// 转换矩阵的存储方式
void Matrix::csr2csc(uint col_start[], uint rows[], double values[]) {		
	uint i;	
	const uint num_elements = this->num_elements();
	const uint num_rows = this->num_rows();
	const uint num_cols = this->num_cols();

	for (i = 0; i <= num_cols; ++i) 
		col_start[i] = 0;
	// 记录前一列的非零元个数
	for (i = 0; i < num_elements; ++i) {
		int col = m_cols[i]; // 列号从0开始
		++col_start[col + 1];
	}
	// 确定每一列在数组中的开始位置
	for (i = 1; i <= num_cols; ++i) 
		col_start[i] += col_start[i - 1];

	// 填充矩阵
	for (i = 0; i < num_rows; ++i) {
		int r_start = m_row_start[i];
		int r_end = m_row_start[i + 1];

		for (int j = r_start; j < r_end; ++j) {
			int col = m_cols[j];
			int loc = col_start[col];
			values[loc] = m_values[j];
			rows[loc] = i; // 行号从0开始
			++col_start[col];
		}
	}

	// 前移每一列的开始位置 
	for (i = num_cols; i > 0; --i) 
		col_start[i] = col_start[i - 1];
	col_start[0] = 0;
}	

auto Matrix::multiply_by_col() -> double** {
	const uint num_elements = this->num_elements();
	const uint num_rows = this->num_rows();
	const uint num_cols = this->num_cols();

	double **result;
	result = new double*[num_rows];
	for (uint i  = 0; i < num_rows ; ++i) {
		result[i] = new double[num_rows];
	}

	uint *col_start = new uint[num_cols + 1];
	uint *rows = new uint[num_elements];
	double *values = new double[num_elements];

	csr2csc(col_start, rows, values);

	for (uint i  = 0; i < num_rows; ++i) {
		for (uint j = 0; j < num_rows; ++j) {
			result[i][j] = 0;
		}
	}

	for (uint i  = 0; i < num_cols; ++i) {
		uint i_start = col_start[i];
		uint i_end = col_start[i + 1];
		for (uint j = i_start; j < i_end; ++j) {
			for (uint k = i_start; k < j; ++k) {
				uint row = rows[j];
				uint col = rows[k];
				result[row][col] += values[j] * values[k];
				result[col][row] = result[row][col];
			}
		}
	}

	delete col_start;
	delete rows;
	delete values;

	return result;		
}
