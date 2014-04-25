#include "Bloomfilter.h"
#include "BloomFilters.h"
#include <sstream>

using std::stringstream;
using std::cout;
using std::endl;

namespace ns3 {

int Bloomfilter::COUNT_BLOOMFILTERS = 0;

uint8_t Bloomfilter::bitMasks[8] = {128, 64, 32, 16, 8, 4, 2, 1};

Bloomfilter::Bloomfilter(uint32_t length) {
	COUNT_BLOOMFILTERS--;
	this->length = length;
	filter = (uint8_t*)calloc(length, sizeof(uint8_t));
}

Bloomfilter::Bloomfilter(uint32_t length, uint8_t *bits) {
	COUNT_BLOOMFILTERS--;
	this->length = length;
	filter = (uint8_t*)calloc(length, sizeof(uint8_t));
	memcpy((void*)filter, (void*)bits, length);
}

Bloomfilter::~Bloomfilter() {
	if (filter){
		free(filter);
		filter = 0;
	}
}

Bloomfilter::Bloomfilter(string& bits) {
	COUNT_BLOOMFILTERS--;
	this->length = bits.length()/8;
	filter = (uint8_t*)calloc(length, sizeof(uint8_t));

	for(uint32_t i=0; i<bits.length(); i++){
		if (bits[i] == '1'){
			setBit(i);
		}
	}
}

void Bloomfilter::DoDispose(void) {
	if (filter){
		free(filter);
		filter = 0;
	}

	COUNT_BLOOMFILTERS--;

	if(COUNT_BLOOMFILTERS==0)
	{
		std::cout<<"BLOOMFILTERS over."<<std::endl;
	}
}

bool Bloomfilter::setBit(uint32_t bitPosition){
	if (bitPosition >= length*8){
		return false;
	}

	uint32_t bytePosition = bitPosition / 8;
	uint32_t inByteBit = bitPosition % 8;
	uint8_t bitMask = Bloomfilter::bitMasks[inByteBit];
	filter[bytePosition] = filter[bytePosition] | bitMask;
	return true;
}

void Bloomfilter::OR(Ptr<Bloomfilter> bf){
	if(this==0)
	{
		std::cout<<"this is null"<<std::endl;
	}

	for (uint32_t i=0; i<length; i++){
		filter[i] = filter[i] | bf->getBuffer()[i];
	}
}

bool Bloomfilter::contains(Ptr<Bloomfilter> bf) const{
	if (length != bf->getLength()){
		return false;
	}

	for (uint32_t byte=0; byte<length; byte++){
		if ((filter[byte] & bf->getByte(byte)) != bf->getByte(byte)){
			return false;
		}
	}

	return true;
}

Ptr<Bloomfilter> Bloomfilter::AND(Ptr<Bloomfilter> bf) const{
	Ptr<Bloomfilter> newBF = CreateObject<Bloomfilter>(length, filter);
	newBF->OR(bf);
	return newBF;
}

string Bloomfilter::toString() const {
	stringstream ss;
	for (uint32_t byte=0; byte<length; byte++){
		uint8_t theByte = filter[byte];
		for(uint32_t bit=0; bit<8; bit++){
			uint8_t mask = Bloomfilter::bitMasks[bit];
			if ((theByte & mask) == mask){
				ss << '1';
			}else{
				ss << '0';
			}
		}
	}
	return ss.str();
}

uint32_t Bloomfilter::serializedSize() const{
	return length;
}

uint32_t Bloomfilter::serializeToBuffer(uint8_t *buffer) const{
	memcpy(buffer, filter, length);
	return length;
}

pair<Ptr<Bloomfilter>, uint32_t> Bloomfilter::deserializeFromBuffer(uint8_t *buff){
	uint32_t bf_len = Bloomfilters::BF_LENGTH;
	Ptr<Bloomfilter> bf = CreateObject<Bloomfilter>(bf_len, buff);
	return pair<Ptr<Bloomfilter>, uint32_t>(bf, bf_len);
}

bool operator<(const Ptr<Bloomfilter>& left, const Ptr<Bloomfilter>& right) {
	if (left->getLength() < right->getLength()) {
		return true;
	} else if (left->getLength() > right->getLength()) {
		return false;
	} else {
		uint8_t *lBuff = left->getBuffer();
		uint8_t *rBuff = right->getBuffer();

		for (uint32_t i = 0; i < left->getLength(); i++) {
			if (lBuff[i] < rBuff[i]){
				return true;
			}else if (lBuff[i] > rBuff[i]){
				return false;
			}
		}
		return false;
	}
}

bool operator== (const Ptr<Bloomfilter>& lhs, const Ptr<Bloomfilter>& rhs){
	if (lhs->getLength() != rhs->getLength()){
		return false;
	}else{
		for (uint32_t i=0; i<lhs->getLength(); i++){
			if (lhs->getBuffer()[i] != rhs->getBuffer()[i]){
				return false;
			}
		}
		return true;
	}
}

bool operator!= (const Ptr<Bloomfilter>& lhs, const Ptr<Bloomfilter>& rhs){
	return lhs != rhs;
}

}
