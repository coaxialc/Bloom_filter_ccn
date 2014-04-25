
#ifndef BLOOMFILTER_H_
#define BLOOMFILTER_H_

#include "ns3/core-module.h"
#include <string>
#include <map>

using std::string;
using std::pair;

namespace ns3{

class Bloomfilter : public Object
{
	public:

	static int COUNT_BLOOMFILTERS;
	Bloomfilter(uint32_t);
	Bloomfilter(uint32_t ,uint8_t *);
	Bloomfilter(string &);

	~Bloomfilter();

	virtual void DoDispose (void);

	uint32_t getLength() const {return length;}
	uint8_t *getBuffer(){return filter;}
	uint8_t getByte(uint32_t pos) const {return filter[pos];}

	bool setBit(uint32_t);
	void OR(Ptr<Bloomfilter> bf);
	bool contains(Ptr<Bloomfilter>) const;
	Ptr<Bloomfilter> AND(Ptr<Bloomfilter> bf) const;
	string toString() const;

	uint32_t serializedSize() const;
	uint32_t serializeToBuffer(uint8_t *) const;
	static pair<Ptr<Bloomfilter>, uint32_t> deserializeFromBuffer(uint8_t *);

	friend bool operator< (const Ptr<Bloomfilter>&, const Ptr<Bloomfilter>&);
	friend bool operator== (const Ptr<Bloomfilter>&, const Ptr<Bloomfilter>&);
	friend bool operator!= (const Ptr<Bloomfilter>&, const Ptr<Bloomfilter>&);

	private:
	static uint8_t bitMasks [8];

	uint8_t * filter;
	uint32_t length;
};

}

#endif
