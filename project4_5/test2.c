int tree[2000];
int N;

void insert(int value){
	int i;
	i = 1;
	while(tree[i]){
		if(tree[i] > value)
			i = 2 * i;
		else
			i = 2 * i + 1;
	}
	tree[i] = value;
}

void inOrder(int i){
	if(tree[i] != 0){
		inOrder(i*2);
		output(tree[i]);
		inOrder(i*2 + 1);
	}
}

void main(void){
	
	N = input();
	while(N){
		insert(input());
		N = N - 1;
	}
	
	inOrder(1);
}
