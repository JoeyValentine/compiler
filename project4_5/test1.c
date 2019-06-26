void mm(int from, int to)
{
	output(from);
	output(to);
}

void hanoi(int n, int from, int by, int to)
{
	if (n == 1)
		mm(from, to);
	else
	{
		
		hanoi(n-1, from, to, by);
		mm(from, to);
		hanoi(n-1, by, from, to);
	}
}

void main(void)
{
	int n;
	n = input();
	hanoi(n, 1, 2, 3);
}
