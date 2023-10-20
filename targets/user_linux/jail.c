int array_local[1000];
int jail() {
	for (int i =0; i < sizeof(array_local)/sizeof(int); i++) {
			if (array_local[i])
					array_local[i] = 0;
			else
					array_local[i] = i;
	}

	return 0;
}
