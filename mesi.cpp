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
	int coherence=0;
	int MtoS=0;
	int MtoM=0;
	int MtoI=0;
	int StoS=0;
	int EtoS=0;
	int StoI=0;
	int EtoI=0;
	int memtoE=0;
	int MemtoM=0;
	int MemtoS=0;
	int EtoM=0;
	int StoM=0;
	int EtoE=0;
	int CachetoM=0;
	int reads=0;
	int writes=0;
	int readhitCount=0;
	int readmissCount=0;
	int writehitCount=0;
	int writemissCount=0;

	while(count<100000)
	{
			//cout << count<<endl;
		int procNum=rand()%2;
		int block_no=rand()%1024;
		struct Block blk=mem->block[block_no];
		//	int word_no=rand()%8;
		//    blk.state='M';
		int write=rand()%2;
		cout<<"============================="<<endl;
        if(write==0)
		cout<<"processor "<<(procNum+1)<<"  block number "<<block_no<<" read operation "<<endl;
		else
		cout<<"processor "<<(procNum+1)<<"  block number "<<block_no<<" write operation "<<endl;
		//read 0,write 1

		struct Processor* processor=new Processor();;
		struct Processor* other_processor=new Processor();
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


		if(!write)    //read
		{
			reads++;


			//hit
			int readhit=0;
			for(i=0;i<32;i++)
			{
				if((this_cache->block[i]).addr==blk.addr)
				{
					//other_cache.block[i].state='I';
					readhit=1;
					readhitCount++;
					if(this_cache->block[i].state=='E')
					EtoE++;
					cout<<"hit at processor"<<(procNum+1)<<" at addr" << i<<endl;
				}
				break;
			}

			//miss

			if(!readhit)
			{
				readmissCount++;
				cout<<"read request on bus by processor"<<(procNum+1)<<endl;
				bus->CacheCoherenceProtocol="read_req";
				coherence++;
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
							this_cache->block[blk.addr%32]=other_cache->block[i];
							(this_cache->block[blk.addr%32]).state='S';  //doubt
							
							MtoS++;
							(other_cache->block[i]).state='S';
							mem->block[blk.addr]=other_cache->block[i];

						}
						else if((other_cache->block[i]).state=='E'||(other_cache->block[i]).state=='S')
						{
							cout<<"read miss "<<" other cache E or S"<<endl;
							cout<<"no memory access required"<<endl;
							this_cache->block[blk.addr%32]=other_cache->block[i];
							(this_cache->block[blk.addr%32]).state='S';  //doubt
							if(other_cache->block[i].state=='E')
							EtoS++;
							else
							StoS++;

							MemtoS++;
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
					cout<<"read miss ,fetching from memory"<<endl;
					this_cache->block[(blk.addr%32)]=mem->block[blk.addr];
					this_cache->block[blk.addr%32].state='E';
					memtoE++;

				}


			}




		}
		else //write
		{
			//cout<<"write"<<endl;
			writes++;
			int writehit=0;
			for(i=0;i<32;i++)
			{
				if((this_cache->block[i]).addr==blk.addr)
				{
					//other_cache.block[i].state='I';
					writehit=1;
					writehitCount++;
				//	cout<<"write hit at processor"<<(procNum+1)<<" at addr" << i<<endl;
					if(this_cache->block[i].state=='M')
					{
						cout<<"value modified from M to M"<<endl;
						MtoM++;

					}
					else if(this_cache->block[i].state=='E')
					{
						this_cache->block[i].state='M';
						cout<<"value modified from E to M"<<endl;
						EtoM++;

					}
					else if(this_cache->block[i].state=='S')
					{
						bus->CacheCoherenceProtocol="Invalidate";
						cout<<"Invalidate on bus in write hit"<<endl;
                        coherence++;
 						(this_cache->block[blk.addr]).state='M'; 
 						StoM++;
						for(i=0;i<32;i++)
						{
							//		cout<<"loop";
							if((other_cache->block[i]).addr==blk.addr)
							{
								(other_cache->block[i]).state='I';
								StoI++;
							}
							//	cout<<"if over";

						}



					}
					break;
				}
				
			}

			if(writehit==0)
			{
				writemissCount++;
				int hitInOtherCache=0;
				for(i=0;i<32;i++)
			{
				if((other_cache->block[i]).addr==blk.addr)
				{
					hitInOtherCache=1;
					if(other_cache->block[i].state=='E'||other_cache->block[i].state=='S')
					{
						this_cache->block[blk.addr%32]=mem->block[blk.addr];
						bus->CacheCoherenceProtocol="RWITM";
						coherence++;
						cout<<"write miss bus protocol read with intent to modify from other cache E or S"<<endl;
						this_cache->block[blk.addr%32].state='M';
						CachetoM++;
						if(other_cache->block[i].state=='E')
							EtoI++;
						else
							StoI++;
						other_cache->block[i].state='I';
						
						

					}
					else if(other_cache->block[i].state=='M')
					{
						bus->CacheCoherenceProtocol="RWITM";
						cout<<"Modify1 -- write miss bus protocol read with intent to modify from other Cache M"<<endl;
						coherence++;
						cout<<"write back"<<endl;
                        mem->block[blk.addr]=other_cache->block[i];
						other_cache->block[i].state=='I';
						MtoI++;
						cout<<"Modify2 -- write miss bus protocol read with intent to modify from other Cache M"<<endl;
						coherence++;
						this_cache->block[blk.addr%32]=mem->block[blk.addr];
						this_cache->block[blk.addr%32].state='M';
						CachetoM++;


					}
					break;
				}
			}
			if(!hitInOtherCache)
			{
				coherence++;
				cout<<"write miss fetch from memory and changing fetched value to M"<<endl;
				this_cache->block[blk.addr%32]=mem->block[blk.addr];
				this_cache->block[blk.addr%32].state='M';
				MemtoM++;
			}



			}


		}

		count++;
	}

	cout<<"no of reads are "<<reads<<endl;
	cout<<"no of writes are "<<writes<<endl;
	cout<<"no of read hit are "<<readhitCount<<endl;
	cout<<"no of read miss are "<<readmissCount<<endl;
	cout<<"no of write hit are "<<writehitCount<<endl;
	cout<<"no of write miss are "<<writemissCount<<endl;
	cout<<"No of bus transactions are "<<coherence<<endl;
	cout<<"No of M to S state changes are "<<MtoS<<endl;
	cout<<"No of E to S state changes are "<<EtoS<<endl;
	cout<<"No of E to M state changes are "<<EtoM<<endl;
	cout<<"No of E to E state changes are "<<EtoE<<endl;
	cout<<"No of S to M state changes are "<<StoM<<endl;
	cout<<"No of S to S state changes are "<<StoS<<endl;
	cout<<"No of M to M state changes are "<<MtoM<<endl;
		cout<<"No of M to I state changes are "<<MtoI<<endl;
	cout<<"No of E to I state changes are "<<EtoI<<endl;
	cout<<"No of S to I state changes are "<<StoI<<endl;
	cout<<"memory to exclusive changes are "<<memtoE<<endl;
	cout<<"memory to Modified changes are "<<MemtoM<<endl;
	cout<<"memory to shared changes are "<<MemtoS<<endl;
	cout<<"cache to Modified are "<<CachetoM<<endl;

	return 0;

}
