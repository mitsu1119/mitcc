int main() {
	int a = 0;
	int b = 0;
	while(a < 45) {
		b = fibonacci(a, 1, 0);
		print(b);
		a = a + 1;
	}
	return 0;
}

int fibonacci(int n, int a, int b) {
	// fibonacci sequence
	/*
	 * F(0) = F(1) = 1.
	 * F(n) = F(n - 1) + F(n - 2), n > 2.
	 */
	if(n == 0) return 0;
	if(n == 1) return a;
	return fibonacci(n - 1, a+b, a);
}
