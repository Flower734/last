#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>
#include <string>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>
#define MAXSIZE 2000
using namespace std;
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

void sorted(int* list, vector<index>& idx,int num) {
	for (int i = 0; i < num-1; i++) {
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
index& SVS(int* list,vector<index> &idx,int num) {
	 s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s�б��еĸ���ֵ�Ƚ�
			bool isExit = false;
			for (int t = 0; t < idx[list[i]].len; t++) {//��i���б�ĸ���ֵ��s�б��еĵڡ�j]��ֵ�Ƚ�
				if (s.arr[count] == idx[list[i]].arr[t]) {
					isExit = true;
					break;
				}
			}
			if (isExit == false) {
				s.len--;
				s.arr.erase(s.arr.begin()+count);//ɾ����s�е�j��Ԫ��
			}
			else {
				count++;
			}
		}
	}
	return s;
}
index& SVS_P_SSE(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s�б��еĸ���ֵ�Ƚ�
			bool isExit = false;
			__m128i ss,result;
			ss = _mm_set1_epi32(s.arr[count]);
			unsigned int res[4] = { 0 };
			for (float t = 0; t < idx[list[i]].len; t+=4) {//��s��һ��Ԫ��ͬʱ��list��4�����Ƚ�
				__m128i ii;
				ii = _mm_set_epi32(idx[list[i]].arr[t], idx[list[i]].arr[t + 1], idx[list[i]].arr[t + 2], idx[list[i]].arr[t + 3]);
				result = _mm_set1_epi32(0);
				result = _mm_sub_epi32(ss, ii);
				int res[4] = { 0 };
				_mm_store_si128(res, result);
				for (int i = 0; i < 4; i++) {
					if (res[i] == 0) {
						isExit = true;
						break;
					}
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
index& SVS_P_AVX(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s�б��еĸ���ֵ�Ƚ�
			bool isExit = false;
			__m256i ss, result;
			ss = _mm256_set1_epi32(s.arr[count]);
			unsigned int res[8] = { 0 };
			for (float t = 0; t < idx[list[i]].len; t += 8) {//��s��һ��Ԫ��ͬʱ��list��4�����Ƚ�
				__m256i ii;
				ii = _mm256_set_epi32(idx[list[i]].arr[t], idx[list[i]].arr[t + 1], idx[list[i]].arr[t + 2], idx[list[i]].arr[t + 3], idx[list[i]].arr[t + 4], idx[list[i]].arr[t + 5], idx[list[i]].arr[t + 6], idx[list[i]].arr[t + 7]);
				result = _mm256_set1_epi32(0);
				result = _mm256_sub_epi32(ss, ii);
				_mm256_storeu_epi32(res, result);
				for (int i = 0; i < 8; i++) {
					if (res[i] == 0) {
						isExit = true;
						break;
					}
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
vector<unsigned int>& ADP_P_SSE(int* list, vector<index>& idx, int num)
{
	vector<unsigned int> S;
	int s = 1;
	bool found = true;
	vector<index> idx_;
	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	sort(idx_.begin(), idx_.end());
	/*for (int i = 0; i < num; i++)
	{
	 for (int j = 0; j < idx_[i].len; j++)
	 {
	  cout << idx_[i].arr[j] << " ";
	 }
	 cout << endl;
	}*/
	for (int t = 0; t < idx_[0].len; t++)
	{
		unsigned int e = idx_[0].arr[t];
		__m128i qwq;
		qwq = _mm_set1_epi32(e);
		while (s != num && found == true)
		{
			for (int i = 0; i < idx_[s].len; i+=4)
			{
				found = false;
				__m128i ewe, result;
				ewe = _mm_loadu_epi32(idx_[s].arr[i], idx_[s].arr[i], idx_[s].arr[i], idx_[s].arr[i]);
				result = _mm_set1_epi32(0);
				result = _mm_sub_epi32(ewe, qwq);
				int res[4] = { 0 };
				_mm_storeu_si32(res, result);
				for (int i = 0; i < 4; i++) {
					if (res[i] == 0) {
						found = true;
						break;
					}
				}
			}
			s = s + 1;
		}
		if (s == num && found == true)
			S.push_back(e);
	}
	for (int i = 0; i < S.size(); i++)
		cout << S.at(i) << endl;
	return S;
}
vector<unsigned int>& ADP_P_AVX(int* list, vector<index>& idx, int num)
{
	vector<unsigned int> S;
	int s = 1;
	bool found = true;
	vector<index> idx_;
	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	sort(idx_.begin(), idx_.end());
	/*for (int i = 0; i < num; i++)
	{
	 for (int j = 0; j < idx_[i].len; j++)
	 {
	  cout << idx_[i].arr[j] << " ";
	 }
	 cout << endl;
	}*/
	for (int t = 0; t < idx_[0].len; t++)
	{
		unsigned int e = idx_[0].arr[t];
		__m256i qwq;
		qwq = _mm256_set1_epi32(e);
		while (s != num && found == true)
		{
			for (int i = 0; i < idx_[s].len; i++)
			{
				found = false;
				__m256i ewe, result;
				ewe = _mm256_set_epi32(idx_[s].arr[i], idx_[s].arr[i + 1], idx_[s].arr[i + 2], idx_[s].arr[i + 3], idx_[s].arr[i + 4], idx_[s].arr[i + 5], idx_[s].arr[i + 6], idx_[s].arr[i + 7]);
				result = _mm256_set1_epi32(0);
				result = _mm256_sub_epi32(ewe, qwq);
				unsigned int res[8] = { 0 };
				_mm256_storeu_epi32(res, result);
				for (int i = 0; i < 8; i++) {
					if (res[i] == 0) {
						found = true;
						break;
					}
				}
			}
			s = s + 1;
		}
		if (s == num && found == true)
			S.push_back(e);
	}
	for (int i = 0; i < S.size(); i++)
		cout << S.at(i) << endl;
	return S;
}
vector<unsigned int>& ADP(int* list, vector<index>& idx, int num)
{
	vector<unsigned int> S;
	int s = 1;
	bool found = true;
	vector<index> idx_;
	for (int i = 0; i < num; i++)
	{
		idx_.push_back(idx[list[i]]);
	}
	for (int t = 0; t < idx_[0].len; t++)
	{
		unsigned int e = idx_[0].arr[t];
		while (s != num && found == true)
		{
			for (int i = 0; i < idx_[s].len; i++)
			{
				found = false;
				if (e == idx_[s].arr[i])
				{
					found = true;
					break;
				}
			}
			s = s + 1;
		}
		if (s == num && found == true)
			S.push_back(e);
	}
	return S;
}
index& Zipper(int* list, vector<index>& idx,int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {
		temp.len = 0;
		temp.arr.clear();
		int p1 = 0, p2 = 0;
		int length = s.len; int count = 0;
		while (count<length && p2 < idx[list[i]].len) {//��ָ��δָ������ĩβʱ
			if (s.arr[p1] == idx[list[i]].arr[p2]){
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
Hash_List** Ԥ����(vector<index> idx,Hash_List**HL) {
	//h(x)=[x/2**16];��������idx�еı��������������ϣ��
	unsigned int p = 0;
	for (int i = 0; i < MAXSIZE; i++) {//��ÿ��indexԤ����
		for (int j = 0; j <idx[i].len; j++) {
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
index& Hash_P_SSE16(int* list, vector<index>& idx, int num) {//���ڹ�ϣ����ʵ��
	//���л����ڱȽ������б�ʱ��ʹ�ò��к���
	//����SSE��PCMPESTRIֻ�ܱȽ�16λ���������õ�hash����ǰ16λ��ͬ������£�ȡ��16λ�Ƚ�
	s = idx[list[0]];
	int th = list[0];
	for (int i = 1; i < num; i++) {//num���б���
		int count = 0;//s�е�ɾ��ָ�룬�ҵ�Ԫ�أ���+1��δ�ҵ���erase����������ǰ�ƣ�����+1
		int length = s.len;
		while (count < 65536 ) {
			if (HL[th][count].beg==-1) {//��������Χ������
				count++;
				continue;
			}
			if (HL[list[i]][count].beg == -1) {//��ĳ����Χ�ڣ�s����������list[i]��������������ɾ��s�е���
				s.arr.erase(s.arr.begin() + HL[th][count].beg, s.arr.begin() + HL[th][count].end + 1);
				HL[th][count].beg = HL[th][count].end = -1;
				count++;
				continue;
			}
			//ĳ����Χ�������б���ֵ
			//�Ƚ������б�����һ��Χ��32λ��ȡ16��λ
			int l1 = HL[th][count].end - HL[th][count].beg + 1;
			l1 = ceil(l1 / 8)*8;
			int l2 = HL[i][count].end - HL[i][count].beg+1;
			l1 = ceil(l2 / 8)*8;
			int L = l1 - l2 > 0 ? l1 : l2;
			short* q1 = new short[L];
			short* q2 = new short[L];
			short s1, s2;//����2�ֽ�
			
			for (int k = 0; k < L ; k++) {
				s1 = idx[th].arr[HL[th][count].beg + k]& 0x0000FFFF;
				q1[k] = s1;
				s2 = idx[i].arr[HL[i][count].beg + k] & 0x0000FFFF;
				q1[k] = s2;
			}
			//��q1�д���s��ĳһ��Χ�����ĺ�16λ
			__m128i mm0, mm1;
			bool isEx = false;
			/*for (short z = 0; z < L; z += 8) {

				mm0 = _mm_loadu_epi16(q1 + z);
				mm1 = _mm_loadu_epi16(q2 + z);
				//int rt=_mm_cmpestri(mm0,2,mm1,2, _SIDD_CMP_EQUAL_ANY);
				_asm {
					pcmpestri mm0, mm1, _SIDD_BIT_MASK;
				}
			}*/
			for (int f =0; f <l1; f++) {
				__m128i qwq;
				qwq = _mm_set1_epi16(q1[f]);
				bool isFound = false;
				for (int h = 0; h < l2; h++) {
					__m128i ewe, result;
					ewe = _mm_set_epi16(q2[i], q2[i + 1], q2[i + 2], q2[i + 3], q2[i+4], q2[i+5], q2[i+6], q2[i+7]);
					result = _mm_set1_epi16(0);
					result = _mm_sub_epi16(ewe, qwq);
					unsigned int res[8] ;
					_mm_storeu_epi32(res, result);
					for (int i = 0; i < 8; i++) {
						if (res[i] == 0) {
							isFound = true;
							break;
						}
					}
					if (isFound == true) {
						break;
					}
				}
				if (isFound == false) {
					s.arr.erase(s.arr.begin() + HL[th][count].beg+f);
					continue;
				}
				else {
					continue;
				}
			}
			
		}
	}
	return s;
}
index& Hash_S(int* list, vector<index>& idx, int num) {//���ڹ�ϣ��Ĵ���ʵ��
	
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
				for (t; t <= t1; t++) {//��i���б�ĸ���ֵ��s�б��еĵڡ�j]��ֵ�Ƚ�
					if (s.arr[count] == idx[list[i]].arr[t]) {
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

	void set(int data)
	{
		int index0 = data / 32; //��������������
		int index1 = index0 / 1024;
		int index2 = index1 / 1024;
		int temp0 = data % 32; //����λ������
		int temp1 = temp0 / 32;
		int temp2 = temp1 / 32;

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
	vector<int> m_bits;
	vector<int> fir_index;
	vector<int> sec_index;
};
BitMap s_(30000000);
void compare(int* list, vector<index>& idx, int num)
{
	vector<index> idx_;
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
	for (int i = 1; i < num; i++)
	{
		for (int j = 0; j < bitmap[0].sec_index.size(); j++)
		{
			if ((s_.sec_index[j] ^ bitmap[i].sec_index[j]) == 0 && s_.sec_index[j] != 0)//��λ���
			{
				for (int t = j * 32; t < j * 32 + 32; t++)
				{
					if ((s_.fir_index[t] ^ bitmap[i].sec_index[t]) == 0 && s_.fir_index[t] != 0)
					{
						for (int l = t * 32; l < t * 32 + 32; l++)
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
void compare(int* list, vector<index>& idx, int num)
{
	vector<index> idx_;
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
	for (int i = 1; i < num; i++)
	{
		for (int j = 0; j < bitmap[0].sec_index.size(); j++)
		{
			if ((s_.sec_index[j] ^ bitmap[i].sec_index[j]) == 0 && s_.sec_index[j] != 0)//��λ���
			{
				for (int t = j * 32; t < j * 32 + 32; t+=4)
				{
					__m128i sf, bs, result;
					sf = _mm_set_epi32(s_.fir_index[t], s_.fir_index[t + 1], s_.fir_index[t + 2], s_.fir_index[t + 3]);
					bs = _mm_set_epi32(bitmap[i].sec_index[t], bitmap[i].sec_index[t + 1], bitmap[i].sec_index[t + 2], bitmap[i].sec_index[t + 3]);
					result = _mm_xor_epi32(sf, bs);
					int res[4];
					_mm_storeu_epi32(res,result);
					if ((s_.fir_index[t] ^ bitmap[i].sec_index[t]) == 0 && s_.fir_index[t] != 0)
					{
						for (int l = t * 32; l < t * 32 + 32; l++)
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
	file.open("ExpIndex", ios::binary|ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}
	vector<index> idx;
	for (int i=0; i <2000; i++) {
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
	file.open("ExpQuery",  ios::in);
	int query[1000][5]={0};
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
	/*-----------------------------------------------------------------------------------------------------------*/

	//������
	clock_t start, end;
	start = clock();
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list=new int[num];//�����е���
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - ������-------- -
		sorted(list, idx, num);
		SVS_P_SSE(list, idx, num);

	}
	end = clock();
	cout << "SVS_sse���ĵ��¼���" << (end-start)/5 << endl;
	start = clock();
	
	/*HL = new Hash_List * [MAXSIZE];
	for(int i=0;i<MAXSIZE;i++){
	HL[i]=new Hash_List[65536];
	}
	HL = Ԥ����(idx, HL);
	start = clock();
	for (int i = 0; i < 1000; i++) {
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
		/*-------hash�Ż���---------
		
		sorted(list, idx, num);
		s = Hash_S(list, idx, num);

	}
	end = clock();
	cout << "��ϣ�Ż����ĵ��¼���" << start - end << endl;*/
	
//	
//	for (int i = 0; i < MAXSIZE; i++) {
//		HL[i] = new Hash_List[65536];
//	}
	//���б���С����˳������
	//
/*	int num = sizeof(list) / sizeof(int);
	
	s = Hash_S(list,idx,num);
	cout << s.len << endl;*/
	/*for (int i = 0; i < s.len; i++) {
		cout << s.arr[i] << ' ';
	}*/
	return 0;
}