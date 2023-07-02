#include <iostream>
#include <stdlib.h>
#include <arm_neon.h>
#include <fstream>
#include<cstdlib>
#include<ctime>
using namespace std;
//D1逆变换 32×4寄存器

void D1_4(int* start, int len)
{
    int32x4_t t1, t2, t3;
    t2 = vld1q_dup_s32(0);
    t3 = vld1q_dup_s32(0);
    int res = len % 4;
    //前四个文档doc
    t1 = vld1q_s32(start);
    t2[2] = t1[0];
    t2[3] = t1[1];

    t2 = vaddq_s32(t1, t2);
    t3[1] = t2[0];
    t3[2] = t2[1];
    t3[4] = t2[2];
    t1 = vaddq_s32(t2, t3);

    vst1q_s32(start, t1);

    for (int i = 4; i < len - res; i += 4)
    {
        t1 = vld1q_s32(start + i);
        t2 = vshlq_n_s32(t1, 31);
        t2 = vshlq_n_s32(t2, 31);
        t2 = vaddq_s32(t1, t2);
        t1 = vshlq_n_s32(t2, 31);
        t1 = vaddq_s32(t1, t2);
        t2 = vdupq_n_s32(*(start + i - 1));
        t1 = vaddq_s32(t1, t2);

        vst1q_s32(start + i, t1);
    }
    //串行掐尾
    for (int i = len - res; i < len; i++)
    {
        start[i] += start[i - 1];
    }
}
int main() {
    ifstream in("gov.bin", ios::binary);
    ofstream out("out.bin", ios::binary);
    clock_t startClock, endClock;
    //double time = 0.0;
    startClock = clock();
    in.seekg(3 * sizeof(int));
    
    int docId = 0;
    int id = 0;
    while (!in.eof()) {
        int last = 0;
        while (docId == id) {
            int32x4_t t1, t2, end;
            end = vdupq_n_s32(last);
            int num[4];
            int temp[4];
            for (int i = 0; i < 4; i++) {
                in.read((char*)&(num[i]), sizeof(int));
                if (num[i] >> 8 != docId) {
                    docId++;
                    break;
                }
                num[i] &= 255;
            }
            
            t1 = vld1q_s32(num);
            t2 = vld1q_lane_s32(num, t2, 2);
            t2 = vld1q_lane_s32(num+1, t2, 3);
            t2 = vaddq_s32(t1, t2);

            temp[0] = 0;
            temp[1] = vgetq_lane_s32(t2, 0);
            temp[2] = vgetq_lane_s32(t2, 1);
            temp[3] = vgetq_lane_s32(t2, 2);
            t1 = vld1q_s32(temp);
            t1 = vaddq_s32(t1, t2);
            t1 = vaddq_s32(t1, end);
            vst1q_s32(num, t1);
            last = temp[3];
            
            
            for (int i = 0; i < 4; i++) {
                out.write((char*)&(num[i]), sizeof(int));
            }
        }
        id++;

    }

    endClock = clock();
    //end = clock();
    double time = (double)(endClock - startClock) / CLOCKS_PER_SEC;
    in.close();
    out.close();
    cout << "Total time:" << time << "s" << endl;
    system("pause");
	return 0;
}