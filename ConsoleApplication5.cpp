#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <locale.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>     /* atoi */
#include <string>
#include <TCHAR.H> 

using namespace std;

// оформление фона консоли перввя цифра - фон, вторая - текст
const int NotUsed = system("color 02");

struct Row{
	char name[128];
	int scores[15];
};

Row rows[200]; // массив структур
Row sorted_rows[200];// структуры после сортировки

char filename[] = "Scoretable.dat";
FILE *f;

int data_loaded = 0; // флаг загрузки данных
int max_name_length = 0;
int judge_numb = 0;
int participant_numb = 0;//количество участников

void diagram(HANDLE hConsole, HWND hcon, PWINDOWINFO pgwi)
{
	int k = participant_numb;
	HDC hdc = GetDC(hcon);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	system("cls");
	HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // для рисования линий
	HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));		 // для заполнения фигур
	HPEN hcp = (HPEN)SelectObject(hdc, hWhitePen);
	int osn = (pgwi->rcClient.bottom - pgwi->rcClient.top - 50);
	MoveToEx(hdc, 100, osn, NULL);	// перемещение курсора
	LineTo(hdc, 100 + k * 35, osn);
	RECT r;
	for (int i = 0; i < k; i++)	{	// здесь должны быть расчеты 

		int ran = sorted_rows[i].scores[judge_numb + 1] / 100 - 50;
		r.left = 100 + i * 35;
		r.top = osn - 300 + ran;
		r.right = 100 + (i + 1) * 35 - 10;
		r.bottom = osn;
		HBRUSH hBrush = CreateSolidBrush(RGB(i * 10 + 100, i * 20 + 50, i * 30 + 100));
		FillRect(hdc, &r, hBrush);
		DeleteObject(hBrush);
	}
	TextOutW(hdc, 150 + k * 35, 50, L"ДИАГРАММА", 10);
	_getch();
	r.left = 0;
	r.top = 0;
	r.right = pgwi->rcClient.bottom;
	r.bottom = pgwi->rcClient.right;
	FillRect(hdc, &r, hBlackBrush);
	DeleteObject(hWhitePen);
	DeleteObject(hBlackBrush);
}



void sort_alphabet() {

	system("cls");

  // сортировка вставками

	for (int i = 0; i < participant_numb; i++) {

		Row temp = sorted_rows[i];
		int q = i;
		//сравниваем элемент с предыдущим двигаясь к 0 индексу
		while (q > 0 && strcmp(temp.name, sorted_rows[q-1].name) < 0) {
			//двигаемся влево, присваивая текущему элементу значение предыдущего, сам элемент двигается вправо
			sorted_rows[q] = sorted_rows[q - 1];
			q--;
		}
		//если нашли элемент который больше сохраненного, то на его место ставим сохраненный, который меньше
		sorted_rows[q] = temp;
	}
}

void print_by_judge(int judge) {
	system("cls");

	if (judge == -1) {
		printf("Неверный номер судьи\n");
		system("pause");
		return;
	}
	// выравнивание таблицы относительно длины имен, подготовка шаблона в виде строк

	string template1 = "\n%-"; // выравнивание по левому краю -
	string template2 = "s\t";
	string template0 = template1 + to_string(max_name_length) + template2; // в шаблон вставлена макисмальная длина имени для определения ширины поля

	for (int k = 0; k < participant_numb + 1; k++) {

		if (k == 0) {
			printf(template0.c_str(), "Участник");
			printf("%s%d\t", "С", judge);

		}
		else {
			// использование шаблона строк в виде массива символов
			printf(template0.c_str(), sorted_rows[k - 1].name);
			printf("%d\t", sorted_rows[k - 1].scores[judge -1]);

		}
	}
	printf("\n\n");
	system("pause");
}

int sort_by_judge() {
	system("cls");
	int judge = 0;
	printf("Введите номер судьи, по которому требуется отсортировать результат: ");
	scanf("%d", &judge);
	if (judge < 1 or judge > judge_numb) return -1;

	int q;

	for (int i = 0; i < participant_numb; i++) {

		Row temp = sorted_rows[i];
		q = i;
		//сравниваем элемент с предыдущим двигаясь к 0 индексу
		while (q > 0 && sorted_rows[q - 1].scores[judge - 1] <= temp.scores[judge - 1]) {
			//двигаемся влево, присваивая текущему элементу значение предыдущего, сам элемент двигается вправо
			sorted_rows[q] = sorted_rows[q - 1];
			q--;
		}
		//если нашли элемент который больше сохраненного, то на его место ставим сохраненный, который меньше
		sorted_rows[q] = temp;
	}
	
	return judge;
}

void print_spread() {
	system("cls");
	// выравнивание таблицы относительно длины имен, подготовка шаблона в виде строк

	string template1 = "\n%-"; // выравнивание по левому краю -
	string template2 = "s\t";
	string template0 = template1 + to_string(max_name_length) + template2; // в шаблон вставлена макисмальная длина имени для определения ширины поля

	for (int k = 0; k < participant_numb + 1; k++) {

		if (k == 0) {
			printf(template0.c_str(), "Участник");
			for (int l = 0; l < judge_numb; l++) {
				printf("%s%d\t", "С", l + 1);
			}
			printf("%s", "Разброс");

		}
		else {
			// использование шаблона строк в виде массива символов
			printf(template0.c_str(), sorted_rows[k - 1].name);
			for (int l = 0; l <= judge_numb; l++) {
				if (l == judge_numb) 
					printf("%d\t", sorted_rows[k - 1].scores[judge_numb+1]);
				else
					printf("%d\t", sorted_rows[k - 1].scores[l]);

			}
		}
	}
	printf("\n\n");
	system("pause");

}


void sort_by_spread() {

	int q;

	for (int i = 0; i < participant_numb; i++) {

		Row temp = sorted_rows[i];
		q = i;
		//сравниваем элемент с предыдущим двигаясь к 0 индексу
		while (q > 0 && sorted_rows[q - 1].scores[judge_numb+1] >= temp.scores[judge_numb+1]) {
			//двигаемся влево, присваивая текущему элементу значение предыдущего, сам элемент двигается вправо
			sorted_rows[q] = sorted_rows[q - 1];
			q--;
		}
		//если нашли элемент который больше сохраненного, то на его место ставим сохраненный, который меньше
		sorted_rows[q] = temp;
	}

}


void sort_by_total_score() {	

	// bubble 1й цикл - колчиество элементов для сравнения, 2й - количество сравнений

	for (int i = 0; i < participant_numb; i++) {
		for (int j = 0; j < participant_numb-1; j++) {
			if (sorted_rows[j].scores[judge_numb] < sorted_rows[j+1].scores[judge_numb]) {
				Row temp = sorted_rows[j];
				sorted_rows[j] = sorted_rows[j + 1];
				sorted_rows[j + 1] = temp;
			}
		}
	}

}

void print_data() {
	system("cls");
	// выравнивание таблицы относительно длины имен, подготовка шаблона в виде строк
		
		string template1 = "\n%-"; // выравнивание по левому краю -
		string template2 = "s\t";
		string template0 = template1 + to_string(max_name_length) + template2; // в шаблон вставлена макисмальная длина имени для определения ширины поля

	for (int k = 0; k < participant_numb + 1; k++) {

		if (k == 0) {
			printf(template0.c_str(), "Участник");
			for (int l = 0; l < judge_numb; l++) {
				printf("%s%d\t", "С",l+1);
			}
			printf("%s", "Всего");

		}
		else {
			// использование шаблона строк в виде массива символов
			printf(template0.c_str(), sorted_rows[k-1].name);
			for (int l = 0; l <= judge_numb; l++) {
				printf("%d\t", sorted_rows[k-1].scores[l]);
			}
		}
		
	}

	printf("\n\n");
	system("pause");

}

void load_data() {

	fopen_s(&f, filename, "r");

	if (!f) {
		printf("Файл не удалось открыть.");
		return;
	}

	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 15; j++) {
			rows[i].scores[j] = 0;
		}
	}

	int semicolon_counter = 0;
	int max_semicolon_counter = -1;
	int i = 0;//счетчик строк - количество структур
	char str[256];

	while (fgets(str, 256, f) != NULL)
	{	
		if (semicolon_counter > max_semicolon_counter) max_semicolon_counter = semicolon_counter;

		semicolon_counter = 0;// счетчик точек с запятой
		int n = 0; //счетчик символов в имени 
		
		char c;
		int ci = 0;

		do {
			c = str[ci];
			if (c == ';') {
				if (semicolon_counter == 0) i++;
				semicolon_counter++;
			}
			if (semicolon_counter == 0) {
				rows[i].name[n++] = c;
				//подсчет имени максимальной длины 
				if (n > max_name_length) {
					max_name_length = n;
				}
			}
			if (semicolon_counter != 0 and (c >= 48 and c <= 57)) {
				char cc[1] = { c };
				rows[i-1].scores[semicolon_counter - 1] = rows[i-1].scores[semicolon_counter - 1] * 10 + atoi(cc);
			}
			ci++;
		} while (c!='\n');
	} 

	judge_numb = max_semicolon_counter;//сколько ; столько и судей

	participant_numb = i; // количество строк - количество игроков

	if (f) fclose(f);

	for (int k = 0; k < participant_numb; k++) {
		int total_score = 0;
		int min = 9999;
		int max = -9999;
		int j = 0;
		for (; j < judge_numb; j++) {
			if (rows[k].scores[j] > max) max = rows[k].scores[j];
			if (rows[k].scores[j] < min) min = rows[k].scores[j];
			total_score += rows[k].scores[j];
		}
		total_score = (total_score - max - min) / (judge_numb - 2);
		rows[k].scores[j] = total_score;
	}

	for (int k = 0; k < participant_numb; k++) {

		int spread = 0; // разброс
		int min = 9999;
		int max = -9999;
		int j = 0;
		for (; j < judge_numb; j++) {
			if (rows[k].scores[j] > max) max = rows[k].scores[j];
			if (rows[k].scores[j] < min) min = rows[k].scores[j];
		}
		spread = max - min;
		rows[k].scores[judge_numb + 1] = spread;
	}

	for (int i = 0; i < participant_numb; i++) {
		sorted_rows[i] = rows[i];
	}

	data_loaded = 1;

}

void enter_data() {

	int choice1 = 1;

	while (true) {

		if (choice1 == 0) {
			fclose(f);
			return;
		}			
		fopen_s(&f, filename, "a");

		//сброс буфера считывание последних символов недосчитанных сканом до этого

		while (char c = getchar() != '\n' && c != EOF);

		system("cls");
		printf("Добавление нового участника\n");

		if (!f) {
			printf("Файл не удалось открыть.");
			return;
		}
		
		char name[128];
		printf("\nВведите ФИО участника: ");
		gets_s(name);
		fprintf(f, "%s;", name);

		char scores[128];
		printf("Введите баллы участника по каждому судье через пробел: \n");
		gets_s(scores);

		for (int i = 0; i < 127; i++) {
			if (scores[i] == ' ') scores[i] = ';';
		}
		fprintf(f, "%s\n", scores);

		if (f) fclose(f);
		printf("Продолжить ввод?\n");

		printf("1 - Да\n");
		printf("0 - Нет\n");

		scanf_s("%d", &choice1);
	}
}

int file_check() {

	int click;
	fopen_s(&f, filename, "r");
	if (!f) {
		printf("Файл не найден.\n");
		printf("Выберите действие: \n");

		printf("1 - создать новый файл\n");
		printf("0 - выйти\n");

		scanf_s("%d", &click);

		if (click == 0) return 0;


		fopen_s(&f, filename, "w");

	}

	if (f) {
		fclose(f);
	}

	return -1;
}

/*void Menu() {

	int choice = file_check();

	while (choice != 0) {
		system("cls");
		printf("\nВыберите действие: \n");
		printf("0 - Выход\n");
		printf("1 - Ввод исходных данных\n");
		printf("2 - Печать итоговой таблицы\n");
		printf("3 - Спортсмены с наибольшим и наименьшим разбросом оценок\n");
		printf("4 - Места по оценкам судьи\n");
		printf("5 - Список спортсменов по алфавиту\n");
		printf("6 - Диаграмма разброса оценок\n");

		printf("\nВаш выбор: ");

		scanf_s("%d", &choice);

		switch (choice) {

			case 1:
				enter_data();
				load_data();
				break;
			case 2:
				if (data_loaded == 0) // если данные незагр загрузить		
					load_data(); 		
				sort_by_total_score();
				print_data();
				break;
			case 3:
				if (data_loaded == 0) 
					load_data();
				sort_by_spread();
				print_spread();
				break;
			case 4:
				if (data_loaded == 0) 
					load_data();
				print_by_judge(sort_by_judge());
				break;
			case 5:
				if (data_loaded == 0) 
					load_data();
				sort_alphabet();
				print_data();
				break;
		}
		
	}

}

void main1() {
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Menu();
}
*/

int main(int argc, _TCHAR* argv[])
{
	// получение информации о консольном окне
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hcon = GetConsoleWindow();

	// программная установка консольного шрифта для отображения русского текста
	CONSOLE_FONT_INFO cfi;
	GetCurrentConsoleFont(hConsole, false, &cfi);
	COORD fs = GetConsoleFontSize(hConsole, cfi.nFont);
	PCONSOLE_FONT_INFOEX ccf = new CONSOLE_FONT_INFOEX;
	(*ccf).dwFontSize.X = 12;
	(*ccf).dwFontSize.Y = 20;
	(*ccf).nFont = 11;
	(*ccf).cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ccf->FontWeight = 400;
	lstrcpyW((*ccf).FaceName, L"Lucida Console");
	(*ccf).FontFamily = FF_DONTCARE;
	bool b = SetCurrentConsoleFontEx(hConsole, false, ccf);
	fs = GetConsoleFontSize(hConsole, cfi.nFont);

	// установка кодировки 1251 для отображения русского текста
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	int n = 7;			//кол-во пунктов меню
	COORD position[7];
	int punkt = 0;
	char names[7][154] = {"Ввод данных", "Просмотр итоговой таблицы", "Просмотр спортсменов по разбросу", "Места по оценкам судьи", "Спортсмены по алфавиту", "Диаграмма разброса оценок" ,"Выход" };
	do {
		int xmax, ymax;

		// получение параметров окна 
		PCONSOLE_SCREEN_BUFFER_INFO pwi = new CONSOLE_SCREEN_BUFFER_INFO;
		PWINDOWINFO pgwi = new WINDOWINFO;
		GetConsoleScreenBufferInfo(hConsole, pwi);
		GetWindowInfo(hcon, pgwi);
		xmax = pwi->dwSize.X;
		ymax = pwi->dwSize.Y;

		int y0 = 7;

		// выравнивание по левому краю относительно первого элемента меню

		position[0].X = (xmax - strlen(names[0])) / 4;
		position[0].Y = y0 + 0;

		for (int i = 1; i < n; i++)
		{
			position[i].X = position[0].X;
			position[i].Y = y0 + i;
		}

		SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE); // цвет фона
		system("cls");			// очистка окна
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | BACKGROUND_BLUE); // цвет символов и цвет фона
		char title[] = "БАЗА ДАННЫХ";
		COORD pos;
		pos.X = (xmax - strlen(title)) / 4;
		pos.Y = 5;
		SetConsoleCursorPosition(hConsole, pos);
		puts(title);
		for (int i = 0; i < 7; i++)
		{
			SetConsoleCursorPosition(hConsole, position[i]);
			puts(names[i]);
		}
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
		SetConsoleCursorPosition(hConsole, position[punkt]);	// выделение текущего пункта ярким цветом
		puts(names[punkt]);
		unsigned char ch;

		do
		{
			// обработка перемещения по меню клавишами со стрелками
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | BACKGROUND_BLUE);
					puts(names[punkt]);
					punkt--;
					if (punkt < 0) punkt = 6;
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;
				case 80:
					SetConsoleCursorPosition(hConsole, position[punkt]);
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | BACKGROUND_BLUE);
					puts(names[punkt]);
					punkt++;
					if (punkt > 6) punkt = 0;
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
					SetConsoleCursorPosition(hConsole, position[punkt]);
					puts(names[punkt]); break;
				}
			}
		} while (ch != 13);	// enter - выбор пункта меню

		switch (punkt)
		{
		case 0:
			enter_data();
			load_data();
			break;
		case 1:
			if (data_loaded == 0) load_data(); // если данные незагр загрузить				
			sort_by_total_score();
			print_data();
			break;
		case 2:
			if (data_loaded == 0) load_data();
			sort_by_spread();
			print_spread();
			break;

		case 3:
			if (data_loaded == 0) load_data();
			print_by_judge(sort_by_judge());
			break;
		case 4:
			if (data_loaded == 0) load_data();
			sort_alphabet();
			print_data();
			break;
		case 5:
			if (data_loaded == 0) load_data();
			diagram(hConsole, hcon, pgwi);	// пример рисования диаграммы
			break;

		}
	} while (punkt != 6);
	return 0;
}


