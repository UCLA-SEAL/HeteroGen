#include <hls_stream.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void top(hls::stream<int> &in, hls::stream<int> &out);

int main(int argc, char** argv) {

	int n;
	FILE* f = fopen(argv[1], "r");
	fscanf(f, "%d\n", &n);
	printf("%d\n", n);
	fclose(f);

	int i = 0, errors = 0;
	static hls::stream<int> in, out;

	in.write(n);

	top(in, out);

	int kernel_output = out.read();
	printf("input: %d\n", n);
	printf("output: %d\n", kernel_output);

	if(kernel_output != n+1)
		errors++;

	if(errors)
		cout << "FAIL!" << endl;
	else
		cout << "Success!" << endl;

	return 0;
}

