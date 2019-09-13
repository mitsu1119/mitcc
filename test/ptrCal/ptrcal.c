int main() {
	int *p;
	int *q;

	allocs(&p, 1, 2, 3, 4);
	q = p + 2;

	return *q;
}

