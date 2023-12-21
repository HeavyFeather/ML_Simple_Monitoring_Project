#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;
	char var;
	int i = 0, j;
	int hour = 0, flag = 0;
	char *filename = argv[1];
	fp1 = fopen(filename, "r");
	fp2 = fopen("real_data.csv","w");
	fputs("month,date,time,user,command,working,non-working,class\n",fp2);
	while(1) {
		var = fgetc(fp1); //взяли символ
	
		if(var == EOF) break;
		if(var == 58) flag++;
		if(var > 47 && var < 58 && i > 1 && flag == 0) { //time checking
			if(hour > 0) hour *= 10;
		hour += var - 48;
		}
		
		if(var == 32) { //если пробел, то смотрим на их количество
			if(i < 4) {
			fputc(44, fp2); //заменяем на ,
			i++;
			continue;
			}
			fputc(32, fp2);
		continue;
		}

		if(var == 44 || var == 59) {
			fputs(" ",fp2);
			continue;
		}
		
		if(var == 10) { //для классификации
			if(hour < 8 || hour > 20) {
				fputs(",0,1,0",fp2);
				//printf(",");
			} else {
				fputs(",1,0,0",fp2);
				//printf(",abnormal");
			}
			hour = 0;
			flag = 0;
			i = 0; // если LF, то обнуляем количество порбелов
		}
		
		fputc(var, fp2);
	}
	fclose(fp1);
	fclose(fp2);
	return 0;
}
// 32 - space
// 44 - ,
// 10 - \n
// 59 - ;
