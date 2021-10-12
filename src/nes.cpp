#include <nes.hpp>

NES::NES(std::ifstream &ines)
	: cpu([](uint16_t addr){ return 0; }, [](uint16_t addr, uint8_t n){})
{
	
}

