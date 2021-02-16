
#include <iostream>
#include <windows.h>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <cstdlib>

typedef void(*API_pointer)(unsigned int height, unsigned int width, unsigned int color_depth, unsigned char* buffer, unsigned char* filename);

HINSTANCE hInst;

using namespace std;

struct bmpheader
{
	unsigned short int width;
	unsigned short int height;
	unsigned int color_depth;
}bmph;
int k2, j = 0;
string filename;
clock_t start, ends;
int main(int argc, char* argv[])
{
	hInst = LoadLibrary(L"libcbg.dll");
	API_pointer  cbg1_encToFile_default = (API_pointer)GetProcAddress(hInst, "cbg1_encToFile_default");
	if (argc == 1)
	{
		cout << "未指定文件，请拖放一个bmp文件来编码" << endl;

	}
	for (int kk = 1; kk < argc; kk++)
	{
		filename = argv[kk];
		ifstream fin(filename.c_str(), ios::binary);
		cout << "文件：" << filename.c_str() << "开始编码，";
		char temp = 0;
		bmph.height = 0;
		bmph.width = 0;
		bmph.color_depth = 0;
		//The cbg image ONLY support 0xffff (2bytes) max width and height!!
		//fin.get(temp);
		for (int i = 0; i < 2; i++)
		{
			fin.seekg(0x12 + i);
			temp = fin.get();
			//write the image width
		//	fout.put(temp);
			unsigned int temp2 = (unsigned char)temp;
			temp2 <<= 8 * i;
			bmph.width += temp2;
		}
		if (fin.get())
		{
			cout << "Error. The cbg image only support 0xffff (2bytes) max width and height!!" << endl;
			getchar();
			exit(0);
		}
		for (int i = 0; i < 2; i++)
		{
			fin.seekg(0x16 + i);
			temp = fin.get();
			//write the image height
		//	fout.put(temp);
			unsigned int temp2 = (unsigned char)temp;
			temp2 <<= 8 * i;
			bmph.height += temp2;
		}
		if (fin.get())
		{
			cout << "Error. The cbg image only support 0xffff (2bytes) max width and height!!" << endl;
			getchar();
			exit(0);
		}
		//cout << bmph.width << " " << bmph.height << endl;
		fin.seekg(0x1c);
		temp = fin.get();
		//write the image color depth
		unsigned int temp2 = (unsigned char)temp;
		bmph.color_depth = temp2;
		//	cout << bmph.color_depth << endl;
		fin.seekg(0x36);

		cout << "图像高度：" << bmph.height << "，图像宽度：" << bmph.width << "，图像色彩深度：" << bmph.color_depth << endl;
		clock_t start = clock();
		unsigned char* raw_pixel_buffer = new unsigned char[bmph.height * bmph.width * (bmph.color_depth / 8)];
		for (long long i = bmph.height - 1; i >= 0; i--)
		{
			for (long long j = 0; j < bmph.width; j++)
			{
				for (short k = 0; k < bmph.color_depth / 8; k++)
				{
					unsigned int cur = i * bmph.width * (bmph.color_depth / 8) + j * (bmph.color_depth / 8) + k;
					raw_pixel_buffer[cur] = (unsigned char)fin.get();
				}
			}
		}
		fin.close();
		unsigned char* cbgname = new unsigned char[filename.length()]();
		bool ok = 0;
		j = 0;
		for (k2 = filename.length() - 1, j = 0; k2 >= 0; k2--)
		{
			if (filename[k2] == '.')
			{
				ok = true;
				continue;
			}
			if (ok == true)
			{
				cbgname[j] = filename[k2];
				j++;
			}

		}
		unsigned char* temp22 = new unsigned char[filename.length()]();
		j = 0;
		for (k2 = filename.length() - 5; k2 >= 0; k2--)
		{
			temp22[k2] = cbgname[j];
			j++;
		}
		delete[] cbgname;
		cbg1_encToFile_default(bmph.height, bmph.width, bmph.color_depth, raw_pixel_buffer, temp22);
		clock_t ends = clock();
		int times = ends - start;
		cout << "编码完成，输出文件名：";
		cout << temp22;
		delete[] temp22;
		cout << "，耗时：" << times << "ms" << endl << endl;
	}
	cout << "所有工作已完成，请按任意键退出..." << endl;
	getchar();
}
