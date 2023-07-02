#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <arm_neon.h>
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <omp.h>
#define MAXSIZE 2000
using namespace std;

class BitMap
{
public:
	BitMap(int range)
	{
		//���ٿռ�
		this->m_bits.resize(range / 32 + 1);
		this->fir_index.resize(range / 1024 + 1);
		this->sec_index.resize(range / 32768 + 1);
	}

	void set(unsigned int data)
	{
		unsigned int index0 = data / 32; //��������������
		unsigned int index1 = index0 / 1024;
		unsigned int index2 = index1 / 1024;
		unsigned int temp0 = data % 32; //����λ������
		unsigned int temp1 = temp0 / 32;
		unsigned int temp2 = temp1 / 32;

		this->m_bits[index0] |= (1 << temp0); //����4λ��Ϊ1
		this->fir_index[index1] |= (1 << temp1);
		this->sec_index[index2] |= (1 << temp2);
	}

	void reset(int data)
	{
		int index = data / 32;
		int temp = data % 32;
		this->m_bits[index] &= ~(1 << temp); //ȡ��
	}
	vector<unsigned int> m_bits;
	vector<unsigned int> fir_index;
	vector<unsigned int> sec_index;
};




class Hash_List {
public:
	int beg = -1;
	int end = -1;
};
class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};


bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; //��С��������
}
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
index s;
index temp;
Hash_List** HL;
index& SVS_NEON(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s�б��еĸ���ֵ�Ƚ�
			bool isExit = false;
			uint32_t length = ceil(idx[list[i]].len / 4) * 4;
			for (int m = idx[list[i]].len ; m < length; m++)
				idx[list[i]].arr[m] = 0;
			for (int t = 0; t < length; t+=4 ) {//��i���б��ĸ���ֵ��s�б��еĵڡ�j]��ֵ�Ƚ�
				
				unsigned int res[4] = { 0 };
				uint32x4_t temp0 = vmovq_n_u32(s.arr[count]);
				uint32x4_t temp1 = vld1q_u32( &idx[list[i]].arr[t]);
				uint32x4_t result = vceqq_u32(temp0, temp1);
				vst1q_u32(res, result);
				if (res[0]==1||res[1]==1||res[2]==1||res[3]==1) {
					isExit = true;
					break;
				}
			}
			if (isExit == false) {
				s.len--;
				s.arr.erase(s.arr.begin() + count);//ɾ����s�е�j��Ԫ��
			}
			else {
				count++;
			}
		}
	}
	return s;
}




index& Zipper(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {
		temp.len = 0;
		temp.arr.clear();
		int p1 = 0, p2 = 0;
		int length = s.len; int count = 0;
		while (count < length && p2 < idx[list[i]].len) {//��ָ��δָ������ĩβʱ
			if (s.arr[p1] == idx[list[i]].arr[p2]) {
				temp.arr.push_back(s.arr[p1]);
				temp.len++;
				p1++; p2++;
			}
			else if (s.arr[p1] > idx[list[i]].arr[p2]) {
				p2++;
			}
			else {
				p1++;
			}
			count++;
		}
		s = temp;
	}
	return s;
}
Hash_List** preprocessor(vector<index> idx, Hash_List** HL) {
	//h(x)=[x/2**16];��������idx�еı��������������ϣ��
	unsigned int p = 0;
	for (int i = 0; i < MAXSIZE; i++) {//��ÿ��indexԤ����
		for (int j = 0; j < idx[i].len; j++) {
			int a = idx[i].arr[j];
			int b = floor(a / 65536);
			if (HL[i][b].beg == -1) {
				HL[i][b].beg = j;
				HL[i][b].end = j;
			}
			else {
				HL[i][b].end = j;
			}
		}
	}
	return HL;
}
index& Hash_S_NEON(int* list, vector<index>& idx, int num) {//���ڹ�ϣ���Ĵ���ʵ��
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s�б��еĸ���ֵ�Ƚ�
			bool isExit = false;
			int a = floor(s.arr[count] / 65536);
			int t = HL[list[i]][a].beg;
			int t1 = HL[list[i]][a].end;
			if (t < 0 || t1 < 0) {
				s.len--;
				s.arr.erase(s.arr.begin() + count);
				continue;
			}
			else {
				uint32_t length = ceil(idx[list[i]].len / 4) * 4;
				for (int m = idx[list[i]].len; m < length; m++)
					idx[list[i]].arr[m] = 0;
				for (t; t <= t1; t+=4) {//��i���б��ĸ���ֵ��s�б��еĵڡ�j]��ֵ�Ƚ�
					unsigned int res[4] = { 0 };
					uint32x4_t temp0 = vmovq_n_u32(s.arr[count]);
					uint32x4_t temp1 = vld1q_u32(&idx[list[i]].arr[t]);
					uint32x4_t result = vceqq_u32(temp0, temp1);
					vst1q_u32(res, result);
					if (res[0] == 1 || res[1] == 1 || res[2] == 1 || res[3] == 1) {
						isExit = true;
						break;
					}
				}
				if (isExit == false) {
					s.len--;
					s.arr.erase(s.arr.begin() + count);//ɾ����s�е�j��Ԫ��
				}
				else {
					count++;
				}
			}
		}
	}
	return s;
}
vector<int> the_index;
vector<int>& search(unsigned int pp[])
{
	int num = sizeof(*pp) / sizeof(unsigned int);
	for (int i = 0; i < num; i++)
	{
		if (pp[i] == 0)
			the_index.push_back(i);
	}
	return the_index;
}


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
	bool isExist = false;
	s_ = bitmap[0];
	unsigned int res[4] = { 0 };
	#pragma omp parallel,num_threads(THREADS_NUM)
	#pragma omp for
	for (int i = 1; i < num; i++)
	{
		for (int j = 0; j < bitmap[0].sec_index.size(); j+=4)
		{
			uint32x4_t zero_ = vmovq_n_u32(0);//��ʼ��Ϊȫ0�����ڱȽ�
			uint32x4_t temp0 = vmovq_n_u32(s_.sec_index[j]);//��0����¼�Ķ����������i����¼��ʼ�Ƚ�
			uint32x4_t temp1 = vld1q_u32(&bitmap[i].sec_index[j]);//��i����¼�ĵ�j��Ԫ�ؿ�ʼ����ȡ4��Ԫ��
			uint32x4_t result = vornq_u32(temp0, temp1);//��λ���
			uint32x4_t isZero = vceqq_u32(zero_, temp0);//�Ƚ�
			vst1q_u32(res, result);//result���res
			vector<int> the_index0 = search(res);
			int m = 0;
			while (!the_index0.empty() && s_.sec_index[j]!=0)
			{
				int t = the_index0[m];
				for (t = t * 32; t < t + 32; t+=4)
				{
					temp0 = vmovq_n_u32(s_.fir_index[t]);
					temp1 = vld1q_u32(&bitmap[i].fir_index[t]);
					result = vornq_u32(temp0, temp1);
					isZero = vceqq_u32(zero_, temp0);
					vst1q_u32(res, result);
					vector<int> the_index1 = search(res);
					int n = 0;
					while (!the_index1.empty() && s_.fir_index[t] != 0)
					{
						int l = the_index1[n];
						for (l = l * 32; l < l + 32; l += 4)
						{
							temp0 = vmovq_n_u32(s_.m_bits[l]);
							temp1 = vld1q_u32(&bitmap[i].m_bits[l]);
							result = vornq_u32(temp0, temp1);
							isZero = vceqq_u32(zero_, temp0);
							vst1q_u32(res, result);
							vector<int> the_index2 = search(res);
							int u = 0;
							while (!the_index2.empty() && s_.m_bits[l] != 0)
							{
								int x = the_index2[u];
								x *= 32;
								if ((s_.m_bits[x] ^ bitmap[i].m_bits[x]) == 0 && s_.m_bits[x] != 0)
									isExist = true;
								u++;
								the_index2.pop_back();
							}
						}
					}
					n++;
					the_index1.pop_back();
				}
				m++;
				the_index0.pop_back();
			}
			if (isExist == false)
			{
				s_.sec_index[j] = 0;
			}
		}
	}
}
vector<index> idx;
int main() {
	//读取二进制文件
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
		stringstream ss; //输入流
		ss << line; //向流中传值
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
	

	struct timespec sts, ets;
	timespec_get(&sts, TIME_UTC);
	for (int i = 0; i < 100; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		 int* list = new int[num];//例子中的数
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - 按表求交-------- -
		compare(list, idx, num);
		delete list;
	}
	timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec-sts.tv_nsec;
	long dnsec = ets.tv_nsec-sts.tv_nsec;
	printf(" % llu. % 09llus\n", dsec, dnsec);
	return 0;
}
