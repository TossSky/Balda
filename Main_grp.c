#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "wincon_new.h"

#pragma warning (disable : 6386)

/*���������� ����������*/
#define true_ 1
#define false_ 0
/*�������� ���*/
#define FONT CON_CLR_WHITE
/*�������������� ������� �� 11� ����*/
#define dictionary "dictionary.txt" 
/*��� ��������������� ��������, ������� ���� ~45k*/
#define inversion "InvPrefix.txt"
/*����, ��������������� ������*/
#define BLOCKED 2
/*���� � ����������� �����*/
#define DOWNLOAD "Download.txt"
/*�������*/
#define REC "Records.txt"

/*����������*/
typedef struct map_
{
	short x;
	short y;
} map;

struct prefix_tree_;

/*���������, �������� ��������*/
typedef struct element_
{
	char letter;
	struct prefix_tree_* next;
} element;

/*���� ������*/
typedef struct prefix_tree_
{
	/*����������� ��������� ������*/
	element current[33];
	/*���������� ����� �� �����*/
	enum
	{
		false,
		true
	} BOOL;
} prefix_tree;

typedef struct field_cell
{
	/*����� ������*/
	char letter;
	/*������ ������*/
	enum
	{
		open,
		close
	} pass;
} cell;

/*������ ��� ������������ ����*/
typedef struct Added
{
	char word[30];
	struct Added* next;
} added;

/*��������������� ������ ��� ������ ��������*/
typedef struct rec_
{
	char name[30];
	int points;
	struct rec_* left;
	struct rec_* right;
} vsOv;

prefix_tree* root, * root_inv;
added* list = NULL;
FILE* saved, * records;
int max_size = 0, start_x = 0, start_y = 0, lvl = 0, last_stand = 0, word_y = 3;
int from_x = 0, from_y = 0, check_end = 0, point_1 = 0, point_2 = 0, comp = 0, move = 0;
char longest[30], start_letter;

/*������� ��� ������� ������ ����� ��������*/
void free_tree(prefix_tree* node)
{
	for (int i = 0; i < 33; i++)
	{
		if (node->current[i].letter != NULL)
		{
			free_tree(node->current[i].next);
			continue;
		}
		free(node);
		break;
	}
}

/*������� ������*/
void clear(cell** field)
{
	free(field);
	prefix_tree* node = root;
	free_tree(node);
	node = root_inv;
	free_tree(node);
}

/*����� ����� � �������� ������*/
int search_in_list(char* word_)
{
	added* current = list;
	while (current != NULL)
	{
		/*���� ����������*/
		if (strcmp(current->word, word_) == 0)
			return 0;
		current = current->next;
	}

	/*��� ����������*/
	return 1;
}

/*���������� � ���� ������� ����*/
void add_to_list(char* word_)
{
	added* node = (added*)malloc(sizeof(added)), * current = list, * prev = NULL;
	if (node == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(node, 0, sizeof(added));
	node->next = NULL;
	strcpy(node->word, word_);

	if (current == NULL)
	{
		list = node;
		return;
	}

	while (current != NULL)
	{
		prev = current;
		current = current->next;
	}
	current = node;
	prev->next = current;
}

/*���������� ������ �������� ������ ����*/
int search_in(char let, prefix_tree* node)
{
	for (int i = 0; i < 33; i++)
	{
		/*����� �� ����������� -> �������� � ������� �������*/
		if (node->current[i].letter == NULL)
			return i;
		if (node->current[i].letter == let)
			return i;
	}
}

/*������� ��� ������� ��������� � �������*/
void insert(char* word, prefix_tree* root)
{
	/*������� ����*/
	prefix_tree* node = root;
	for (int i = 0; i < strlen(word); i++)
	{
		int result = search_in(word[i], node);
		if (node->current[result].letter == NULL)
		{
			/*��� �� ����������� ����� �� ������ ����� -> ���������� � ������*/
			node->current[result].letter = word[i];
		}
		if (node->current[result].next == NULL)
		{
			prefix_tree* new_node = (prefix_tree*)malloc(sizeof(prefix_tree));
			if (new_node == NULL)
			{
				/*mistake here*/
				exit(1);
			}
			memset(new_node, 0, sizeof(prefix_tree));
			new_node->BOOL = false;
			node->current[result].next = new_node;
		}
		node = node->current[result].next;
	}
	node->BOOL = true;
}

/*���������� ��������*/
void dict_read(prefix_tree* root, FILE* dict)
{
	/*������� ���������� �������� �� ������*/
	char ins_array[30];
	int i = 0;
	while (fgets(ins_array, 30, dict))
	{
		int size = strlen(ins_array) - 1;
		if (ins_array[size] == '\n')
			ins_array[size] = '\0';
		insert(ins_array, root);
	}
}

/*�������� ����� �� �������� -> 1 - ���-�� �������*/
int main_search(prefix_tree* root_, char* word)
{
	prefix_tree* node = root_;
	for (int i = 0; i < strlen(word); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == NULL)
			{
				/*���� ����� �� �������� � ���������� NULL - ������ �������� ��� ������*/
				return 0;
			}
			if (node->current[j].letter == word[i])
			{
				node = node->current[j].next;
				break;
			}
		}
	}
	/*���� ����������*/
	if (node->BOOL == true)
		return 1;

	return 0;
}

/*�������� ��������*/
int check_valid(char* new_current)
{
	/*���� � �������� ��� ����� � ������� -> �� ����������*/
	prefix_tree* node = root_inv;
	for (int i = 0; i < strlen(new_current); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == new_current[i])
			{
				node = node->current[j].next;
				break;
			}
			if (j == 32)
				return 0;
		}
	}

	/*���� ������� ���� � ������ ���������*/
	return 1;
}

/*����� ������ ����� � ����*/
int search_node(prefix_tree* node, char fnd)
{
	for (int i = 0; i < 33; i++)
	{
		if (node->current[i].letter == NULL)
			return -1;
		if (node->current[i].letter == fnd)
			return i;
	}

	return -1;
}

/*���������� ��������� �� ��������� ���� �� �������� �������*/
void dict_search(cell** field, int x, int y, prefix_tree* node, char* new_current)
{
	/*���� ������� ��������� ������ - ���������� ����� ������*/
	if (lvl < 3)
	{
		/*if (lvl == 1 && strlen(new_current) > 4)
			return;*/
		/*else */if (lvl == 2 && strlen(new_current) > 5)
			return;
	}

	/*�������� �������� ����� �� �������������*/
	if (node->BOOL == true && strlen(new_current) > max_size)
	{
		if (search_in_list(new_current))
		{
			max_size = strlen(new_current);
			strcpy(longest, new_current);
		}
	}

	if (lvl == 1)
		return;

	/*�������� ������, ������� �� ����� �� ���� �������*/
	int result = 0, cl = 0;
	if (field[x][y].pass == open)
	{
		field[x][y].pass = close;
		cl++;
	}

	/*����������� ����� ��������� ����*/

	/*�������� ������� ������*/
	result = search_node(node, field[x - 1][y].letter);
	if (field[x - 1][y].letter != ' ' && field[x - 1][y].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x - 1][y].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x - 1, y, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*�������� ������ ������*/
	result = search_node(node, field[x][y + 1].letter);
	if (field[x][y + 1].letter != ' ' && field[x][y + 1].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x][y + 1].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x, y + 1, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*�������� ������ ������*/
	result = search_node(node, field[x + 1][y].letter);
	if (field[x + 1][y].letter != ' ' && field[x + 1][y].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x + 1][y].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x + 1, y, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}
	/*�������� ����� ������*/
	result = search_node(node, field[x][y - 1].letter);
	if (field[x][y - 1].letter != ' ' && field[x][y - 1].pass != close && result != -1)
	{
		new_current[strlen(new_current)] = field[x][y - 1].letter;
		new_current[strlen(new_current)] = '\0';
		dict_search(field, x, y - 1, node->current[result].next, new_current);
		new_current[strlen(new_current) - 1] = '\0';
	}

	/*���� ������ ������� ���� ������� - �������*/
	if (cl == 1)
		field[x][y].pass = open;

	return;
}

/*������� �������������� ������� + �������� � ����, � �������� ���������� ��������*/
prefix_tree* create(char* new_current)
{
	/*�������� �������� ����� ��� "��������" � �������� �������*/
	int idx = strlen(new_current) - 1;
	for (int i = 0; i < strlen(new_current) / 2; i++, idx--)
	{
		char swap = new_current[i];
		new_current[i] = new_current[idx];
		new_current[idx] = swap;
	}

	/*������� � ����, � �������� ���������� �������� "�������"*/
	prefix_tree* node = root;
	for (int i = 0; i < strlen(new_current); i++)
	{
		for (int j = 0; j < 33; j++)
		{
			if (node->current[j].letter == new_current[i])
			{
				node = node->current[j].next;
				break;
			}
		}
	}

	/*���������� ����, � �������� ����� ��������*/
	return node;
}

/*����������� ����� ��������� + �������� �� ���������� � �������� �������*/
void rec_prefix(cell** field, int x, int y, char* current, int idx)
{
	/*���� ������ ������ ����������*/
	if (field[x][y].letter == ' ' || field[x][y].letter == BLOCKED)
		return;

	/*���������� �������� ����� + �������� ������� ������*/
	current[idx] = field[x][y].letter;
	field[x][y].pass = close;
	current[idx + 1] = '\0';

	/*����� ���������� ��� �������� � ��������� �������*/
	char new_current[30];
	memset(new_current, 0, 30);
	strcpy(new_current, current);

	/*�������� �������� ����� � ������ ���������*/
	if (main_search(root_inv, current))
	{
		prefix_tree* node = create(new_current);
		/*���������� �������*/
		dict_search(field, from_x, from_y, node, new_current);
	}
	else
	{
		/*�������� ���� �� ��������� ���������� ����������
		-> ������������ ���������� ����������� �������*/
		if (!check_valid(new_current))
		{
			field[x][y].pass = open;
			return;
		}
	}


	/*������� ������*/
	if (field[x - 1][y].letter != ' ' && field[x - 1][y].pass != close)
		rec_prefix(field, x - 1, y, current, idx + 1);
	current[idx + 1] = '\0';
	/*������ ������*/
	if (field[x][y + 1].letter != ' ' && field[x][y + 1].pass != close)
		rec_prefix(field, x, y + 1, current, idx + 1);
	current[idx + 1] = '\0';
	/*������ ������*/
	if (field[x + 1][y].letter != ' ' && field[x + 1][y].pass != close)
		rec_prefix(field, x + 1, y, current, idx + 1);
	current[idx + 1] = '\0';
	/*����� ������*/
	if (field[x][y - 1].letter != ' ' && field[x][y - 1].pass != close)
		rec_prefix(field, x, y - 1, current, idx + 1);
	current[idx + 1] = '\0';

	/*�������� ������� ������*/
	field[x][y].pass = open;
}

/*�������� ���� ����*/
void find_prefix(cell** field, int x, int y) // field[][7]
{
	int start = 224;
	char current[30], idx = 0;
	memset(current, 0, 30);

	/*�������� ���� ���� �������� (����� �)*/
	for (int i = 0; i < 32; i++)
	{
		int before = max_size;
		field[x][y].letter = start;
		rec_prefix(field, x, y, current, idx);
		/*������������ ������ ���������*/
		if (before < max_size)
		{
			start_x = x;
			start_y = y;
			start_letter = start;
		}
		field[x][y].letter = ' ';
		memset(current, 0, 30);
		start++;
	}
}

/*����� �������� ���� � �������*/
void print_field(cell** field, int field_size)
{
	for (int i = 1; i < field_size + 1; i++)
	{
		for (int j = 1; j < field_size + 1; j++)
		{
			printf("%c", field[i][j].letter);
		}
		printf("\n");
	}
}

/*�������� ������� ����*/
void clean_words()
{
	for (int i = 3; i < 26; i++)
	{
		gotoxy(11, i);
		for (int j = 0; j < 15; j++)
			fprintf(stdout, " ");
	}
	word_y = 3;
}

/*��������� �������*/
void recV()
{
	records = fopen(REC, "a");
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	gotoxy((x - strlen("�����������! �� �������, ������ 100 ������")) / 2, y);
	fprintf(stdout, "�����������! �� �������, ������ %d ������!", point_1);
	gotoxy((x - strlen("�����������! �� �������, ������ 100 ������")) / 2, y + 2);
	con_draw_release();

	fprintf(stdout, "������� ���� ���: ");
	char name[30];
	if (name != NULL)
		memset(name, 0, 30);
	else
		exit(1);

	show_cursor(1);
	if (fscanf(stdin, "%25s", name) != 1)
		exit(1);
	fprintf(records, "%d %s\n", point_1, name);
	show_cursor(0);
	pause(3000);
	fclose(records);
}

/*��� ����*/
int mrOvechkin_move(cell** field, int size)
{
	int start_check = 0;
	for (int i = 1; i < size + 1; i++)
	{
		for (int j = 1; j < size + 1; j++)
		{
			/*����� ������, � ������� ����� �������� �������*/
			if (field[i][j].letter == ' ' && (field[i + 1][j].letter < 0 ||
				field[i - 1][j].letter < 0 || field[i][j + 1].letter < 0 || field[i][j - 1].letter < 0))
			{
				start_check++;
				from_x = i;
				from_y = j;
				find_prefix(field, i, j);
			}
		}
	}

	if (max_size == 0)  
	{
		check_end = 1;
		if (start_check != 0)
		{
			/*���������� � ������� ��������*/
			recV();
			return 1;
		}
	}
	else
	{
		field[start_x][start_y].letter = start_letter;
		gotoxy((con_width_new() - size * 5) / 2 + 6 * (start_y - 1) + 2,
			(con_height_new() - size * 3) / 2 + 3 * (start_x - 1) + 1);
		fprintf(stdout, "%c", start_letter);
		point_2 += strlen(longest);
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
		if (word_y < 25)
		{
			gotoxy(11, word_y++);
			fprintf(stdout, longest);
		}
		else
		{
			clean_words();
		}
		add_to_list(longest);

		max_size = 0;
	}
	return 0;
}

/*�������� ������*/
void create_FILES(FILE** dict, FILE** inv)
{
	if (((*dict) = fopen(dictionary, "r+")) == NULL || ((*inv) = fopen(inversion, "r+")) == NULL)
	{
		/*mistake here*/
		exit(1);
	}
}

/*�������� ��������-��������*/
void create_TRIES()
{
	root = (prefix_tree*)malloc(sizeof(prefix_tree));
	if (root == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(root, 0, sizeof(prefix_tree));

	root_inv = (prefix_tree*)malloc(sizeof(prefix_tree));
	if (root_inv == NULL)
	{
		/*mistake here*/
		exit(1);
	}
	memset(root_inv, 0, sizeof(prefix_tree));
}

/*�������� �������-����*/
cell** create_FIELD(int* field_size)
{
	/*��������� ����������� �������*/
	memset(longest, 0, 30);

	cell** field = (cell**)malloc((*field_size + 2) * sizeof(cell*));
	memset(field, 0, (*field_size + 2) * sizeof(cell*));
	if (field != NULL)
	{
		for (int i = 0; i < *field_size + 2; i++)
		{
			field[i] = (cell*)malloc((*field_size + 2) * sizeof(cell));

			if (field[i] != NULL)
			{
				memset(field[i], 0, (*field_size + 2) * sizeof(cell));
			}
		}
	}

	for (int i = 1; i < *field_size + 1; i++)
	{
		for (int j = 1; j < *field_size + 1; j++)
			field[i][j].letter = ' ';
	}

	for (int i = 0; i < *field_size + 2; i++)
	{
		field[0][i].letter = BLOCKED;
		field[i][0].letter = BLOCKED;
		field[*field_size + 1][i].letter = BLOCKED;
		field[i][*field_size + 1].letter = BLOCKED;
	}

	///*��������� �����*/
	char* start_word = (char*)malloc(*field_size + 10);
	if (start_word != NULL)
		memset(start_word, 0, *field_size + 10);
	else
		exit(1);

	if (*field_size == 5)
		strcpy(start_word, "�����");
	if (*field_size == 7)
		strcpy(start_word, "�������");
	if (*field_size == 10)
		strcpy(start_word, "����������");

	/*���� �������� ���������� - ��������� ���������*/
	if (*field_size % 2)
	{
		for (int i = 1; i < *field_size + 1; i++)
			field[*field_size / 2 + 1][i].letter = start_word[i - 1];
	}
	/*���� ������ - ���������*/
	else
	{
		for (int i = 1; i < *field_size + 1; i++)
			field[i][i].letter = start_word[i - 1];
	}

	return field;
}

/*�������� ���������� ������*/
void delete_red(map* selected, int idx, int size, cell** field)
{
	con_draw_lock();

	for (int i = 0; i < idx; i++)
	{
		gotoxy((con_width_new() - size * 5) / 2 + 6 * selected[i].x + 2,
			(con_height_new() - size * 3) / 2 + 3 * selected[i].y + 1);
		fprintf(stdout, "%c", field[selected[i].y + 1][selected[i].x + 1].letter);
	}

	con_draw_release();
}

/*��������� ���� � ����*/
void main_menu(char** vars, short active_button)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7, line = y - 1, line_2 = (con_width_new() - strlen(vars[4])) / 2 - 8;

	for (int i = 0; i < (16 + strlen(vars[4])) / 2 + 1; i++, line_2 += 2)
	{
		gotoxy(line_2, line - 1);
		fprintf(stdout, "#");
	}

	for (int i = 0; i < 13; i++, line++)
	{
		gotoxy((con_width_new() - strlen(vars[4])) / 2 - 8, line);
		fprintf(stdout, "#");
		gotoxy((con_width_new() + strlen(vars[4])) / 2 + 7, line);
		fprintf(stdout, "#");
	}

	line_2 = (con_width_new() - strlen(vars[4])) / 2 - 8;
	for (int i = 0; i < (16 + strlen(vars[4])) / 2 + 1; i++, line_2 += 2)
	{
		gotoxy(line_2, line);
		fprintf(stdout, "#");
	}

	for (int i = 0; i < 6; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		if (i == active_button)
		{
			con_set_color(FONT, CON_CLR_GRAY);
			fprintf(stdout, "%s", vars[i]);
			con_set_color(FONT, CON_CLR_BLACK);
			continue;
		}
		fprintf(stdout, "%s", vars[i]);
	}
	con_draw_release();
}

/*��������� �������� ������*/
void changer(int active_button, char** vars, int colour)
{
	if (colour == 0)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
	else
	{
		con_set_color(FONT, CON_CLR_GRAY);
	}

	for (int i = 0; i < strlen(vars[active_button]); i++)
	{
		gotoxy((con_width_new() - strlen(vars[active_button])) / 2 + i, con_height_new() / 2 - 7 + 2 * active_button);
		fprintf(stdout, " ");
	}

	gotoxy((con_width_new() - strlen(vars[active_button])) / 2, con_height_new() / 2 - 7 + 2 * active_button);
	fprintf(stdout, "%s", vars[active_button]);

	if (colour == 1)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
}

/*� ���������*/
void about()
{
	clrscr();
	con_draw_lock();

	char* vars[] = { "����� - ���������� ����, ��������� �����", "������ - 4851003/1", "��� - 2021",
		"�����-������������� ��������������� ����������� ����� ��������",
		"�������� ����������������� � ������ ����������", "��� ������ ������� ESC" };
	
	int x = con_width_new(), y = con_height_new() / 2 - 7;
	for (int i = 0; i < 5; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		fprintf(stdout, "%s", vars[i]);
	}

	gotoxy(x - strlen(vars[5]) - 2, con_height_new() - 2);
	fprintf(stdout, "%s", vars[5]);

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*�������� ��� ����� ����*/
void menu_game(int active_button, char** vars)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	for (int i = 0; i < 2; i++, y += 2)
	{
		gotoxy((x - strlen(vars[i])) / 2, y);
		if (i == active_button)
		{
			con_set_color(FONT, CON_CLR_GRAY);
			fprintf(stdout, "%s", vars[i]);
			con_set_color(FONT, CON_CLR_BLACK);
			continue;
		}
		fprintf(stdout, "%s", vars[i]);
	}

	gotoxy(x - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");
	con_draw_release();
}

/*����� ����������*/
void change_lvl(short active, char** vars, int pl, int pl_1)
{
	pl_1 = pl_1 == 0 ? 5 : pl_1;
	pl_1 = pl_1 == 2 ? -3 : pl_1;
	if (pl)
	{
		for (int i = 0; i < strlen(vars[active - 1]); i++)
		{
			gotoxy((con_width_new() - strlen(vars[active - 1])) / 2 + i, con_height_new() / 2 - pl_1);
			fprintf(stdout, " ");
		}
	}
	else
	{
		for (int i = 0; i < strlen(vars[active + 1]); i++)
		{
			gotoxy((con_width_new() - strlen(vars[active + 1])) / 2 + i, con_height_new() / 2 - pl_1);
			fprintf(stdout, " ");
		}
	}
	gotoxy((con_width_new() - strlen(vars[active])) / 2, con_height_new() / 2 - pl_1);
	fprintf(stdout, "%s", vars[active]);
}

/*����������� ������*/
void change_pos(map* coor, short part, int size)
{
	con_draw_lock();
	int x = (con_width_new() - size * 5) / 2 + 6 * coor->x, cur_x = x;
	int y = (con_height_new() - size * 3) / 2 + 3 * coor->y;

	if (part == 0)
	{
		con_set_color(FONT, CON_CLR_BLACK);
	}
	else
	{
		con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	}

	for (int k = 0; k < 5; k++, x++)
	{
		gotoxy(x, y);
		if (k == 0 || k == 4)
			fprintf(stdout, "+");
		else
			fprintf(stdout, "-");
	}
	y++;
	gotoxy(cur_x, y);
	fprintf(stdout, "|");
	gotoxy(x - 1, y);
	fprintf(stdout, "|");
	y++;
	for (int k = 0; k < 5; k++, cur_x++)
	{
		gotoxy(cur_x, y);
		if (k == 0 || k == 4)
			fprintf(stdout, "+");
		else
			fprintf(stdout, "-");
	}

	con_draw_release();
}

/*����� �����*/
void select_(map* coor, int size, cell** field, int var)
{
	int x = (con_width_new() - size * 5) / 2 + 6 * coor->x + 2;
	int y = (con_height_new() - size * 3) / 2 + 3 * coor->y + 1;

	if (!var)
		con_set_color(CON_CLR_RED, CON_CLR_BLACK);

	gotoxy(x, y);
	fprintf(stdout, "%c", field[coor->y + 1][coor->x + 1].letter);
	con_set_color(FONT, CON_CLR_BLACK);
}

/*�������� �������� ������ ������ ��� �������*/
int check(map* selected, map* coor, cell** field, int idx)
{
	if (field[coor->y + 1][coor->x + 1].letter == ' ')
		return false_;

	if (idx == -1)
		return true_;

	for (int i = 0; i < idx; i++)
	{
		if (selected[i].x == coor->x && selected[i].y == coor->y)
			return 0;
	}

	int first = selected[idx].x, second = selected[idx].y;

	/*��������� ������� �� ����*/
	if (coor->x == first && coor->y == second) return 2;

	if (coor->x == first && coor->y == second + 1) return 1;
	if (coor->x == first && coor->y == second - 1) return 1;
	if (coor->x == first + 1 && coor->y == second) return 1;
	if (coor->x == first - 1 && coor->y == second) return 1;

	return 0;
}

/*�������� �����*/
int test(map* selected, int* idx, cell** field, int size, map* new_coor)
{
	con_draw_lock();

	if (size == 5 || size == 7)
	{
		gotoxy((con_width_new() - strlen("��������� �����")) / 2 + 2, 1);
		con_set_color(FONT, CON_CLR_GRAY);
		fprintf(stdout, "��������� �����");
		con_set_color(FONT, CON_CLR_BLACK);
	}

	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("��������� �����") - 6, 0);
		con_set_color(FONT, CON_CLR_GRAY);
		fprintf(stdout, "��������� �����");
		con_set_color(FONT, CON_CLR_BLACK);
	}

	con_draw_release();

	short go = true_;
	while (go)
	{
		
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_DOWN && size != 10)
			{
				gotoxy((con_width_new() - strlen("��������� �����")) / 2 + 2, 1);
				fprintf(stdout, "��������� �����");
				return;
			}
			if (code == KEY_LEFT && size == 10)
			{
				gotoxy(con_width_new() - strlen("��������� �����") - 6, 0);
				fprintf(stdout, "��������� �����");
				return;
			}
			if (code == KEY_ENTER)
			{
				go = false_;
				break;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}


	/*����� �� �������*/
	if (*idx == 0)
	{
		if (size != 10)
		{
			gotoxy((con_width_new() - strlen("��������� �����")) / 2 + 2, 1);
			fprintf(stdout, "��������� �����");
			return;
		}
		if (size == 10)
		{
			gotoxy(con_width_new() - strlen("��������� �����") - 6, 0);
			fprintf(stdout, "��������� �����");
			return;
		}
		return;
	}

	/*���������� �������� �����*/
	char* word = (char*)malloc(*idx + 1);
	if (word != NULL)
		memset(word, 0, *idx + 1);
	else
		exit(1);
	for (int i = 0; i < *idx; i++)
	{
		word[i] = field[selected[i].y + 1][selected[i].x + 1].letter;
	}
	word[*idx] = '\0';

	int yap = 0;
	/*�������������� �� ������������ �����*/
	for (int i = 0; i < *idx; i++)
	{
		if (selected[i].x == new_coor->x && selected[i].y == new_coor->y)
		{
			yap++;
			break;
		}
	}

	if (yap)
	{
		if (main_search(root, word))
		{
			if (size != 10)
			{
				gotoxy((con_width_new() - strlen("����� ���������!")) / 2 + 2, 2);
				if (search_in_list(word))
				{
					fprintf(stdout, "����� ���������!");
					add_to_list(word);
					if (!comp)
						point_1 += strlen(word);
					else
					{
						if (move)
							point_2 += strlen(word);
						else
							point_1 += strlen(word);
					}
					gotoxy(4, 1);
					if (point_1 / 10 < 1)
						fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
					else if (point_1 / 100 < 1)
						fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
					else if (point_1 / 1000 < 1)
						fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
					/*�hange_cells + ���������� ����� � �� � ��*/
					delete_red(selected, *idx, size, field);
					last_stand = 0;
					*idx = 0;
					new_coor->x = -1;
					new_coor->y = -1;
					if (word_y < 25)
					{
						gotoxy(11, word_y++);
						fprintf(stdout, word);
					}
					else
					{
						clean_words();
					}
					/*��� ����*/
					if (!comp)
					{
						if (mrOvechkin_move(field, size) == 1)
							return 1;
					}
					else
						move = move == 0 ? 1 : 0;
				}
			}
			else
			{
				gotoxy(con_width_new() - strlen("����� ���������!") - 6, 1);
				if (search_in_list(word))
				{
					fprintf(stdout, "����� ���������!");
					add_to_list(word);
					if (!comp)
						point_1 += strlen(word);
					else
					{
						if (move)
							point_2 += strlen(word);
						else
							point_1 += strlen(word);
					}
					gotoxy(4, 1);
					if (point_1 / 10 < 1)
						fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
					else if (point_1 / 100 < 1)
						fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
					else if (point_1 / 1000 < 1)
						fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
					/*�hange_cells + ���������� ����� � �� � ��*/
					delete_red(selected, *idx, size, field);
					new_coor->x = -1;
					new_coor->y = -1;
					if (word_y < 25)
					{
						gotoxy(11, word_y++);
						fprintf(stdout, word);
					}
					else
					{
						clean_words();
					}
					last_stand = 0;
					*idx = 0;
					/*��� ����*/
					if (!comp)
					{
						if (mrOvechkin_move(field, size) == 1)
							return 1;
					}
					else
						move = move == 0 ? 1 : 0;
				}
			}
		}
		else
		{
			if (size != 10)
			{
				gotoxy((con_width_new() - strlen("����� ��� � �������")) / 2 + 2, 2);
				fprintf(stdout, "����� ��� � �������");
			}
			else
			{
				gotoxy(con_width_new() - strlen("����� ��� � �������") - 4, 1);
				fprintf(stdout, "����� ��� � �������");
			}
		}
	}

	pause(2000);
	
	if (size != 10)
	{
		gotoxy((con_width_new() - strlen("��������� �����")) / 2 + 2, 1);
		fprintf(stdout, "��������� �����");
		gotoxy((con_width_new() - strlen("����� ��� � �������")) / 2 + 2, 2);
		fprintf(stdout, "                   ");
	}
	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("��������� �����") - 6, 0);
		fprintf(stdout, "��������� �����");
		gotoxy(con_width_new() - strlen("����� ��� � �������") - 4, 1);
		fprintf(stdout, "                   ");
	}

	return 0;
}

/*������ ������� ������� ������*/
int give_up()
{
	con_draw_lock();

	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "�������");

	con_draw_release();
	con_set_color(FONT, CON_CLR_BLACK);
	
	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				clrscr();
				gotoxy((con_width_new() - strlen("� ��������� ��� �������!")) / 2, con_height_new() / 2);
				fprintf(stdout, "� ��������� ��� �������!");
				pause(3000);
				return 1;
			}
			if (code == KEY_RIGHT)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				gotoxy(2, con_height_new() - 2);
				fprintf(stdout, "�������");
				return 0;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*������ ������������ �����*/
int file_size()
{
	saved = fopen(DOWNLOAD, "r");
	fseek(saved, 0, SEEK_END);
	int size = ftell(saved);
	fseek(saved, 0, SEEK_SET);
	return size;
}

/*����� ��������� �� ��������*/
void save_your_tears(int size, cell** field)
{
	clrscr();
	con_draw_lock();

	saved = fopen(DOWNLOAD, "w");

	int x = con_width_new(), y = con_height_new() / 2 - 7, active = 0;

	gotoxy((x - strlen("��������� ����?")) / 2, y);
	fprintf(stdout, "��������� ����?");
	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy((x - strlen("�� ���")) / 2 - 1, y + 2);
	fprintf(stdout, "��");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy((x - strlen("�� ���")) / 2 + 4, y + 2);
	fprintf(stdout, "���");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_RIGHT)
			{
				if (active < 1)
				{
					active++;
					con_set_color(FONT, CON_CLR_BLACK);
					gotoxy((x - strlen("�� ���")) / 2 - 1, y + 2);
					fprintf(stdout, "��");
					con_set_color(FONT, CON_CLR_GRAY);
					gotoxy((x - strlen("�� ���")) / 2 + 4, y + 2);
					fprintf(stdout, "���");
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 0)
				{
					active--;
					con_set_color(FONT, CON_CLR_GRAY);
					gotoxy((x - strlen("�� ���")) / 2 - 1, y + 2);
					fprintf(stdout, "��");
					con_set_color(FONT, CON_CLR_BLACK);
					gotoxy((x - strlen("�� ���")) / 2 + 4, y + 2);
					fprintf(stdout, "���");
				}
			}
			if (code == KEY_ENTER)
			{
				if (active == 1)
				{
					fclose(saved);
					return;
				}
				/*������ ���� + ������� ���������*/
				fprintf(saved, "%d %d %d %d %d\n", size, lvl, comp, word_y, move);
				/*���� �������*/
				fprintf(saved, "%d %d\n", point_1, point_2);
				/*���������� ����*/
				for (int i = 1; i < size + 1; i++)
				{
					for (int j = 1; j < size + 1; j++)
					{
						fprintf(saved, "%c", field[i][j].letter);
					}
					fprintf(saved, "\n");
				}
				/*���������� �������������� ����*/
				added* current = list;
				while (current != NULL)
				{
					fprintf(saved, "%s\n", current->word);
					current = current->next;
				}

				fclose(saved);

				return;
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}
	
}

/*������ ����*/
int start_game(int size, cell** field, int g)
{
	clrscr();
	con_draw_lock();
	int x = (con_width_new() - size * 5) / 2, y = (con_height_new() - size * 3) / 2;
	int prev = 0, idx_1 = 0;
	map selected[150];
	memset(selected, 0, 150 * sizeof(map));

	for (int i = 0; i < size; i++)
	{
		int cur_x = x, cur_x_1 = x;
		for (int j = 0; j < size; j++)
		{
			if (i == 0 && j == 0)
				con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
			else
				con_set_color(FONT, CON_CLR_BLACK);
			for (int k = 0; k < 5; k++, cur_x++)
			{
				gotoxy(cur_x, y);
				if (k == 0 || k == 4)
					fprintf(stdout, "+");
				else
					fprintf(stdout, "-");
			}
			y++;
			gotoxy(cur_x_1, y);
			fprintf(stdout, "|");
			gotoxy(cur_x - 1, y);
			fprintf(stdout, "|");
			y++;
			for (int k = 0; k < 5; k++, cur_x_1++)
			{
				gotoxy(cur_x_1, y);
				if (k == 0 || k == 4)
					fprintf(stdout, "+");
				else
					fprintf(stdout, "-");
			}
			y -= 2;
			cur_x++;
			cur_x_1++;
		}
		y += 3;
	}

	if (g == 0)
	{
		char slovoSPB[15];
		memset(slovoSPB, 0, 10);
		if (size == 5)
		{
			strcpy(slovoSPB, "�����");
			add_to_list("�����");
		}
		if (size == 7)
		{
			strcpy(slovoSPB, "�������");
			add_to_list("�������");
		}
		if (size == 10)
		{
			strcpy(slovoSPB, "����������");
			add_to_list("����������");
		}
		int idx = 0;
		y = (con_height_new() - size * 3) / 2;
		if (size == 5 || size == 7)
		{
			for (int i = 0; i < size; i++)
			{
				gotoxy(x + 2 + 6 * i, y + 3 * (size / 2) + 1);
				fprintf(stdout, "%c", slovoSPB[idx++]);
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				gotoxy(x + 2 + 6 * i, y + 3 * i + 1);
				fprintf(stdout, "%c", slovoSPB[idx++]);
			}
		}
	}
	else
	{
		y = (con_height_new() - size * 3) / 2 + 1;
		for (int i = 1; i < size + 1; i++, y += 3)
		{
			for (int j = 1; j < size + 1; j++)
			{
				gotoxy(x + 2 + 6 * (j - 1), y);
				fprintf(stdout, "%c", field[i][j].letter);
			}
		}
		added* current_ = list->next;
		x = 11, y = 3;
		while (current_ != NULL)
		{
			gotoxy(x, y);
			y++;
			fprintf(stdout, "%s", current_->word);
			current_ = current_->next;
		}
	}

	gotoxy(con_width_new() - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");
	if (size == 10)
	{
		gotoxy(con_width_new() - strlen("��������� �����") - 6, 0);
		fprintf(stdout, "��������� �����");
	}
	else
	{
		gotoxy((con_width_new() - strlen("��������� �����")) / 2 + 2, 1);
		fprintf(stdout, "��������� �����");
	}

	if (!comp)
	{
		gotoxy(4, 0);
		fprintf(stdout, "  �����  | ��������� ");
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
	}
	else
	{
		gotoxy(4, 0);
		fprintf(stdout, "  �����  |  �����  ");
		gotoxy(4, 1);
		if (point_1 / 10 < 1)
			fprintf(stdout, "    %d    |     %d     ", point_1, point_2);
		else if (point_1 / 100 < 1)
			fprintf(stdout, "    %d   |     %d     ", point_1, point_2);
		else if (point_1 / 1000 < 1)
			fprintf(stdout, "    %d  |     %d     ", point_1, point_2);
	}

	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "�������");

	con_draw_release();

	map* coordinates = (map*)malloc(sizeof(map));
	map* new_coor = (map*)malloc(sizeof(map));
	if (coordinates != NULL && new_coor != NULL)
	{
		memset(coordinates, 0, sizeof(map));
		memset(new_coor, 0, sizeof(map));
	}
	else
		exit(1);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if ((code > 223 && code < 256 || code == 480) && last_stand == 0)
			{
				if (field[coordinates->y + 1][coordinates->x + 1].letter == ' ')
				{
					gotoxy((con_width_new() - size * 5) / 2 + 6 * coordinates->x + 2,
						(con_height_new() - size * 3) / 2 + 3 * coordinates->y + 1);
					fprintf(stdout, "%c", code);
					last_stand++;
					field[coordinates->y + 1][coordinates->x + 1].letter = code;
					new_coor->x = coordinates->x;
					new_coor->y = coordinates->y;
				}
			}
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				/*���������?*/
				save_your_tears(size, field);

				point_1 = 0, point_2 = 0, move = 0, word_y = 3;
				list->next = NULL;
				list->word[0] = '\0';
				free(field);
				return 1;
			}
			if (code == KEY_UP)
			{
				if (coordinates->y == 0 && (size == 5 || size == 7))
				{
					if (test(selected, &idx_1, field, size, new_coor) == 1)
						return 1;
				}

				if (coordinates->y > 0)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 1;
					coordinates->y--;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_RIGHT)
			{

				if (coordinates->x == size - 1 && (size == 10))
				{
					if (test(selected, &idx_1, field, size, new_coor) == 1)
						return 1;
				}

				if (coordinates->x < size - 1)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 1;
					coordinates->x++;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_DOWN)
			{
				if (coordinates->y < size - 1)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else
						prev = 0;
					coordinates->y++;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_LEFT)
			{
				if (coordinates->x == 0)
				{
					if (give_up())
					{
						point_1 = 0, point_2 = 0, move = 0, word_y = 3;
						list->next = NULL;
						list->word[0] = '\0';
						free(field);
						return 1;
					}
				}

				if (coordinates->x > 0)
				{
					if (prev == 0)
						change_pos(coordinates, 0, size);
					else prev = 0;
					coordinates->x--;
					change_pos(coordinates, 1, size);
					con_set_color(FONT, CON_CLR_BLACK);
				}
			}
			if (code == KEY_ENTER)
			{
					/*�������� ������� �� ������*/
				int result = check(selected, coordinates, field, idx_1 - 1);
				if (!result)
					/*������ �� �������*/
					break;

				if (result == 2)
				{
					select_(coordinates, size, field, 1);
					idx_1--;
					break;
				}

				selected[idx_1].x = coordinates->x;
				selected[idx_1++].y = coordinates->y;

				select_(coordinates, size, field, 0);
			}
			if (code == KEY_BACK)
			{
				if (!last_stand)
					break;
				if (coordinates->x == new_coor->x && coordinates->y == new_coor->y)
				{
					delete_red(selected, idx_1, size, field);
					idx_1 = 0;
					field[coordinates->y + 1][coordinates->x + 1].letter = ' ';
					gotoxy((con_width_new() - size * 5) / 2 + 6 * coordinates->x + 2,
						(con_height_new() - size * 3) / 2 + 3 * coordinates->y + 1);
					fprintf(stdout, " ");
					last_stand--;
				}
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}
}

/*����� ������������� ���������� ����*/
int game_parametres()
{
	clrscr();
	con_draw_lock();

	short active = 0, size = 0;
	char* vars[] = { "�����", "������", "������" ,"5","7","10",
		"��� ������", "������ ����������" };

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	gotoxy(x - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");
	gotoxy(2, con_height_new() - 2);
	fprintf(stdout, "*��� ������ ����������� ������� ������ ��� �����");
	gotoxy((x - strlen("�������� ������� ���������")) / 2, y);
	fprintf(stdout, "�������� ������� ���������");
	gotoxy((x - strlen(vars[0])) / 2, y + 2);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[0]);
	con_draw_release();

	/*���������� ��� ���� � �����*/
	int field_size = 0;
	cell** field = NULL;

	short go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return 0;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 2)
				{
					active++;
					change_lvl(active, vars, 1, 0);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 0)
				{
					active--;
					change_lvl(active, vars, 0, 0);
				}
			}
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				/*������� ���������*/
				lvl = active + 1;
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_draw_lock();
	
	gotoxy((x - strlen("�������� ������ ���������")) / 2, y + 4);
	fprintf(stdout, "�������� ������ ���������");
	gotoxy((x - strlen(vars[3])) / 2, y + 6);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[3]);
	active = 3;
	con_draw_release();

	go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 5)
				{
					active++;
					change_lvl(active, vars, 1, 1);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 3)
				{
					active--;
					change_lvl(active, vars, 0, 1);
				}
			}
			if (code == KEY_ENTER)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				if (active == 3)
					size = 5;
				if (active == 4)
					size = 7;
				if (active == 5)
					size = 10;

				field_size = size;
				field = create_FIELD(&field_size);
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_draw_lock();

	gotoxy((x - strlen("�������� ����� ����")) / 2, y + 8);
	fprintf(stdout, "�������� ����� ����");
	gotoxy((x - strlen(vars[6])) / 2, y + 10);
	con_set_color(FONT, CON_CLR_GRAY);
	fprintf(stdout, "%s", vars[6]);
	active = 6;
	con_draw_release();

	go = true_;
	while (go)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			if (code == KEY_ESC)
			{
				con_set_color(FONT, CON_CLR_BLACK);
				return;
			}
			if (code == KEY_RIGHT)
			{
				if (active < 7)
				{
					active++;
					change_lvl(active, vars, 1, 2);
				}
			}
			if (code == KEY_LEFT)
			{
				if (active > 6)
				{
					active--;
					change_lvl(active, vars, 0, 2);
				}
			}
			if (code == KEY_ENTER)
			{
				if (active == 6)
					comp = 1;
				else
					comp = 0;
				con_set_color(FONT, CON_CLR_BLACK);
				go = false_;
				break;
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

	con_set_color(FONT, CON_CLR_BLACK);
	if (start_game(size, field, 0))
		return 1;
}

/*�������� ����������� ����*/
void dwnld()
{
	if (!file_size())
	{
		clrscr();
		gotoxy((con_width_new() - strlen("��� ����������� ���!")) / 2, con_height_new() / 2 - 7);
		fprintf(stdout, "��� ����������� ���!");
		pause(3000);
		fclose(saved);
		return;
	}

	char num[30];
	if (num != NULL)
		memset(num, 0, 30);
	else
		exit(1);
	/*������ ���� � ������� ���������*/
	fgets(num, 30, saved);
	int field_size = 0, stop = 0;
	for (int i = 0; i < 10; i++)
	{
		if (num[i] == ' ')
		{
			stop = i + 1;
			break;
		}
		field_size = field_size * pow(10, i) + num[i] - '0';
	}
	lvl = num[stop] - '0';

	comp = num[stop + 2] - '0';

	word_y = num[stop + 4] - '0';

	move = num[stop + 6] - '0';

	num[0] = '\0';
	fgets(num, 30, saved);

	for (int i = 0; i < 10; i++)
	{
		if (num[i] == ' ')
		{
			stop = i + 1;
			break;
		}
		point_1 = point_1 * pow(10, i) + num[i] - '0';
	}

	int new_i = 0;
	while (num[stop] != '\n')
	{
		point_2 = point_2 * pow(10, new_i++) + num[stop++] - '0';
	}

	num[0] = '\0';

	cell** field = create_FIELD(&field_size);

	for (int i = 1; i <= field_size; i++)
	{
		for (int j = 1; j <= field_size; j++)
		{
			if (fscanf(saved, "%c", &field[i][j].letter) != 1)
				exit(1);
		}
		/*������������� \n*/
		fseek(saved, 2, SEEK_CUR);
	}

	while (fgets(num, 30, saved) != NULL)
	{
		if (num[strlen(num) - 1] == '\n')
			num[strlen(num) - 1] = '\0';
		add_to_list(num);
	}

	fclose(saved);

	start_game(field_size, field, 1);
}

/*����� ���� ����*/
void game_move()
{
	char* vars[] = { "����� ����", "��������� ����" };
	short active_button = 0;
	menu_game(active_button, vars);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{
			code = key_pressed_code();
			/*�����*/
			if (code == KEY_ESC)
			{
				return;
			}
			if (code == KEY_UP)
			{
				if (active_button > 0)
				{
					changer(active_button, vars, 0);
					active_button--;
					changer(active_button, vars, 1);
					break;
				}
			}
			if (code == KEY_DOWN)
			{
				if (active_button < 1)
				{
					changer(active_button, vars, 0);
					active_button++;
					changer(active_button, vars, 1);
					break;
				}
			}
			/*������� ���������*/
			if (code == KEY_ENTER)
			{
				/*����� ������� ����*/
				switch (active_button)
				{
				case(0):
					if (game_parametres() == 1)
						return;
					menu_game(active_button, vars);
					break;
				case(1):
					dwnld();
					menu_game(active_button, vars);
					break;
				}
			}
		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}
}

/*������� <-> ����������*/
void reference()
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() /2 - 14;
	gotoxy(4, y); //(x - strlen("����������")) / 2
	y += 2;
	con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	fprintf(stdout, "����������");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[1] ����������� �� ���� ����� - [KEY_UP].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[2] ����������� �� ���� ������ - [KEY_RIGHT].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[3] ����������� �� ���� ���� - [KEY_DOWN].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[4] ����������� �� ���� ����� - [KEY_LEFT].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[5] ��������� ����� - [KEY_ENTER].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[6] ������ ��������� - [KEY_ENTER]+[KEY_ENTER].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[7] ������ ������������ ����� - [KEY_BACK].");
	gotoxy(4, y);
	y += 2;
	fprintf(stdout, "[8] ��������� ����� - ����� ����� �� ���������.");
	

	gotoxy(x - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*���������� ���� � ������� + �����������*/
void settings(FILE* dict)
{
	clrscr();
	con_draw_lock();

	int x = con_width_new(), y = con_height_new() / 2 - 7;
	con_set_color(FONT, CON_CLR_GRAY);
	gotoxy((x - strlen("�������� ����� ����� � �������")) / 2, y);
	fprintf(stdout, "�������� ����� ����� � �������");
	con_set_color(FONT, CON_CLR_BLACK);
	gotoxy((x - strlen("�������� ������� ��������� �������� � ������� \"����� ����\"")) / 2, y + 5);
	fprintf(stdout, "�������� ������� ��������� �������� � ������� \"����� ����\"");
	gotoxy(x - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");

	con_draw_release();
	char new_word[30];
	if (new_word != NULL)
		memset(new_word, 0, 30);
	
	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
			if (code == KEY_ENTER)
			{
				show_cursor(1);
				gotoxy((x - strlen("�������� ����� ����� � �������")) / 2, y + 2);
				if (fscanf(stdin, "%25s", new_word) != 1)
					exit(1);
				insert(new_word, root);
				fprintf(dict, "\n%s", new_word);
				new_word[0] = '\0';
				gotoxy((x - strlen("�������� ����� ����� � �������")) / 2, y + 2);
				fprintf(stdout, "                                                                  ");
				gotoxy((x - strlen("�������!")) / 2, y + 2);
				fprintf(stdout, "�������!");
				pause(3000);
				gotoxy((x - strlen("�������� ����� ����� � �������")) / 2, y + 2);
				fprintf(stdout, "                                                                  ");
				show_cursor(0);
			}
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*������� � ������ ��������*/
void ins_rec(int num, char* wrd, vsOv** rt)
{
	vsOv* node, * current = *rt, * prev = NULL;
	node = (vsOv*)malloc(sizeof(vsOv));
	if (node == NULL)
		exit(1);
	memset(node, 0, sizeof(vsOv));
	node->points = num;
	strcpy(node->name, wrd);
	node->left = NULL;
	node->right = NULL;

	while (current != NULL)
	{
		prev = current;
		current = current->points < node->points ? current->right : current->left;
	}

	/*�������*/
	if (prev == NULL)
	{
		*rt = node;
		return;
	}
	else
	{
		current = node;
		if (prev->points > node->points)
			prev->left = node;
		else
			prev->right = node;
	}
	return;
}

/*����� ������*/
void pr_nodes(vsOv* rt, int x, int* y, int* nums)
{
	int done = 0;
	if (rt->right == NULL)
	{
		gotoxy(x, *y);
		fprintf(stdout, "[%d] %s - %d", *nums, rt->name, rt->points);
		*y = *y + 2;
		*nums = *nums + 1;
		done++;
	}
	else 
		pr_nodes(rt->right, 4, y, nums);

	if (rt->left != NULL)
		pr_nodes(rt->left, 4, y, nums);
	else
	{
		if (done)
			return;
		gotoxy(x, *y);
		fprintf(stdout, "[%d] %s - %d", *nums, rt->name, rt->points);
		*y = *y + 2;
		*nums = *nums + 1;
	}
}

/*����������� ����� �������� � �������*/
void show_r(vsOv* rt)
{
	clrscr();
	con_draw_lock();
	int x = con_width_new(), y = 2;
	int nums = 1;


	if (rt == NULL)
	{
		gotoxy((x - strlen("������� �����!")) / 2, y);
		fprintf(stdout, "������� �����!");
		con_draw_release();
		pause(4000);
		return;
	}

	gotoxy(4, y);
	con_set_color(CON_CLR_BLUE, CON_CLR_BLACK);
	fprintf(stdout, "������� ��������");
	con_set_color(FONT, CON_CLR_BLACK);
	y += 2;
	pr_nodes(rt, 4, &y, &nums);

	gotoxy(x - strlen("��� ������ ������� ESC") - 2, con_height_new() - 2);
	fprintf(stdout, "��� ������ ������� ESC");

	con_draw_release();

	while (true_)
	{
		while (!key_is_pressed())
		{
			short code = key_pressed_code();
			if (code == KEY_ESC)
				return;
		}
		while (key_is_pressed())
			key_pressed_code();
	}
}

/*�������� ������*/
void clr_tr(vsOv* rt)
{
	if (rt == NULL)
		return;
	if (rt->left == NULL && rt->right == NULL)
	{
		free(rt);
		return;
	}
	clr_tr(rt->left);
	clr_tr(rt->right);
}

/*������� ��������*/
void rcrds_show()
{
	records = fopen(REC, "r");
	vsOv* rc_root = NULL;
	char wrd[30], final_[30];
	if (wrd == NULL || final_ == NULL)
		exit(1);
	memset(wrd, 0, 30);
	memset(final_, 0, 30);

	int num = 0;
	while (fgets(wrd, 30, records) != NULL && wrd[0] != '\n')
	{
		int idx = 0, idx_1 = 0;
		while (wrd[idx] != ' ')
		{
			num = num * 10 + (wrd[idx] - '0');
			idx++;
		}
		idx++;
		while (wrd[idx] != '\0' && wrd[idx] != '\n')
		{
			final_[idx_1] = wrd[idx];
			idx++;
			idx_1++;
		}
		final_[idx_1] = '\0';
		ins_rec(num, final_, &rc_root);
		num = 0;
	}

	show_r(rc_root);

	/*������� ������ + �������� �����*/
	clr_tr(rc_root);
	fclose(records);
}
 
/*�������� ��������� �� ��������*/
void menu_move(FILE* dict)
{
	char* vars[] = { "������", "�������", "����������", "�������", "� ���������", "�����" };
	short active_button = 0;

	main_menu(vars, active_button);

	while (true_)
	{
		short code = 0;
		while (!key_is_pressed())
		{

			code = key_pressed_code();
			if (code == KEY_UP)
			{
				if (active_button > 0)
				{
					changer(active_button, vars, 0);
					active_button--;
					changer(active_button, vars, 1);
					break;
				}
			}
			if (code == KEY_DOWN)
			{
				if (active_button < 5)
				{
					changer(active_button, vars, 0);
					active_button++;
					changer(active_button, vars, 1);
					break;
				}
			}
			/*������� ���������*/
			if (code == KEY_ENTER)
			{
				/*����� ������� ����*/
				switch (active_button)
				{
				case(0):
					game_move();
					main_menu(vars, active_button);
					break;
				case(1):
					rcrds_show();
					main_menu(vars, active_button);
					break;
				case(2):
					settings(dict);
					main_menu(vars, active_button);
					break;
				case(3):
					reference();
					main_menu(vars, active_button);
					break;
				case(4):
					about();
					main_menu(vars, active_button);
					break;
				case(5):
					fprintf(stdout, "\n\n\n\n\n");
					return;
				}
			}

		}

		while (key_is_pressed())
			key_pressed_code();

		pause(40);
	}

}

int main()
{
	/*������������ ������� + ��������� ��������� 1251*/
	con_init(80,25);
	/*������� �������*/
	show_cursor(0);

	FILE* dict, * inv;

	/*�������� ������ � ��������*/
	create_FILES(&dict, &inv, &records);
	create_TRIES();

	/*�������� ���� ��������*/
	dict_read(root, dict);
	dict_read(root_inv, inv);

	/*����������� ����*/
	menu_move(dict);
	
	/*�������� ������-��������*/
	fclose(dict);
	fclose(inv);

	return EXIT_SUCCESS;
}