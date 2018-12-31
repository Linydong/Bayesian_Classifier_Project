
#include <iostream>   
#include <fstream> 
#include "string.h" 
#include "math.h"

using namespace std;

class record {
public:
	float a;                //一条数据所包含的4个参数   
	float b;
	float c;
	float d;
	char attr[30];          //该条记录的真实类别   
	char clas[30];          //该条记录的Bayes分类结果   
};

record DATA[200];           //存储每条记录的数据   
const int row = 50;         //抽取50条数据作为训练数据集   
const int total = 150;      //共有150条数据   
const char str1[30] = "Iris-setosa";
const char str2[30] = "Iris-versicolor";
const char str3[30] = "Iris-virginica";
const float PI = 3.1415926;
float _xa1, _xb1, _xc1, _xd1, sa1, sb1, sc1, sd1;       //_x是求得的平均值，s是求得的标准差   
float _xa2, _xb2, _xc2, _xd2, sa2, sb2, sc2, sd2;
float _xa3, _xb3, _xc3, _xd3, sa3, sb3, sc3, sd3;

void input(const char* str, int row_num) {
	ifstream inp;
	inp.open(str);

	if (inp) {
		int rows = row_num;
		while (!inp.eof()) {
			inp >> DATA[rows].a;
			inp.seekg(1, ios::cur); //seekg设置输入文件流的输入指针位置；

			inp >> DATA[rows].b;
			inp.seekg(1, ios::cur);

			inp >> DATA[rows].c;
			inp.seekg(1, ios::cur);

			inp >> DATA[rows].d;
			inp.seekg(1, ios::cur);

			inp >> DATA[rows].attr;

			//cout << "b: " << DATA[rows].b << endl;

			rows++;
		}
	}
	else {
		cout << "文件未找到！" << endl;
		return;
	}

	inp.close();
}

float Gauss(float x, float _x, float s) {
	float k = 1 / (sqrt(2 * PI)*s);
	float l = -(x - _x)*(x - _x) / (2 * s*s);  //连续属性可考虑概率密度函数 
	return k * exp(l);
}

float compute_s(const char str[30], char ss, float _x) {
	int n = 0, num = 0;
	float sum = 0;
	if (ss == 'a') {
		while (n != row) {
			if (strcmp(DATA[n].attr, str))  // 比较参数1和参数（1、若参数1>参数2，返回正数；2、若参数1<参数2，返回负数；3、若参数1=参数2，返回0；）
				n++;
			else {
				sum += (DATA[n].a - _x)*(DATA[n].a - _x);
				num++;
				n++;
			}
		}
		return sum / (num - 1);
	}
	else if (ss == 'b') {
		while (n != row) {
			if (strcmp(DATA[n].attr, str))
				n++;
			else {
				sum += (DATA[n].b - _x)*(DATA[n].b - _x);
				num++;
				n++;
			}
		}
		return sum / (num - 1);
	}
	else if (ss == 'c') {
		while (n != row) {
			if (strcmp(DATA[n].attr, str))
				n++;
			else {
				sum += (DATA[n].c - _x)*(DATA[n].c - _x);
				num++;
				n++;
			}
		}
		return sum / (num - 1);
	}
	else if (ss == 'd') {
		while (n != row) {
			if (strcmp(DATA[n].attr, str))
				n++;
			else {
				sum += (DATA[n].d - _x)*(DATA[n].d - _x);
				num++;
				n++;
			}
		}
		return sum / (num - 1);
	}

}

void preprepare() {
	//char filename[15];  
	//strcpy(filename, "Iris训练数据集.txt");
	input("Iristraindata.txt", 0);                      //读入训练数据集   
	int n = 0, num = 0;
	float sum_a = 0, sum_b = 0, sum_c = 0, sum_d = 0;
	while (n != row && !strcmp(DATA[n].attr, str1)) {    //计算得到12个_x和s   
		sum_a += DATA[n].a;
		sum_b += DATA[n].b;
		sum_c += DATA[n].c;
		sum_d += DATA[n].d;
		num++;
		n++;
	}
	_xa1 = sum_a / num;
	_xb1 = sum_b / num;
	_xc1 = sum_c / num;
	_xd1 = sum_d / num;
	sa1 = compute_s(str1, 'a', _xa1);
	sb1 = compute_s(str1, 'b', _xb1);
	sc1 = compute_s(str1, 'c', _xc1);
	sd1 = compute_s(str1, 'd', _xd1);

	num = 0, sum_a = 0, sum_b = 0, sum_c = 0, sum_d = 0;
	while (n != row && !strcmp(DATA[n].attr, str2)) {
		sum_a += DATA[n].a;
		sum_b += DATA[n].b;
		sum_c += DATA[n].c;
		sum_d += DATA[n].d;
		num++;
		n++;
	}
	_xa2 = sum_a / num;
	_xb2 = sum_b / num;
	_xc2 = sum_c / num;
	_xd2 = sum_d / num;
	sa2 = compute_s(str2, 'a', _xa2);
	sb2 = compute_s(str2, 'b', _xb2);
	sc2 = compute_s(str2, 'c', _xc2);
	sd2 = compute_s(str2, 'd', _xd2);

	num = 0, sum_a = 0, sum_b = 0, sum_c = 0, sum_d = 0;
	while (n != row && !strcmp(DATA[n].attr, str3)) {
		sum_a += DATA[n].a;
		sum_b += DATA[n].b;
		sum_c += DATA[n].c;
		sum_d += DATA[n].d;
		num++;
		n++;
	}
	_xa3 = sum_a / num;
	_xb3 = sum_b / num;
	_xc3 = sum_c / num;
	_xd3 = sum_d / num;
	sa3 = compute_s(str3, 'a', _xa3);
	sb3 = compute_s(str3, 'b', _xb3);
	sc3 = compute_s(str3, 'c', _xc3);
	sd3 = compute_s(str3, 'd', _xd3);
}

//从文件名为 str 的文件中读取记录，存入data[row_num]往后的数组中   
float Gauss(float x, float _x, float s);                //高斯分布公式   
float compute_s(const char str[30], char ss, float _x); //计算s   
void preprepare();                                      //处理训练数据，得到分类模型   

int main()
{
	preprepare();
	//char filename1[15];  
	//strcpy(filename1, "Iris检测数据集.txt");                                    //预处理   
	input("Iristestdata.txt", row);
	int n = row;
	while (n != total) {
		float a_1, b_1, c_1, d_1;
		float a_2, b_2, c_2, d_2;
		float a_3, b_3, c_3, d_3;
		a_1 = Gauss(DATA[n].a, _xa1, sa1);              //使用高斯分布公式，得到条件概率   
		b_1 = Gauss(DATA[n].b, _xb1, sb1);
		c_1 = Gauss(DATA[n].c, _xc1, sc1);
		d_1 = Gauss(DATA[n].d, _xd1, sd1);

		a_2 = Gauss(DATA[n].a, _xa2, sa2);
		b_2 = Gauss(DATA[n].b, _xb2, sb2);
		c_2 = Gauss(DATA[n].c, _xc2, sc2);
		d_2 = Gauss(DATA[n].d, _xd2, sd2);

		a_3 = Gauss(DATA[n].a, _xa3, sa3);
		b_3 = Gauss(DATA[n].b, _xb3, sb3);
		c_3 = Gauss(DATA[n].c, _xc3, sc3);
		d_3 = Gauss(DATA[n].d, _xd3, sd3);

		double p1, p2, p3;
		p1 = a_1 * b_1*c_1*d_1;
		p2 = a_2 * b_2*c_2*d_2;
		p3 = a_3 * b_3*c_3*d_3;
		if (p1 >= p2 && p1 >= p3) {                        //条件概率较大的属性确定为该条记录的属性   
			for (int i = 0; i < 30; i++)
				DATA[n].clas[i] = str1[i];
		}
		else if (p2 > p1 && p2 >= p3) {
			for (int i = 0; i < 30; i++)
				DATA[n].clas[i] = str2[i];
		}
		else if (p3 > p1 && p3 > p2) {
			for (int i = 0; i < 30; i++)
				DATA[n].clas[i] = str3[i];
		}
		n++;
	}

	int error = 0;
	for (int i = row; i < total; i++) {
		cout << DATA[i].a << "\t" << DATA[i].b << "\t" << DATA[i].c << "\t" << DATA[i].d << "\t" << DATA[i].clas;
		if (strcmp(DATA[i].clas, DATA[i].attr)) {
			error++;
			cout << "\t 预测错误!";
		}
		cout << endl;
	}
	cout << "总共测试文本量：" << (total - row) << endl;
	cout << "预测正确类别量：" << (total - row - error) << endl;
	cout << "NavieBayes分类器正确率为：" << float((total - row - error) * 100) / float(total - row) << " %" << endl;

	system("pause");
}

