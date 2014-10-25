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
		//	cout << count<<endl;
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
					cout<<"read request on bus by processor"<<(procNum+1)<<endl;
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
							this_cache->block[blk.addr%32]=other_cache->block[i];
							cout<<"moesi S O read miss"<<endl;
							(this_cache->block[blk.addr%32]).state='S';  //doubt
							(other_cache->block[i]).state='O';
							

						}
						else if((other_cache->block[i]).state=='E'||(other_cache->block[i]).state=='S')
						{
							cout<<"read miss "<<" other cache E or S"<<endl;
							cout<<"no memory access required"<<endl;
							this_cache->block[blk.addr%32]=other_cache->block[i];
							(this_cache->block[blk.addr%32]).state='S';  //doubt
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
							cout<<"read miss,fetching from memory "<<endl;
					this_cache->block[(blk.addr%32)]=mem->block[blk.addr];
					this_cache->block[blk.addr%32].state='E';

				}


			}




		}
		else //write
		{
			//cout<<"write"<<endl;
			int writehit=0;
			for(i=0;i<32;i++)
			{
				if((this_cache->block[i]).addr==blk.addr)
				{
					//other_cache.block[i].state='I';
					writehit=1;
				//	cout<<"write hit at processor"<<(procNum+1)<<" at addr" << i<<endl;
					int j;
					for(j=0;j<32;j++)
			{
					if((other_cache->block[j]).addr==blk.addr && other_cache->block[j].state=='O')
					{	other_cache->block[j].state='I';
						cout<<"Invalidate block "<<j<<" at other cache in write hit"<<endl;
                     }
			}
					if(this_cache->block[i].state=='M')
					{
						cout<<"value modified from M to M"<<endl;

					}
					else if(this_cache->block[i].state=='E')
					{
						this_cache->block[i].state='M';
						cout<<"value modified from E to M"<<endl;

					}
					else if(this_cache->block[i].state=='O')
					{
						this_cache->block[i].state='M';
						cout<<"value modified from O to M"<<endl;

					}
					else if(this_cache->block[i].state=='S')
					{
						bus->CacheCoherenceProtocol="Invalidate";
						cout<<"Invalidate on bus in write hit"<<endl;
                     
 						(this_cache->block[blk.addr]).state='M'; 

						for(i=0;i<32;i++)
						{
							//		cout<<"loop";
							if((other_cache->block[i]).addr==blk.addr)
							{
								(other_cache->block[i]).state='I';
								
							}
							//	cout<<"if over";

						}



					}
					break;
				}
				
			}

			if(writehit==0)
			{
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
						cout<<"write miss bus protocol read with intent to modify from other cache E or S"<<endl;
						this_cache->block[blk.addr%32].state='M';
						other_cache->block[i].state='I';
					}
					else if(other_cache->block[i].state=='M')
					{
						bus->CacheCoherenceProtocol="RWITM";
						cout<<"Modify1 -- write miss bus protocol read with intent to modify from other Cache M"<<endl;
						cout<<"write back"<<endl;
                        mem->block[blk.addr]=other_cache->block[i];
						other_cache->block[i].state=='I';
						cout<<"Modify2 -- write miss bus protocol read with intent to modify from other Cache M"<<endl;
						this_cache->block[blk.addr%32]=mem->block[blk.addr];
						this_cache->block[blk.addr%32].state='M';


					}
					break;
				}
			}
			if(!hitInOtherCache)
			{
				cout<<"write miss fetch from memory and changing fetched value to M"<<endl;
				this_cache->block[blk.addr%32]=mem->block[blk.addr];
						this_cache->block[blk.addr%32].state='M';

			}



			}


		}

		count++;
	}

	return 0;

}
