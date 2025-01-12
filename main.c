#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

int delkadata(char *data, int velikost){
	int delka = 0;
	for(int a = 1; a <= velikost; a++){
		if(data[a] != 0x00){
			delka++;
		}
	}
	return delka;
}

void vytiskni(char capacity[3], int status, char manufacturer[6], char model[8], float voltage, float voltagemin, int cislobat){
//				inicializace promennych
	int x = 50, y = 10, r, g;
	char data[x], buffer[x], zobrazjednou[4], barva[20];
	char *tty = ttyname(STDIN_FILENO);
	voltage = voltage / 100000; voltagemin = voltagemin / 100000;


//		Vytvoreni ramecku (obdelnik)
	for(int a = 0; a <= y; a++){
		for(int b = 0; b <= x; b++){ buffer[b] = ' '; data[b] = 0x00; }
		for(int b = 0; b <= x; b++){
			if(a == 0 || a == y){
				if(b <= x-4){ buffer[b] = '-'; }
			}
			else{
				if(b == 0 || b == x-4){ buffer[b] = '-'; }
			}
		}
//		Pridani "+ polu" do ramecku aby vypadal jako AA baterie
		     if(a == 4){ buffer[x-4] = '-'; buffer[x-3] = '-'; buffer[x-2] = '-'; buffer[x-1] = '-'; buffer[x-0] = '-'; }
		else if(a == 5){ buffer[x-4] = '-'; buffer[x-3] = '-'; buffer[x-2] = '-'; buffer[x-1] = '-'; buffer[x-0] = '-'; }
		else if(a == 6){ buffer[x-4] = '-'; buffer[x-3] = '-'; buffer[x-2] = '-'; buffer[x-1] = '-'; buffer[x-0] = '-'; }
//		Data
		switch(a){
			case 2:
				sprintf(data, "Baterie: %d", cislobat);
				break;
			case 3:
				for(int b = 0; b <= 11; b++){ data[b] = '_'; }
				break;
			case 4:
				sprintf(data, "%s, %s", manufacturer, model);
				break;
			case 5:
				switch(status){
					case 1:
						sprintf(data, "Nabiji se: %s%%", capacity);
						break;
					case 2:
						sprintf(data, "Vybiji se: %s%%", capacity);
						break;
					case 3:
						sprintf(data, "Nenabiji se: %s%%", capacity);
						break;
				}
				break;
			case 6:
				sprintf(data, "      Min     Soucasna   Zbyvajici");
				break;
			case 7:
				sprintf(data, "%.3fV    %.3fV    %.3fV", voltagemin, voltage, voltage-voltagemin);
				break;
			case 8:
					switch(status){
						case 1:
							if(voltage == 100){ sprintf(data, "Baterie je plne nabita. "); }
										  else{ sprintf(data, "Baterie se nabiji. "); }
						case 2:
							     if(atoi(capacity) >= 80                        ){ sprintf(data, "Baterie je nabita. "); }
							else if(atoi(capacity) >= 20 && atoi(capacity) <= 80){ sprintf(data, "Baterie je v poradku. "); }
							else if(                        atoi(capacity) <= 20){ sprintf(data, "Baterie je vybita. "); }
					}
				break;
		}
//		Zapis dat do ramecku
		for(int b = 0; b <= sizeof(data); b++){
			if(data[b] != 0x00){
				buffer[(((x/2)-(delkadata(data, sizeof(data))/2))+b)-2] = data[b];
			}
		}
//		Vytisknuti ramecku
		for(int b = 0; b <= 3; b++){ zobrazjednou[b] = '0'; }
		for(int b = 0; b <= x; b++){
			if(atoi(capacity)/2 >= b){
				if(strstr(tty, "pts")){
					 r=255; g=0;
					sprintf(barva, "\x1b[48;2;%d;%d;0m", r-(b*5), g+(b*5));
					printf(barva);
				}
				else{
//					     if(atoi(capacity) <= 20 && zobrazjednou[0] == '0' ){ printf("\x1B[41m"); zobrazjednou[0] = 1; }
//					else if(atoi(capacity) > 20 && atoi(capacity) <= 80 && zobrazjednou[0] == '0' ){ printf("\x1B[43m"); zobrazjednou[0] = 1; }
//					else if(atoi(capacity) > 80 && zobrazjednou[0] == '0' ){ printf("\x1B[42m"); zobrazjednou[0] = 1; }
					     if(b >= 0  && b < 19){ printf("\x1B[41m"); }
					else if(b >= 20 && b < 39){ printf("\x1B[43m"); }
					else if(b >= 40 && b < 50){ printf("\x1B[42m"); }
				}
			}
			else{
				if(zobrazjednou[3] == '0'){
					printf("\x1B[49m");
					zobrazjednou[3] = '1';
				}
			}
			if(buffer[b] == '-'){ printf("\x1b[48;2;255;255;255m"); } if(b > 46){ if(buffer[b] == ' '){ printf("\x1b[49m"); } }
			printf("%c", buffer[b]);
		}
		printf("\n");
	}
	printf("\x1B[49m");
}


int main(){
//				inicializace promennych
	char technology[7], manufacturer[6], model[8], status[15], capacity[3], voltage[8], voltagemin[8], cesta[40];
	int statuss;
	FILE *soubor;
	DIR* dir;
//				smazani terminalu
	system("clear");
	printf("\n");
//				Otevreni souboru, ziskani dat a zavreni souboru

	for(int a = 0; a <= 9; a++){
		sprintf(cesta, "/sys/class/power_supply/BAT%d", a);
		dir = opendir(cesta);
		if(dir){
			closedir(dir);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/technology",         a);
			soubor = fopen(cesta, "r"); fgets(technology,   sizeof(technology),   soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/technology",         a);
			soubor = fopen(cesta, "r"); fgets(technology,   sizeof(technology),   soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/manufacturer",       a);
			soubor = fopen(cesta, "r"); fgets(manufacturer, sizeof(manufacturer), soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/model_name",         a);
			soubor = fopen(cesta, "r"); fgets(model,        sizeof(model),        soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/status",             a);
			soubor = fopen(cesta, "r"); fgets(status,       sizeof(status),       soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/capacity",           a);
			soubor = fopen(cesta, "r"); fgets(capacity,     sizeof(capacity),     soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/voltage_now",        a);
			soubor = fopen(cesta, "r"); fgets(voltage,      sizeof(voltage),      soubor); fclose(soubor);
			sprintf(cesta, "/sys/class/power_supply/BAT%d/voltage_min_design", a);
			soubor = fopen(cesta, "r"); fgets(voltagemin,   sizeof(voltagemin),   soubor); fclose(soubor);
//				informace o nabijeni
		         if(strstr(status, "Charging"))    { statuss = 1; }
			else if(strstr(status, "Discharging")) { statuss = 2; }
			else if(strstr(status, "Not charging")){ statuss = 3; }

			vytiskni(capacity, statuss, manufacturer, model, atoi(voltage), atoi(voltagemin), a);
			printf("\n");
		}
	}


	printf("\n");
	return 0;
}
