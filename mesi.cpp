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
	struct MainMemory* mem=new MainMemory();
	struct Bus* bus =new  Bus();
	struct Processor* p1=new Processor();
	p1->index=0;
	struct Processor* p2=new Processor();
	p2->index=1;
	int i;
	for(i=0;i<1024;i++)
		mem->block[i].addr=i;
//starting
	int count=1;
   while(count<100000)
   {
	int procNum=rand()%2;
	int block_no=rand()%1024;
	struct Block blk=mem->block[block_no];
//	int word_no=rand()%8;
//    blk.state='M';
	int readwrite=rand()%2;


	//cout<<processor<<"  "<<block_no<<" "<<"  "<<readwrite;
	//read 0,write 1

	if(!readwrite)    //read
	{
		struct Processor* processor=new Processor();;
		struct Processor* other_processor=new Processor();;
		if(procNum==0)
			{
				processor=p1;
				other_processor=p2;
			}	
		else
		{
			processor=p2;
			other_processor=p1;
		}
		struct Cache* this_cache=&processor->cache;
		struct Cache* other_cache=&other_processor->cache;


//hit
		int readhit=0;
        for(i=0;i<32;i++)
			{
			if((this_cache->block[i]).addr==blk.addr)
			{
				//other_cache.block[i].state='I';
				readhit=1;
				cout<<"hit at processor"<<(procNum+1)<<" at addr" << i<<endl;
			}
			break;
		    }

//miss

 	if(!readhit)
 	{
 	//	cout<<"read request on bus by processor"<<(procNum+1)<<endl;
 		bus->CacheCoherenceProtocol="read_req";
 		//cout<<"hey";
 		int memoryhit=1;
 		for(i=0;i<32;i++)
			{
		//		cout<<"loop";
			if((other_cache->block[i]).addr==blk.addr)
			{
				//other_cache.block[i].state='I';
				//readhit=1;
				//cout<<"hit at processor"<<(procNum+1)<<" at addr" << i;
				if((other_cache->block[i]).state=='M')
				{
					cout<<"read miss "<<" other cache M"<<endl;
					cout<<"no memory access required"<<endl;
					this_cache->block[blk.addr]=other_cache->block[i];
					(this_cache->block[blk.addr]).state='S';  //doubt
					(other_cache->block[i]).state='S';
					mem->block[blk.addr]=other_cache->block[i];
                   
				}
				else if((other_cache->block[i]).state=='E'||(other_cache->block[i]).state=='S')
				{
					cout<<"read miss "<<" other cache E or S"<<endl;
					cout<<"no memory access required"<<endl;
					this_cache->block[blk.addr]=other_cache->block[i];
					(this_cache->block[blk.addr]).state='S';  //doubt
					(other_cache->block[i]).state='S';
					//mem.block[blk.addr]=other_cache.block[i];

				}
				memoryhit=0;
			 break;	
			}
		//	cout<<"if over";
			
		}

		if(memoryhit)
		{
	//		cout<<"memory hit"<<endl;
			this_cache->block[(blk.addr%32)]=mem->block[blk.addr];
			this_cache->block[blk.addr].state='E';

		}


 	}
       




		// bus.CacheCoherenceProtocol="invalidate";
		// int other_processor_index=1-processor;
		// struct other_processor;
		// if(other_processor_index==0)
		// 	other_processor=p1;
		// else
		// 	other_processor=p2;

		// struct Cache other_cache=other_processor.cache;
		// for(i=0;i<32;i++)
		// 	{
		// 	if(other_cache.block[i].addr==blk.addr)
		// 	{
		// 		other_cache.block[i].state='I';
		// 	}
		// 	break;
		// }


	}
	else //write
	{
    // cout<<"write"<<endl;

	}

count++;
}

	return 0;

}
