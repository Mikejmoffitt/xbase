#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *f = fopen("SAMPLE.TXT", "r");
	if (!f) return -1;

	while (!feof(f)) putchar(fgetc(f));
	fclose(f);
	return 0;
}
