#include<iostream>
#include<fstream>
#include<string.h>

using namespace std;


void ppm_load(char* filename, unsigned char*& data, int& w, int& h)
{
	cout << "PPM_LOAD" << endl;
	char header[1024];
	FILE* fp = NULL;
	int line = 0;

	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		return;
	}

	while (line < 2) {
		fgets(header, 1024, fp);
		if (header[0] != '#') {
			++line;
		}
	}

	sscanf(header, "%d %d\n", &w, &h);

	fgets(header, 20, fp);

	data = (unsigned char*)malloc(sizeof(char)*w*h*3);
	memset(data, 0, sizeof(char)*w*h * 3);
	fread(data, w * h * 3, 1, fp);

	fclose(fp);
}

void ppm_save(char* filename, unsigned char* data, int w, int h)
{
	FILE* fp;
	char header[20];

	fp = fopen(filename, "wb");

	fprintf(fp, "P6\n%d %d\n255\n", w, h);

	fwrite(data, w * h * 3, 1, fp);

	fclose(fp);
}
#define MAX(a,b) ((a) > (b) ? (a) : (b))
int main(int argc, char *argv[])
{
	unsigned char* data = NULL;
	int w = 0;
	int h = 0;
//	char* filename = "debug_clusters.pnm";
	char* filename = argv[1];
	ppm_load(filename, data, w, h);
	bool maxFilter = false;
	if (data == NULL)
	{
		return 0;
	}
	unsigned char* result;
	result = (unsigned char*)malloc(sizeof(unsigned char) * w * h * 3);
	memset(result, 0, sizeof(char) * w * h * 3);
	if (maxFilter)
	{
		for (int y = 1; y < h - 1; y++)
		{
			for (int x = 1; x < w - 1; x++)
			{
				for (int d = 0; d < 3; d++)
				{
					unsigned char value = data[d + (x - 1) * 3 + (y - 1) * 3 * w];
					value = MAX(value, data[d + x * 3 + (y - 1) * 3 * w]);
					value = MAX(value, data[d + (x + 1) * 3 + (y - 1) * 3 * w]);
					value = MAX(value, data[d + (x - 1) * 3 + y * 3 * w]);
					value = MAX(value, data[d + x * 3 + y * 3 * w]);
					value = MAX(value, data[d + (x + 1) * 3 + y * 3 * w]);
					value = MAX(value, data[d + (x - 1) * 3 + (y + 1) * 3 * w]);
					value = MAX(value, data[d + x * 3 + (y + 1) * 3 * w]);
					value = MAX(value, data[d + (x + 1) * 3 + (y + 1) * 3 * w]);

					result[d + x * 3 + y * 3 * w] = value;
				}
			}
		}
	}
	else
	{
		for (int y = 1; y < h - 1; y++)
		{
			for (int x = 1; x < w - 1; x++)
			{
				for (int d = 0; d < 3; d++)
				{
					result[d + x * 3 + y * 3 * w] = (data[d + (x - 1) * 3 + (y - 1) * 3 * w] + data[d + x * 3 + (y - 1) * 3 * w] + data[d + (x + 1) * 3 + (y - 1) * 3 * w] + \
						data[d + (x - 1) * 3 + y * 3 * w] + data[d + x * 3 + y * 3 * w] + data[d + (x + 1) * 3 + y * 3 * w] + \
						data[d + (x - 1) * 3 + (y + 1) * 3 * w] + data[d + x * 3 + (y + 1) * 3 * w] + data[d + (x + 1) * 3 + (y + 1) * 3 * w]) / 9;
				}
			}
		}
	}
	ppm_save("debug_clusters2.pnm", result, w, h);
	free(data);
	free(result);
	return 0;
}
