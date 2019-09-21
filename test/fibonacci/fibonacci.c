int main() {
	char a = 0;
	int b = 0;
	while(a < 100) {
		b = fibonacci(a);
		print(b);
		a = a + 1;
	}
	return 0;
}

int fibonacci(char n) {
	// fibonacci sequence
	/*
	 * F(0) = F(1) = 1.
	 * F(n) = F(n - 1) + F(n - 2), n > 2.
	 */
	if(n == 0) return 1;
	if(n == 1) return 1;
	return fibonacci(n - 1) + fibonacci(n - 2);
}
