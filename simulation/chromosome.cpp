#include "stdafx.h"
#include "chromosome.h"

using namespace std;

uint8_t get_nybble(std::uint16_t number, const unsigned short part)
{
	if (part > 3)
		throw std::out_of_range("'part' must be a number between 0 and 3");
	uint8_t n = (number >> (4 * part)) & 0xF;
	return n;
}

uint8_t * explode(std::uint16_t number) {
	uint8_t * retval = new uint8_t[4];
	retval[0] = get_nybble(number, 0);
	retval[1] = get_nybble(number, 1);
	retval[2] = get_nybble(number, 2);
	retval[3] = get_nybble(number, 3);
	return retval;
}


chromosome::chromosome(std::vector<uint16_t> _dna) {
	dna = _dna;
	fittness = 0;
}

chromosome::chromosome(string str) :
	dna()
{
	fittness = 0;
	int index = 0;
	while (str.length() > 0) {
		index = str.find(' ');
		string num = str.substr(0, index);
		index++;
		str = str.substr(index, str.length() - index);
		dna.push_back(atoi(num.c_str()));
	}
}

uint8_t * chromosome::get(int i) {
	return explode(dna[i]);
}

std::string chromosome::toString() {
	std::stringstream ss;

	for (int i = 0; i < dna.size(); i++) {
		uint8_t * data = get(i);
		ss << (int)data[0] << " " << (int)data[1] << " " << (int)data[2] << " " << (int)data[3] << " ";
	}

	return ss.str();
}

chromosome::~chromosome() {
	dna.clear();
}