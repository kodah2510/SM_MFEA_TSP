#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
class IOHandler
{
public:
	std::string input_file;
	IOHandler();
	IOHandler(const std::string&& m_input_file);
	~IOHandler();
	
	std::vector<float> read_mat(const std::string&& m_input_file, size_t& gene_sz);

	std::string get_problem_name();
private:
	void split_token(std::string& line, std::vector<float>& values_pack);
	
};