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

std::string to_hex(uint16_t val, uint bytes)
{
	if (bytes > 2 || bytes < 1)
		return std::string();

	char buf[5];
	if (bytes - 1 == 0)
		snprintf(buf, sizeof(buf), "%02X", val);
	else
		snprintf(buf, sizeof(buf), "%04X", val);

	return std::string(buf);
}
