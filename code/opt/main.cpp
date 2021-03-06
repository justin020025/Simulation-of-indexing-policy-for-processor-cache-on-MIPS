#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <cmath>
#include <iomanip>
#include <math.h>

using namespace std;

int hit = 0;
int miss = 0;

class cache_set_LRU
{
public:
	unsigned int n;
	int com_bits;
	list<string> set_;
	cache_set_LRU(){};
	cache_set_LRU(int com_bits = 4, int n_block = 4)
	{
		this->n = (unsigned int) n_block;
		this->com_bits = com_bits;
	};

	void access(string a, unsigned int i, int *ifhit) // i¬ö¿ýaddr index
	{
		string compare = a.substr(0, com_bits);
		if(set_.size() == 0)
		{
			set_.push_back(compare);
			miss++;
		}
		else
		{
			auto it = find(set_.begin(), set_.end(), compare); //
			if(it != set_.end())
			{
				hit++;
				ifhit[i] = 1;
				set_.erase(it);
				set_.push_back(compare);
			}
			else
			{
				if(set_.size() == n)
				{
					miss++;
					set_.erase(set_.begin());
					set_.push_back(compare);
				}
				else
                {
                    set_.push_back(compare);
                    miss++;
                }
			}
		}


	};

};


int to_deci(string str)
{
	int len = str.length();
	int total = 0;
	for(int i=0; i<len; i++)
	{
		//total += atoi(*str.at(i))*pow(2, len-1-i);
		if(str[i] == '1')
            total += pow(2, len-1-i);
	}
	return total;
}

int main(int argc, char **argv )
{
	//read cache arch
	if(argc < 3)
	{
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
	}
	ifstream cache_info(argv[1]);
	if(!cache_info)
	{
		cout<<"filename "<<argv[1]<< "does not exist!\n";
		return 0;
	}
	string str;
	int cache_para[4];
	int i = 0;
	while(cache_info>>str)
	{
		cache_info>>cache_para[i];
		i++;
	}
	cache_info.close();

	//read memory access
	ifstream mem_info(argv[2], ifstream::in);
	if(!mem_info)
	{
		cout<<"filename "<<"argv[2] "<< "does not exist!\n";
		return 0;
	}

	string title, temp;
	vector<string> addr;
	getline(mem_info, title);
	while(mem_info>>temp)
	{
		addr.push_back(temp);
	}
	addr.pop_back();
	//addr.erase(addr.begin());

	mem_info.close();

	int ifhit[addr.size()] = {0}; //¬ö¿ýhit/miss


	int addr_bits = cache_para[0];
	int n_sets = cache_para[1];
	int block_per_set = cache_para[2];
	int block_size = cache_para[3];
	int idx_bits = ceil(log(n_sets)/log(2));
	int offset = ceil(log(block_size)/log(2));
	int tag_bits = addr_bits-idx_bits-offset;


    /*
    //LSB
	vector<cache_set_LRU>  cache_LRU(n_sets, cache_set_LRU(tag_bits + idx_bits, block_per_set));

	for(unsigned int i=0; i<addr.size(); i++)
	{
		int index = to_deci(addr[i].substr(tag_bits, idx_bits));
		cache_LRU[index].access(addr[i], i, ifhit);
	}
	*/


	// optimize
	int com_bits = idx_bits+tag_bits;
	int bit_use[addr_bits] = {0};
	float Q[com_bits] = {0};

    for(int i=0; i< com_bits; i++)
    {
        float n1 = 0;
        float n0 = 0;
        for(unsigned int j=0; j< addr.size(); j++)
        {
            if(addr[j][i] == '1')
                n1++;
            else
                n0++;
        }
        Q[i] = min(n1,n0)/max(n1,n0);
    }
    /*for(int i=0; i<com_bits; i++)
    {
        cout<<Q[i];
    }
    cout<<endl;
    */
    float C[com_bits][com_bits];

    for(int i=0; i< com_bits; i++)
    {
        for(int j=0; j< com_bits; j++)
        {
            float E = 0;
            float D = 0;
            for(unsigned int k=0; k< addr.size(); k++)
            {
                if(addr[k][i] == addr[k][j])
                    E++;
                else
                    D++;
            }
            C[i][j] = min(E,D)/max(E,D);
        }
    }
    /*for(int i=0; i< com_bits; i++)
    {
        for(int j=0; j< com_bits; j++)
        {
            cout<<C[i][j]<<' ';
        }
        cout<<endl;
    }
    cout<<endl<<endl<<endl;*/

    vector<int> bit2use;
    while(bit2use.size() < idx_bits)
    {
        int Q_idx = -1;
        float Q_val = 0;
        for(int i=0; i<com_bits; i++)
        {
            if(Q[i]>Q_val)
            {
                Q_val = Q[i];
                Q_idx = i;

                //cout<<"Q_val"<<Q_val<<' '<<"Qidx"<<Q_idx<<endl;
            }
        }
        if(Q_idx == -1)
        {
            Q_idx = 0;
            for(int j=0; j<com_bits ; j++)
            {
                if(bit_use[Q_idx] == 1)
                    Q_idx++;
                else
                    break;

            }
        }

        bit2use.push_back(Q_idx);
        bit_use[Q_idx] = 1;

        for(int i=0; i<com_bits; i++)
        {
            Q[i] *= C[Q_idx][i];
        }
    }

    for(int i=0; i<com_bits; i++)
    {
        cout<<bit_use[i]<<' ';
    }
    cout<<endl;
    //algo end


    vector<cache_set_LRU>  cache_LRU_opt(n_sets, cache_set_LRU(tag_bits + idx_bits, block_per_set));

	for(unsigned int i=0; i<addr.size(); i++)
	{
	    string index_str;
	    for(int j=0; j<com_bits; j++)
        {
            if(bit_use[j] == 1)
                index_str+=addr[i][j];
        }
		int index = to_deci(index_str);
		//cout<<index<<endl;
		cache_LRU_opt[index].access(addr[i], i, ifhit);
	}









	ofstream output(argv[3]);
	output << "Address bits: " << cache_para[0] <<endl;
    output << "Number of sets: " << cache_para[1] <<endl;
    output << "Associativity: " << cache_para[2] <<endl;
    output << "Block size: " << cache_para[3] <<endl << endl;
    output << "Indexing bit count: " << idx_bits<< endl;
    output << "Indexing bits:";
    for(int i = 0; i< com_bits ; i++)
    {
        if(bit_use[i] == 1)
        {
            output<<' '<<addr_bits-1-i;
        }
    }
    output << endl;
    output << "Offset bit count: " << offset << endl << endl;
    output << title << endl;
    for(unsigned int i = 0; i<addr.size(); i++)
    {
        output << addr[i] << ' ';
        if(ifhit[i] == 0)
            output<<"miss"<<endl;
        else
            output<<"hit"<<endl;
    }

    output << ".end" << endl << endl << "Total cache miss count: " << miss;
    output.close();
    //cout<<"---------------------------------------------"<<endl;

    return 0;













}
