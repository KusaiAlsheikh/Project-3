#include <stdbool.h>//this allowed me to use bool variables in login
#include <stdio.h>//this is a standard library 
#include <string.h>//the command strcmp is taken from here
#include <stdlib.h>
#include <ctype.h>

#pragma warning  (disable : 4996)
int c_count = 0;
float Matrix[50][50];
char var_list[50] = "";
int r_count = 0;

void put_coef_into_matrix(char var_name, int row, float coef) {
	if (var_name == 0) {
		Matrix[row][c_count - 1] = coef;
		return;
	}

	for (int i = 0; i < strlen(var_list); i++) {
		if (var_list[i] == var_name) {
			Matrix[row][i] = coef;
		}
	}
}

bool read_line(const char* fileName) {
	FILE* file;
	errno_t res;

	if (res = fopen_s(&file, fileName, "r")) {
		fprintf(stderr, "Cannot open file. Error no: %d\n", res);
		return 0;
	}

	do {
		char line_buffer[128] = ""; // buffer to store line
		int n;
		if ((n = fscanf(file, "%128[^\n]\n", line_buffer)) == 1) {
			if (strcmp(line_buffer, "") == 0) // checking if line is empty
				continue;

			char* first = strtok(line_buffer, "=");
			int len = strlen(first);

			for (int i = 0; i < len; i++) {
				if (isalpha(first[i])) {
					if (strchr(var_list, first[i]) == 0) {
						var_list[c_count++] = first[i];
					}
				}
			}
		}
		else if (n != EOF) {
			fputs("Couldn't find a match.\n", stderr);
			fclose(file);
			return 0;
		}
		else
			break;
	} while (1);

	c_count += 1; // increment 1 for constant term

	rewind(file);

	do {
		char line_buffer[128] = ""; // buffer to store line
		int n;
		if ((n = fscanf(file, "%128[^\n]\n", line_buffer)) == 1) {
			if (strcmp(line_buffer, "") == 0) // checking if line is empty
				continue;

			char* first = strtok(line_buffer, "=");
			char* second = strtok(NULL, "=");
			int len = strlen(first);

			for (int i = 0, offset = 0; i < len; i += offset) {
				bool is_negative = 0;
				if (first[i] == '-' || first[i] == '+') {
					is_negative = first[i] == '-';
					i++;
				}
				float coef = 1.0; // 2x, x
				char var = 0;
				if (sscanf(&first[i], "%f%c%n", &coef, &var, &offset) == 0) {
					var = first[i];
					offset = 1;
				}
				if (is_negative)
					coef *= -1.0;
				put_coef_into_matrix(var, r_count, coef);
			}

			put_coef_into_matrix(0, r_count, atof(second));
			r_count++;
		}
		else if (n != EOF) {
			fputs("Couldn't find a match.\n", stderr);
			fclose(file);
			return 0;
		}
		else
			break;
	} while (1);

	fclose(file);

	return 1;
}

void solve_matrix(int x, int y) {
	float* row = Matrix[y];
	float coef = row[x];
	if (coef == 0.0) {
		float* nonZeroRow = NULL;
		for (int i = y + 1; i < r_count && !nonZeroRow; i++) {
			if (Matrix[i][x] != 0.0) {
				nonZeroRow = Matrix[i];
			}
		}

		if (nonZeroRow) {
			for (int i = 0; i < c_count; i++) { // swapping rows
				float temp_data = nonZeroRow[i];
				nonZeroRow[i] = row[i];
				row[i] = temp_data;
			}
			solve_matrix(x, y);
		}
		else {
			if (x + 1 < c_count)
				solve_matrix(x + 1, y);
		}
	}
	else {
		if (coef != 1.0) {
			for (int i = 0; i < c_count; i++)
				row[i] = row[i] * 1 / coef;
		}
		for (int i = 0; i < r_count; i++) {
			if (i == y || Matrix[i][x] == 0.0)
				continue;
			
			float mult = -1 * Matrix[i][x];
			for (int j = 0; j < c_count; j++)
				Matrix[i][j] = Matrix[i][j] + row[j] * mult;
		}
		if (x + 1 < c_count && y + 1 < r_count) // check here
			solve_matrix(x + 1, y + 1);
	}
}

void main() {
	read_line("eqs.txt");

	solve_matrix(0, 0);

	for (int i = 0; i < strlen(var_list); i++) {
		printf("%c = %f\n", var_list[i], Matrix[i][c_count - 1]);
	}
}