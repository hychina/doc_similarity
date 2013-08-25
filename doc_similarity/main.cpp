// Copyright 2013 LHY

#include "maxtrix.h"
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::unordered_set;
using std::unordered_map;
using std::string;
using std::ifstream;
using std::istringstream;
using std::make_pair;
using std::map;
using std::exception;

const char *file_text = "my_clean_text.txt";
const char *file_stopwords = "stopwords.txt";
unordered_set<string> stopwords;
unordered_map<uint, double> doc_length;
Matrix matrix;

void read_stopwords() {
    ifstream in(file_stopwords);
    string stopword;

    while (getline(in, stopword)) {
        if (stopword != "") {
            stopwords.insert(stopword);
        }
    }
}

vector<string>* extract_words(const string &doc) {
    istringstream document(doc);
    vector<string> *words = new vector<string>();
    string word;
    while (document >> word) {
        words->push_back(word);
    }
    return words;
}

void calculate_weights() {
    ifstream in(file_text);

    string doc;
    int doc_count = 0;
    unordered_map<string, uint> word_index;
    unordered_map<uint, uint> doc_freq;

    while (getline(in, doc)) {
        if (doc == "") continue;

        vector<string> *words = extract_words(doc);
        unordered_map<uint, uint> term_freq;

        for (string word : *words) {
            // if word is in stopwords vector
            if (stopwords.count(word) != 0) {
                continue;
            }

            auto result = word_index.insert(make_pair(word, word_index.size()));
            uint index = result.first->second;

            uint tf = ++term_freq[index];

            // first time a word appears in a document
            if (tf == 1) {
                ++doc_freq[index];
            }
        }

        typedef unordered_map<uint, int>::value_type value;

        auto max = max_element(term_freq.cbegin(), term_freq.cend(),
                               [] (const value &i, const value &j) {
                                   return i.second < j.second;
                               });

        for (auto &i : term_freq) {
            double weight = 0.4 + 0.6 * (i.second / max->second);
            matrix.add(doc_count, i.first, weight);
        }

        ++doc_count;
        delete words;
    }

    // update the weights
    for (uint i = 0; i < matrix.num_rows(); ++i) {
        uint r_start = matrix.row_start(i);
        uint r_end = matrix.row_start(i+1);
        double sum = 0;

        for (uint j = r_start; j < r_end; ++j) {
            double weight = matrix.value_at(j);
            uint word = matrix.col_at(j);

            uint df = doc_freq[word];
            double idf = log(static_cast<double>(doc_count)
                             / static_cast<double>(df)) + 1;
            double new_weight = weight * idf;

            matrix.update(j, new_weight);

            // calculate the length
            sum += pow(new_weight, 2);
        }

        double length = sqrt(sum);
        doc_length.insert(make_pair(i, length));
    }

    in.close();
}

void show_user_interface(double** result) {
    cout << "输入文档编号，查询与之最相似的前十篇文档..." << endl;

    while (true) {
        string input;
        cin >> input;

        if (input == "q" || input == "Q") break;

        uint doc_number;
        try {
            doc_number = atoi(input.c_str());
        } catch(exception e) {
            cout << "请输入正确数字..." << endl;
            continue;
        }

        if (doc_number < 1 || doc_number > matrix.num_rows()) {
            cout << "请输入1到"
                 << matrix.num_rows()
                 << "范围之内的数字..." << endl;
            continue;
        }

        double *similarities = result[doc_number - 1];
        map<double, uint> sim_map;

        for (uint i = 0; i < matrix.num_rows(); ++i) {
            if (doc_number == i + 1) continue;
            sim_map.insert(make_pair(similarities[i], i + 1));
        }

        int count = 0;
        for (auto itr = sim_map.rbegin(); itr != sim_map.rend(); ++itr) {
            if (count >= 10) break;

            cout << itr->second << " ";
            ++count;
        }
        cout << endl;
    }
}

int main() {
    clock_t start = clock();

    read_stopwords();
    calculate_weights();

    cout << "预处理用时："
         << (static_cast<float>(clock() - start)) / CLOCKS_PER_SEC
         << "秒" << endl;
    start = clock();

    double **result = matrix.multiply_by_col();

    for (uint i = 0; i < matrix.num_rows(); ++i) {
        for (uint j = 0; j < matrix.num_rows(); ++j) {
            result[i][j] = result[i][j] / (doc_length[i] * doc_length[j]);
        }
    }

    cout << "矩阵运算用时："
         << (static_cast<float>(clock() - start)) / CLOCKS_PER_SEC
         << "秒" << endl;

    show_user_interface(result);

    for (uint i = 0; i < matrix.num_rows(); ++i) {
        delete result[i];
    }
    delete result;
    return 0;
}
