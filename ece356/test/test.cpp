#include <cstdio>

extern "C" {
	void add(int &result, int left, int fight);

}

int main() {
	int result;
	add(result, 20, 30);

	printf("Result of 20 + 30 = %\n", result);

	return 0;
}
