#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <sstream>
#define MAXSIZE 2000
#include <omp.h>
#define THREADS_NUM 4
using namespace std;

class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};

index s;
index temp;
void sorted(int* list, vector<index>& idx, int num) {
	for (int i = 0; i < num - 1; i++) {
		for (int j = 0; j < num - i - 1; j++) {
			if (idx[list[j]].len > idx[list[j + 1]].len) {
				int tmp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = tmp;
			}
		}
	}
}
index* s0=new index[THREADS_NUM];
vector<index> idx;
class BitMap
{
public:
	BitMap(int range)
	{
		//开辟空间
		this->m_bits.resize(range / 32 + 1);
		this->fir_index.resize(range / 1024 + 1);
		this->sec_index.resize(range / 32768 + 1);
	}

	void set(int data)
	{
		int index0 = data / 32; //数组索引即区间
		int index1 = index0 / 1024;
		int index2 = index1 / 1024;
		int temp0 = data % 32; //具体位置索引
		int temp1 = temp0 / 32;
		int temp2 = temp1 / 32;

		this->m_bits[index0] |= (1 << temp0); //左移4位置为1
		this->fir_index[index1] |= (1 << temp1);
		this->sec_index[index2] |= (1 << temp2);
	}

	void reset(int data)
	{
		int index = data / 32;
		int temp = data % 32;
		this->m_bits[index] &= ~(1 << temp); //取反
	}
	vector<int> m_bits;
	vector<int> fir_index;
	vector<int> sec_index;
};
BitMap s_(30000000);
vector<index> idx_;
void compare(int* list, vector<index>& idx, int num)
{

	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	sort(idx_.begin(), idx_.end());
	vector<BitMap> bitmap;
	for (int i = 0; i < num; i++)
	{
		bitmap.push_back(30000000);
		for (int j = 0; j < idx_[i].len; j++)
		{
			bitmap[i].set(idx_[i].arr[j]);
		}
	}
	int count = 0;
	bool isExist = false;
	s_ = bitmap[0];
	int i, j, t, l;
	bool parallel=true;
#pragma omp parallel if (parallel),num_threads(4),private(i,j,t,l)
#pragma omp for
	for ( i = 1; i < num; i++)
	{
		for ( j = 0; j < bitmap[0].sec_index.size(); j++)
		{
			if ((s_.sec_index[j] ^ bitmap[i].sec_index[j]) == 0 && s_.sec_index[j] != 0)//��λ���
			{
				for ( t = j * 32; t < j * 32 + 32; t++)
				{
					if ((s_.fir_index[t] ^ bitmap[i].sec_index[t]) == 0 && s_.fir_index[t] != 0)
					{
						for ( l = t * 32; l < t * 32 + 32; l++)
							if ((s_.m_bits[l] ^ bitmap[i].m_bits[l]) == 0 && s_.m_bits[l] != 0)
								isExist = true;
					}
				}
			}
			if (isExist == false)
			{
				s_.sec_index[j] = 0;
			}
		}
	}
}
int main() {
	//��ȡ�������ļ�
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}

	for (int i = 0; i < 2000; i++) {
		index temp;
		file.read((char*)&temp.len, sizeof(temp.len));
		for (int j = 0; j < (temp.len); j++)
		{
			unsigned int binary_temp;
			file.read((char*)&binary_temp, sizeof(binary_temp));
			temp.arr.push_back(binary_temp);
		}
		idx.push_back(temp);
	}
	file.close();
	/*---------------------------------------------------------------------------------------------------------*/
	file.open("ExpQuery", ios::in);
	int query[1000][5] = { 0 };
	string line;
	int count = 0;
	while (getline(file, line)) {
		stringstream ss; //������
		ss << line; //�����д�ֵ
		int a = 0;
		while (!ss.eof()) {
			int temp;
			ss >> temp;
			query[count][a] = temp;
			a++;
		}
		count++;
	}
	file.close();
	/*-----------------------------------------------------------------------------------------------------------*/


	//struct timespec sts, ets;
	//timespec_get(&sts, TIME_UTC);
	clock_t start,_end;
	start=clock();
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];//�����е���
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - ������-------- -
		sorted(list, idx, num);
		SVS_omp_baseonP(list, idx, num);
		delete list;
	}
	_end=clock();
	cout<<(_end-start)/5;
	/*timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec - sts.tv_nsec;
	long dnsec = ets.tv_nsec - sts.tv_nsec;
	printf(" % llu. % 09llus\n", dsec, dnsec);*/
	return 0;
}

