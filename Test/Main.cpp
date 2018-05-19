#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
struct Node
{
	size_t name;
	size_t x, y;
};
auto dist(Node& a, Node& b)
{
	auto X = a.x - b.x;
	auto Y = a.y - b.y;
	return sqrt(X * X + Y * Y);
}
auto dist_mat(std::vector<Node>& nodes)
{
	std::vector<double> res;
	size_t base = nodes.size();
	res.resize(base * base);
	for (size_t i = 0; i < base; i++)
	{
		for (size_t j = 0; j < base; j++)
		{
			res[i * base + j] = dist(nodes[i], nodes[j]);
		}
	}
	return res;
}
void export_file(std::vector<double>& dist_mat, size_t base, std::string& name)
{
	std::ofstream output_file;
	output_file.open(name + ".txt");

	for (size_t i = 0; i < base; i++)
	{
		for (size_t j = 0; j < base; j++)
		{
			output_file << dist_mat[i * base + j] << " ";
		}
		output_file << "\n";
	}

	output_file.close();
}
auto get_file_name(std::string& name)
{
	auto end = name.find(".");
	return name.substr(0, end);
}
int main()
{
	std::vector<std::string> input_files = 
	{
		/*"bier127.tsp",
		"ch130.tsp",
		"eil51.tsp",
		"kroA150.tsp",
		"kroA200.tsp",
		"kroB150.tsp",
		"kroB200.tsp",
		"lin105.tsp",
		"pr76.tsp",
		"pr107.tsp",*/
		"pr124.tsp"
		/*"pr136.tsp",
		"pr144.tsp",
		"pr152.tsp",
		"pr226.tsp",
		"pr264.tsp",
		"pr299.tsp",
		"rat195.tsp",
		"ts225.tsp",
		"u159.tsp"*/
	};
	for (auto& name : input_files)
	{
		std::string file_name = { "TSPLIB\\" + name };
		std::ifstream file(file_name);
		std::string line;
		std::vector<Node> nodes;
		size_t dim = 0;
		if (file.is_open())
		{
			while (std::getline(file, line))
			{
				if (line.find("DIMENSION") != std::string::npos)
				{
					auto start = line.find_last_of(" ");
					auto dim_str = line.substr(start, line.size() - start);
					dim = std::stoi(dim_str);
					std::cout << "Dim: " << dim << "\n";
				}
				if (line.find("NODE_COORD_SECTION") != std::string::npos)
					break;
			}


			nodes.reserve(dim);
			while (std::getline(file, line) && line.compare("EOF") != 0)
			{
				//std::cout << line << "\n";
				std::regex ws_re("\\s+");
				std::vector<size_t> values_pack;
				values_pack.reserve(3);
				std::for_each(
					std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1),
					std::sregex_token_iterator(),
					[&values_pack](const std::string& token)
				{
					if (token.compare("") != 0)
						values_pack.emplace_back(std::stoi(token, nullptr));
				}
				);
				Node n;
				n.name = values_pack[0];
				n.x = values_pack[1];
				n.y = values_pack[2];

				nodes.emplace_back(n);

			}
		}
		else
		{
			std::cerr << "Cannot read input file\n";
		}

		auto mat = dist_mat(nodes);

		export_file(mat, dim, get_file_name(name));
	}
	
	std::cout << "done\n";
	std::cin.get();
	return 0;
}