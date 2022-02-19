#include <util.hpp>

#include <iterator>
#include <fstream>

std::vector<uint8_t> readfile(const char *filename)
{
	std::streampos size;
	std::ifstream f(filename, std::ios::binary);

	f.unsetf(std::ios::skipws);

	f.seekg(0, std::ios::end);
	size = f.tellg();
	f.seekg(0, std::ios::beg);

	std::vector<uint8_t> data;
	data.reserve(size);
	data.insert(data.begin(),
	            std::istream_iterator<uint8_t>(f),
				std::istream_iterator<uint8_t>());

	return data;
}
