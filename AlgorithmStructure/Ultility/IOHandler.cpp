#include "IOHandler.h"

IOHandler::IOHandler(){}

IOHandler::IOHandler(const std::string && m_input_file) : input_file(m_input_file){}

IOHandler::~IOHandler() {}
//take dist_mat and gene_sz as parameters
std::vector<float> IOHandler::read_mat(const std::string && m_input_file, size_t& gene_sz)
{
	// TODO: insert return statement here
	input_file = { m_input_file };
	std::ifstream file(m_input_file);
	std::string line;

	std::vector<float> dist_mat;

	if (file.is_open())
	{
		//assign number_of_cities 
		std::string first_line;
		std::getline(file, first_line);
		std::vector<float> tmp_pack;
		tmp_pack.reserve(1000);
		split_token(first_line, tmp_pack);

		//finishing read the first line 
		//initialize the distance matrix
		gene_sz = tmp_pack.size();
		dist_mat.reserve(gene_sz * gene_sz);

		for (auto& val : tmp_pack)
		{
			dist_mat.emplace_back(val);
		}

		while (std::getline(file, line))
		{
			split_token(line, dist_mat);
		}
	}
	else
	{
		std::cerr << "Cannot read input file\n";
	}
	return dist_mat;
}
//split file_name to problem name 
std::string IOHandler::get_problem_name()
{
	auto slash_pos = input_file.find_last_of('\\');
	auto dot_pos = input_file.find_first_of('.');
	std::string res;
	if (slash_pos != std::string::npos && dot_pos != std::string::npos)
	{
		res = input_file.substr(slash_pos + 1, dot_pos - slash_pos - 1);
	}
	return res;
}
//split string based on regex
void IOHandler::split_token(std::string & line, std::vector<float>& values_pack)
{
	std::regex ws_re("\\s+");
	std::for_each(
		std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),
		std::sregex_token_iterator(),
		[&values_pack](const std::string& token)
		{
			if (token.compare("") != 0)
				values_pack.emplace_back(std::stof(token, nullptr));
		}
	);
}
