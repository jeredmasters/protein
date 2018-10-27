#include "stdafx.h"
#include "chromosome.h"

using namespace std;

uint8_t get_nybble(std::uint16_t number, const unsigned short part)
{
	return (number >> (4 * part)) & 0xF;
}

chromosome::chromosome(std::vector<uint16_t> _dna) {
	dna = _dna;
	fitness = 0;
}

chromosome::chromosome(const char * gene[], int size) :
	dna()
{
	for (int i = 0; i < size; i+=4) {
		int sum = 0;
		for (int j = 0; j < 4; j++) {
			sum += atoi(gene[i + j]) * (int)pow(16, j);
		}
		dna.push_back(sum);
	}
}

chromosome::chromosome(string str) :
	dna()
{
	fitness = 0;
	int index = 0;
	while (str.length() > 0) {
		index = str.find(' ');
		string num = str.substr(0, index);
		index++;
		str = str.substr(index, str.length() - index);
		dna.push_back(atoi(num.c_str()));
	}
}
chromosome * chromosome::clone() {
	std::vector<uint16_t> temp;
	temp.reserve(dna.size());
	for (int i = 0; i < dna.size(); i++) {
		temp.push_back(dna[i]);
	}
	return new chromosome(temp);
}
uint8_t * chromosome::get(int i) {
	uint16_t number = dna[i];
	uint8_t * retval = new uint8_t[4];
	retval[0] = get_nybble(number, 0);
	retval[1] = get_nybble(number, 1);
	retval[2] = get_nybble(number, 2);
	retval[3] = get_nybble(number, 3);
	return retval;
}

std::string chromosome::toString() {
	std::stringstream ss;

	for (int i = 0; i < dna.size(); i++) {
		uint8_t * data = get(i);
		ss << (int)data[0] << " " << (int)data[1] << " " << (int)data[2] << " " << (int)data[3] << " ";
		delete data;
	}

	return ss.str();
}

chromosome::~chromosome() {
	dna.clear();
}