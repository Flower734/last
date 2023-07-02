#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
//#include <windows.h>
#include <sys/time.h>
using namespace std;

struct index {
    int len;               // 长度字段，表示数组的长度
    std::vector<unsigned int> arr; // 存储二进制数据的容器，使用std::vector存储unsigned int类型的数据
};


index& SVS(int* list,vector<index> &idx,int num) {
index s = idx[list[0]];
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

/*---------------------------------------------------------------------------------------------------------*/
int main()
{
//读取二进制文件
	fstream file;
	file.open("/home/sTest/data/search_engine/ExpIndex", ios::binary|ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}
	cout<<"infile"<<endl;
	vector<index> idx;//存在idx里
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
	file.open("/home/sTest/data/search_engine/ExpQuery",  ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}
	cout<<"infile"<<endl;
	int query[1000][5]={0};//放在query数组中
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
/*---------------------------------------------------------------------------------------------------------*/
struct timeval start, end;
	for(int t=5;t<=1000;)
  {
//LARGE_INTEGER start, end;
//QueryPerformanceCounter(&start);
gettimeofday(&start, NULL);
   for (int i = 0; i < t; i++) {
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


		// 按表求交
		//SVS(list,idx,num);
		//按元素求交
		ADP(list,idx,num);

	}
	//QueryPerformanceCounter(&end);
//LARGE_INTEGER frequency;

//QueryPerformanceFrequency(&frequency);
// 计算时间间隔（单位：秒）
//long double elapsed = static_cast<long double>(end.QuadPart - start.QuadPart)*1000/ frequency.QuadPart/t;
gettimeofday(&end, NULL);
long double elapsed = ((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec)) / 1000.0;//ms
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
