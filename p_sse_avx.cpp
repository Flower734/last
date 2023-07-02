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
#include<windows.h>
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
bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len;
}
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
	for (int i = 1; i < num; i++) {//num个列表求交
		int count = 0;//s中的删除指针，找到元素，则+1，未找到则erase但由于数组前移，不用+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s列表中的各个值比较
			bool isExit = false;
			for (int t = 0; t < idx[list[i]].len; t++) {//第i个列表的各个值与s列表中的第【j]个值比较
				if (s.arr[count] == idx[list[i]].arr[t]) {
					isExit = true;
					break;
				}
			}
			if (isExit == false) {
				s.len--;
				s.arr.erase(s.arr.begin()+count);//删除第s中第j个元素
			}
			else {
				count++;
			}
		}
	}
	return s;
}
//index& SVS_P_SSE(int* list, vector<index>& idx, int num) {
//	s = idx[list[0]];
//	for (int i = 1; i < num; i++) {//num个列表求交
//		int count = 0;//s中的删除指针，找到元素，则+1，未找到则erase但由于数组前移，不用+1
//		int length = s.len;
//		for (int j = 0; j < length; j++) {//s列表中的各个值比较
//			bool isExit = false;
//			__m128i ss,result;
//			ss = _mm_set1_epi32(s.arr[count]);
//			unsigned int res[4] = { 0 };
//			for (float t = 0; t < idx[list[i]].len; t+=4) {//将s中一个元素同时和list中4个数比较
//				__m128i ii;
//				ii = _mm_set_epi32(idx[list[i]].arr[t], idx[list[i]].arr[t + 1], idx[list[i]].arr[t + 2], idx[list[i]].arr[t + 3]);
//				result = _mm_set1_epi32(0);
//				result = _mm_sub_epi32(ss, ii);
//				int res[4] = { 0 };
//				_mm_store_si128(res, result);
//				for (int i = 0; i < 4; i++) {
//					if (res[i] == 0) {
//						isExit = true;
//						break;
//					}
//				}
//			}
//			if (isExit == false) {
//				s.len--;
//				s.arr.erase(s.arr.begin() + count);//删除第s中第j个元素
//			}
//			else {
//				count++;
//			}
//		}
//	}
//	return s;
//}
index& SVS_P_AVX(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	for (int i = 1; i < num; i++) {//num个列表求交
		int count = 0;//s中的删除指针，找到元素，则+1，未找到则erase但由于数组前移，不用+1
		int length = s.len;
		for (int j = 0; j < length; j++) {//s列表中的各个值比较
			bool isExit = false;
			__m256i ss, result;
			ss = _mm256_set1_epi32(s.arr[count]);
			unsigned int res[8] = { 0 };
			for (float t = 0; t < idx[list[i]].len; t += 8) {//将s中一个元素同时和list中4个数比较
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
				s.arr.erase(s.arr.begin() + count);//删除第s中第j个元素
			}
			else {
				count++;
			}
		}
	}
	return s;
}
//vector<unsigned int>& ADP_P_SSE(int* list, vector<index>& idx, int num)
//{
//	vector<unsigned int> S;
//	int s = 1;
//	bool found = true;
//	vector<index> idx_;
//	for (int i = 0; i < num; i++)
//	{
//		idx_.push_back(idx[list[i]]);
//	}
//	sort(idx_.begin(), idx_.end());
//	/*for (int i = 0; i < num; i++)
//	{
//	 for (int j = 0; j < idx_[i].len; j++)
//	 {
//	  cout << idx_[i].arr[j] << " ";
//	 }
//	 cout << endl;
//	}*/
//	for (int t = 0; t < idx_[0].len; t++)
//	{
//		unsigned int e = idx_[0].arr[t];
//		__m128i qwq;
//		qwq = _mm_set1_epi32(e);
//		while (s != num && found == true)
//		{
//			for (int i = 0; i < idx_[s].len; i+=4)
//			{
//				found = false;
//				__m128i ewe, result;
//				ewe = _mm_loadu_epi32(idx_[s].arr[i], idx_[s].arr[i], idx_[s].arr[i], idx_[s].arr[i]);
//				result = _mm_set1_epi32(0);
//				result = _mm_sub_epi32(ewe, qwq);
//				int res[4] = { 0 };
//				_mm_storeu_si32(res, result);
//				for (int i = 0; i < 4; i++) {
//					if (res[i] == 0) {
//						found = true;
//						break;
//					}
//				}
//			}
//			s = s + 1;
//		}
//		if (s == num && found == true)
//			S.push_back(e);
//	}
//	for (int i = 0; i < S.size(); i++)
//		cout << S.at(i) << endl;
//	return S;
//}
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






int main() {
	//读取二进制文件
	fstream file;
	file.open("D:/ExpIndex_Query/ExpIndex", ios::binary|ios::in);
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
	file.open("D:/ExpIndex_Query/ExpQuery",  ios::in);
	int query[1000][5]={0};
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
	/*-----------------------------------------------------------------------------------------------------------*/
//cout<<"SVS_P_SSE:"<<endl;
//for(int t=5;t<=1000)
//{	//按表求交
//struct timeval start, end;
//	gettimeofday(&start, NULL);
//	for (int i = 0; i < 5; i++) {
//		int num = 0;
//		for (int j = 0; j < 5; j++) {
//			if (query[i][j] != 0) {
//				num++;
//			}
//		}
//		int* list=new int[num];//例子中的数
//		for (int j = 0; j < num; j++) {
//			list[j] = query[i][j];
//		}
//		//------ - 按表求交-------- -
//		sorted(list, idx, num);
//		SVS_P_SSE(list, idx, num);
//
//	}
//	gettimeofday(&end, NULL);
//long double elapsed = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0/n;//ms
//cout<<t<<" "<<"time:"<<elapsed<<" "<<"ms"<<endl;
//
//if(t==5)
//{t=50;continue;}
//if(t==50)
//{
//    t=100;continue;
//}
//if(t==100)
//{
//    t=200;continue;
//}
//if((t==200)||(t==400)||(t==600)||(t==800)||(t==1000))
//{
//
//    t=t+200;
//    continue;
//}
//
//
//}

cout<<"SVS_P_AVX:"<<endl;

for(int t=5;t<=1000;)
{	//按表求交
LARGE_INTEGER start, end;
QueryPerformanceCounter(&start);
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list=new int[num];//例子中的数
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - 按表求交-------- -
		sorted(list, idx, num);
		SVS_P_AVX(list, idx, num);

	}
QueryPerformanceCounter(&end);
LARGE_INTEGER frequency;

QueryPerformanceFrequency(&frequency);


long double elapsed = static_cast<long double>(end.QuadPart - start.QuadPart)*1000/ frequency.QuadPart/t;
cout<<t<<" "<<"time:"<<elapsed<<" "<<"ms"<<endl;

if(t==5)
{t=50;continue;}
if(t==50)
{
    t=100;continue;
}
if(t==100)
{
    t=200;continue;
}
if((t==200)||(t==400)||(t==600)||(t==800)||(t==1000))
{

    t=t+200;
    continue;
}
}

//cout<<"ADP_P_SSE:"<<endl;
//
//for(int t=5;t<=1000)
//{	//按表求交
//struct timeval start, end;
//	gettimeofday(&start, NULL);
//	for (int i = 0; i < 5; i++) {
//		int num = 0;
//		for (int j = 0; j < 5; j++) {
//			if (query[i][j] != 0) {
//				num++;
//			}
//		}
//		int* list=new int[num];//例子中的数
//		for (int j = 0; j < num; j++) {
//			list[j] = query[i][j];
//		}
//		//------ - 按表求交-------- -
//		sorted(list, idx, num);
//		ADP_P_SSE(list, idx, num);
//
//	}
//	gettimeofday(&end, NULL);
//long double elapsed = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0/n;//ms
//cout<<t<<" "<<"time:"<<elapsed<<" "<<"ms"<<endl;
//
//if(t==5)
//{t=50;continue;}
//if(t==50)
//{
//    t=100;continue;
//}
//if(t==100)
//{
//    t=200;continue;
//}
//if((t==200)||(t==400)||(t==600)||(t==800)||(t==1000))
//{
//
//    t=t+200;
//    continue;
//}
//
//
//}


cout<<"ADP_P_AVX:"<<endl;
for(int t=5;t<=1000;)
{	//按表求交
LARGE_INTEGER start, end;
QueryPerformanceCounter(&start);
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list=new int[num];//例子中的数
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - 按表求交-------- -
		sorted(list, idx, num);
		ADP_P_AVX(list, idx, num);

	}
QueryPerformanceCounter(&end);
LARGE_INTEGER frequency;

QueryPerformanceFrequency(&frequency);

long double elapsed = static_cast<long double>(end.QuadPart - start.QuadPart)*1000/ frequency.QuadPart/t;
cout<<t<<" "<<"time:"<<elapsed<<" "<<"ms"<<endl;

if(t==5)
{t=50;continue;}
if(t==50)
{
    t=100;continue;
}
if(t==100)
{
    t=200;continue;
}
if((t==200)||(t==400)||(t==600)||(t==800)||(t==1000))
{

    t=t+200;
    continue;
}


}


	return 0;
}
