#include<iostream>
#include<string>
#include<stdlib.h>
#include<time.h>
using namespace std;
struct Word
{
	//4

	int word_size;
};


struct Block
{
	int addr;
	int block_size; //32
	struct Word word[8];
	char state;
};
struct Cache
{
	struct Block block[32];
	int size;
};

struct Processor
{
	int index;
	struct Cache cache;

};

struct MainMemory
{
	struct Block block[1024];

};

struct Bus
{
	string CacheCoherenceProtocol;
};
int main()
{

	srand(time(NULL));
	struct MainMemory mem;
	struct Bus bus;
	struct Processor p1;
	p1.index=0;
	struct Processor p2;
	p2.index=1;
	int i;
	for(i=0;i<1024;i++)
		mem.block[i].addr=i;


	int processor=rand()%2;
	int block_no=rand()%1024;
	struct Block blk=mem.block[block_no];
	int word_no=rand()%8;
    blk.state='M';
	int readwrite=rand()%2;


	cout<<processor<<"  "<<block_no<<" "<<word_no<<"  "<<readwrite;
	//read 0,write 1

	if(writeflag)
	{
		bus.CacheCoherenceProtocol="invalidate";
		int other_processor_index=1-processor;
		struct other_processor;
		if(other_processor_index==0)
			other_processor=p1;
		else
			other_processor=p2;

		struct Cache other_cache=other_processor.cache;
		for(i=0;i<32;i++)
			{
			if(other_cache.block[i].addr==blk.addr)
			{
				other_cache.block[i].state='I';
			}
			break;
		}


	}

	return 0;

}
