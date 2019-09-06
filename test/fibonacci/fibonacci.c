int main() {
	int a;
	int b;
	a = 0;
	b = 0;
	while(a < 20) {
		b = fibonacci(a);
		print(b);
		a = a + 1;
	}
	return 0;
}

int fibonacci(int n) {
	if(n == 0) return 1;
	if(n == 1) return 1;
	return fibonacci(n - 1) + fibonacci(n - 2);
}
