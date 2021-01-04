#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_score.h"
#include "blank.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM];
char id_table[SNUM][10];

char stuDir[BUFLEN];
char ansDir[BUFLEN];
char errorDir[BUFLEN];
char threadFiles[ARGNUM][FILELEN];
char cIDs[ARGNUM][FILELEN];

int mOption = false; //ä�� ���� ���ϴ� ���� ���� ����
int eOption = false; //error.txt�� ���� �޽��� ���
int tOption = false; //������ �� -lpthread �ɼ� �߰�
int iOption = false; //�ش� �л����� Ʋ�� ���� ���� ���


/**
�� �� : ssu_score(int argc, char *argv[])
�� �� : ���� ������ �������� �л����� ������ ����� ä��
*/
void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){ //���ڷ� ���� ���ڿ��� "-h"�� ��ġ�ϸ�
			print_usage(); //���� ���
			return;
		}
	}

	memset(saved_path, 0, BUFLEN); //�迭 0���� �ʱ�ȭ
	if(argc >= 3 && strcmp(argv[1], "-i") != 0){ //���ڰ� ����� ���� ���
		strcpy(stuDir, argv[1]); //ù��° ���ڸ� stuDir�� ����
		strcpy(ansDir, argv[2]); //�ι�° ���ڸ� ansDir�� ����
	}

	if(!check_option(argc, argv)) //���ڷ� � �ɼ��� �־������� Ȯ��
		exit(1);

	if(!mOption && !eOption && !tOption && iOption){ //"-i" �ɼ��� ���
		do_iOption(cIDs); //�ش� �л����� Ʋ�� ���� ���� ���
		return;
	}

	getcwd(saved_path, BUFLEN); //���� �۾� ���丮�� saved_path�� ����

	if(chdir(stuDir) < 0){ //�۾� ���丮�� stuDir�� ����
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); //���� �۾� ���丮�� stuDir�� ����

	chdir(saved_path); //�۾� ���丮�� saved_path�� ����
	if(chdir(ansDir) < 0){ //�۾� ���丮�� ansDir�� ����
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); //���� �۾� ���丮�� ansDir�� ����

	chdir(saved_path); //�۾� ���丮�� saved_path�� ����

	set_scoreTable(ansDir); //���� ���̺� ����
	set_idTable(stuDir); //id ���̺� ����

	printf("grading student's test papers..\n");
	score_students(); //�л����� ���� ���

	if(iOption) { //-i �ɼ��� �����Ǿ� �ִ� ���
		do_iOption(cIDs); //�ش� �л����� Ʋ�� ���� ���� ���
	}

	return;
}


/**
�� �� : check_option(int argc, char *argv[])
�� �� : ����� ���ڷ� ���� �ɼ��� �������� Ȯ��
*/
int check_option(int argc, char *argv[])
{
	int i, j;
	int c;

	/* ����� ���� �м� */
	while((c = getopt(argc, argv, "e:thmi")) != -1)
	{
		switch(c){
			case 'e': //���ڷ� -e �ɼ� ���� ���
				eOption = true;
				strcpy(errorDir, optarg); //optarg�� errorDir�� ����

				if(access(errorDir, F_OK) < 0) //errorDir ���� ���� ����
					mkdir(errorDir, 0755); //errorDir ����
				else{
					rmdirs(errorDir); //errorDir ����
					mkdir(errorDir, 0755); //�ٽ� errorDir ����
				}
				break;

			case 't': //���ڷ� -t �ɼ� ���� ���
				tOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM) //�ִ� �������� ���� �ʰ�
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //����� ���ڸ� threadFiles�� ����
					i++; 
					j++;
				}
				break;

		//	case 'm': //���ڷ� -m �ɼ� ���� ���
				//����
			
			case 'i': //���ڷ� -i �ɼ� ���� ���
				iOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM) //�ִ� �������� ���� �ʰ�
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //����� ���ڸ� cIDs�� ����
					i++; 
					j++;
				}
				break;

			case '?': //���ڷ� ? ���� ���
				printf("Unkown option %c\n", optopt); //�������� ���� �ɼ�
				return false;
		}
	}

	return true;
}


/*
void do_cOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	int i = 0;
	char *p, *saved;

	if((fp = fopen("score.csv", "r")) == NULL){
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}

	fscanf(fp, "%s\n", tmp);

	while(fscanf(fp, "%s\n", tmp) != EOF)
	{
		p = strtok(tmp, ",");

		if(!is_exist(ids, tmp))
			continue;

		printf("%s's score : ", tmp);

		while((p = strtok(NULL, ",")) != NULL)
			saved = p;

		printf("%s\n", saved);
	}
	fclose(fp);
}
*/



/**
�� �� : do_iOption(char (*ids)[FILELEN])
�� �� : "-i" �ɼ� ���ൿ���� ���ڷ� ���� �л����� Ʋ�� ���� ���� ���
*/
void do_iOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	int i = 0;
	char *p, *saved;

	if((fp = fopen("score.csv", "r")) == NULL){ //���� �б��������� ����
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}

	fscanf(fp, "%s\n", tmp); //���µ� ���Ϸκ��� tmp ����
	

	/* ���� ������ ���� */
	while(fscanf(fp, "%s\n", tmp) != EOF)
	{
		p = strtok(tmp, ","); //","�� �������� ���ڿ� �ڸ�
		
		if(!is_exist(ids, tmp)) //�ش� �л��� id�� tmp�� �������� �ʴ� ���
			continue; //���� �ǳʶ�

		printf("%s's wrong answer : ", tmp);
	
		/*  �ڸ� ���ڿ��� NULL �־��� */
		while((p = strtok(NULL, ",")) != NULL) {
			if(!strcmp(p, "0")) { //p�� ����Ű�� ������ 0���� ���
				saved = p;
				printf("%s\n", saved); //0���� ���� ���
			}
		} 
	}

	fclose(fp);
}


int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}


/**
�� �� : set_scoreTable(char *ansDir)
�� �� : ���� ���̺� ����
*/
void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	sprintf(filename, "%s", "score_table.csv"); //filename�� �־��� ���´�� ����

	if(access(filename, F_OK) == 0) //���� ���翩�� ����
		read_scoreTable(filename); //������ �о� score_table�� qname, score ���·� ����
	else{
		make_scoreTable(ansDir); //�Է¹��� ������� score_table�� qname, score ���·� ����
		write_scoreTable(filename); //scoreTable�� ����� �����͸� ���� ������ ���Ͽ� write
	}
}

void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		strcpy(score_table[idx].qname, qname);
		score_table[idx++].score = atof(score);
	}

	fclose(fp);
}


/**
�� �� : make_scoreTable(char *ansDir)
�� �� : ������ �������� �ʴ� ���, 2������ type���� ������ ������ score�� �Է¹ް�, score_table�� qname, score ���·� ����
*/
void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int idx = 0;
	int i;

	num = get_create_type(); //���� ��ȣ�� ������ � ���·� �Է¹����� ����

	if(num == 1) //��ĭ ������ ���α׷� ������ ������ �Է¹޴� ���
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore); //��ĭ ���� score
		printf("Input value of program question : ");
		scanf("%lf", &pscore); //���α׷� ���� score
	}

	if((dp = opendir(ansDir)) == NULL){ //DIR ����ü ���, ���� ����ü �ʱ�ȭ
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}	

	/*  DIR ����ü�� ���� ���� */
	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //���丮 �̸��� "."�̰ų� ".."�� ��쿡
			continue; //���� �ǳʶ�

		if((type = get_file_type(dirp->d_name)) < 0) //������ Ÿ�� ������ type ������ ����
			continue;

		strcpy(score_table[idx++].qname, dirp->d_name); //dirp�� ����� ���丮 �̸��� score_table�� qname ����� ����

	}

	closedir(dp); //dp ����ü�� ���丮 close
	sort_scoreTable(idx); //scoreTable�� ���� ��ȣ�� ������������ ����

	for(i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname); //������ Ÿ�� ������ type ������ ����

		if(num == 1) //score�� ��ĭ ������ ���α׷� ������ ������ �Է¹޴� ���
		{
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2) //��� ������ score�� �Է¹޴� ���
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score; //score_table�� score ������ �Է¹��� ���� ����
	}
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = sizeof(score_table) / sizeof(score_table[0]);

	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	for(i = 0; i < num; i++)
	{
		if(score_table[i].score == 0)
			break;

		sprintf(tmp, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp));
	}

	close(fd);
}


void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	while((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		sprintf(tmp, "%s/%s", stuDir, dirp->d_name);
		stat(tmp, &statbuf);

		if(S_ISDIR(statbuf.st_mode))
			strcpy(id_table[num++], dirp->d_name);
		else
			continue;
	}

	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){

			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);


			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){

				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-."));
	
	p = strtok(NULL, "-.");
	if(p == NULL)
		*num2 = 0;
	else
		*num2 = atoi(p);
}

int get_create_type()
{
	int num;

	while(1)
	{
		printf("score_table.csv file doesn't exist in TREUDIR!\n");
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		if(num != 1 && num != 2)
			printf("not correct number!\n");
		else
			break;
	}

	return num;
}


/**
�� �� : score_students()
�� �� : �л����� score ���
*/
void score_students()
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);

	if((fd = creat("score.csv", 0666)) < 0){ //"score.csv" �̸��� ���� ����
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); //���� ��ȣ�� ������ sum�� ���¸� �������� �л����� ���� ���̺��� ù��° row ����

	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], ""))
			break;

		sprintf(tmp, "%s,", id_table[num]); //num���� ī��Ʈ�Ǵ� id_table�� ��Ҹ� tmp�� ���
		write(fd, tmp, strlen(tmp));  //tmp���� �ش� ũ�⸸ŭ�� fd�� �ǹ��ϴ� ���Ͽ� write

		score += score_student(fd, id_table[num]); //�л��� score�� ���ڷ� �޾� ���� ����
	}

	printf("Total average : %.2f\n", score / num); //��ü ��� ���

	close(fd);
}


/**
�� �� : score_student(int fd, char *id)
�� �� : �л��� �й��� ���ڷ� �޾� score ���
*/
double score_student(int fd, char *id)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0) //score_table�� ������ 0�� ���
			break;

		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname); //stuDir, id, qname�� �־��� ���·� tmp�� ���

		if(access(tmp, F_OK) < 0) //tmp�� ���� ���� ���� ����
			result = false;
		else
		{
			if((type = get_file_type(score_table[i].qname)) < 0) //������ Ÿ�� ������ type ������ ����
				continue;
			
			if(type == TEXTFILE) //������ TEXTFILE�� ���
				result = score_blank(id, score_table[i].qname); //��ĭ ���� ä��
			else if(type == CFILE) //������ CFILE�� ���
				result = score_program(id, score_table[i].qname); //���α׷� ���� ä��
		}

		if(result == false) //�л� ����� ������ �ƴ� ���
			write(fd, "0,", 2); //fd�� �ǹ��ϴ� ���Ͽ� 0���� �ǹ��ϴ� "0," �Է�
		else{
			if(result == true){ //�л� ����� ������ ���
				score += score_table[i].score; //���� ����
				sprintf(tmp, "%.2f,", score_table[i].score); //������ �Ҽ� ��°�ڸ����� tmp�� ����
			}
			else if(result < 0){ //�л� ����� ���������� warning�� �߻��� ���
				score = score + score_table[i].score + result; //�ش� ���� ����
				sprintf(tmp, "%.2f,", score_table[i].score + result); //������ �Ҽ� ��°�ڸ����� tmp�� ����
			}
			write(fd, tmp, strlen(tmp)); //������ ä�� ��� ���̺� write
		}
	}

	printf("%s is finished.. score : %.2f\n", id, score);  //ä���� �����ϸ� �л����� ���� ���

	sprintf(tmp, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	write(fd, ",", 1);

	for(i = 0; i < size; i++){
		if(score_table[i].score == 0)
			break;
		
		sprintf(tmp, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	memset(result, 0, BUFLEN);
	while(read(fd, &c, 1) > 0)
	{
		if(c == ':')
			break;
		
		result[idx++] = c;
	}
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}


/**
�� �� : score_blank(char *id, char *filename)
�� �� : �־��� ������ ��ĭ ������ ���, �л��� ������ txt ���� ä��
*/
int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[BUFLEN];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	memset(qname, 0, sizeof(qname)); //qname 0���� �ʱ�ȭ
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); //qname�� filename ����

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename); //tmp�� ������ ���� ���·� ����
	fd_std = open(tmp, O_RDONLY); //tmp�� �б� �������� ����
	strcpy(s_answer, get_answer(fd_std, s_answer)); //���� ������ s_answer�� ����

	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	if(!check_brackets(s_answer)){ //��ȣ�� ¦�� �´��� �˻�
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer))); //���ڿ� ��������

	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	/* �л��� ������ txt ������ �о�鿩 ��ū �迭�� ���� */ 
	if(!make_tokens(s_answer, tokens)){
		close(fd_std); //���� close
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0); //��ū���� ������ �켱 ������ ���� Ʈ�� ����

	sprintf(tmp, "%s/%s", ansDir, filename); //tmp�� ������ ���� ���·� ����
	fd_ans = open(tmp, O_RDONLY); //tmp�� �б� �������� ����

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx])); //�޸� �ʱ�ȭ

		strcpy(a_answer, get_answer(fd_ans, a_answer)); //���� ������ a_answer�� ����

		if(!strcmp(a_answer, ""))
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer))); //���ڿ� ��������

		if(has_semicolon == false){
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		else if(has_semicolon == true)
		{
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		/* ���� txt ������ �о�鿩 ��ū �迭�� ���� */
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0); //��ū���� ������ �켱 ������ ���� Ʈ�� ����

		compare_tree(std_root, ans_root, &result); //�л� ������� ���� Ʈ���� �������� ���� Ʈ���� ���Ͽ� ���� Ȯ��

		if(result == true){ //������ ���
			close(fd_std); //���� close
			close(fd_ans); //���� close

			if(std_root != NULL) //�л� ������� ���� Ʈ���� �޸� ����
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root); //�������� ���� Ʈ���� �޸� ����
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root); //�л� ������� ���� Ʈ���� �޸� ����
	if(ans_root != NULL)
		free_node(ans_root); //�������� ���� Ʈ���� �޸� ����

	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	compile = compile_program(id, filename);

	if(compile == ERROR || compile == false)
		return false;
	
	result = execute_program(id, filename);

	if(!result)
		return false;

	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);

	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}


/**
�� �� : compile_program(char *id, char *filename)
�� �� : �л��� ������� ������ ���α׷� ������
*/
double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[BUFLEN], tmp_e[BUFLEN];
	char command[BUFLEN];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	memset(qname, 0, sizeof(qname)); //�迭 0���� �ʱ�ȭ
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); //filename�� �ش� ���̸�ŭ qname�� ����
	
	isthread = is_thread(qname); //qname�� �����帧�� �ִ��� Ȯ��

	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread) //������ �� ��, -t �ɼ����� -lpthrad �ɼ� �߰� �� ���
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666); //tmp_e ���� ����

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END); //���� ũ�� ����
	close(fd);
	unlink(tmp_e); //tmp_e�� ���� ��ũ ����

	if(size > 0)
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread) //������ �� ��, -t �ɼ����� -lpthrad �ɼ� �߰� �� ���
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666); //tmp_f ���� ����

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END); //���� ũ�� ����
	close(fd); //���� close

	if(size > 0){
		if(eOption) //-e�ɼ��� ������ ���
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0) //���� ���� ���� ����
				mkdir(tmp_e, 0755); //tmp_e ���丮 ����

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e); //������ tmp_f�� �̸��� tmp_e�� ����

			result = check_error_warning(tmp_e); //tmp_e�� ����üũ
		}
		else{ 
			result = check_error_warning(tmp_f); //tmp_f�� ����üũ
			unlink(tmp_f); //tmp_f�� ���� ��ũ ����
		}

		return result;
	}

	unlink(tmp_f); //tmp_f�� ���� ��ũ ����
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	while(fscanf(fp, "%s", tmp) > 0){
		if(!strcmp(tmp, "error:"))
			return ERROR;
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	return warning;
}


/**
�� �� : execute_program(char *id, char *filename)
�� �� : �л��� ������� ������ ���α׷� ����
*/
int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname)); //�迭 �ʱ�ȭ
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname); //�־��� ���´�� ans_fname�� ���
	fd = creat(ans_fname, 0666); //���� ����

	sprintf(tmp, "%s/%s.exe", ansDir, qname); //�־��� ���´�� tmp�� ���
	redirection(tmp, fd, STDOUT);
	close(fd); //���� close

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname); //�־��� ���´�� std_fname�� ���
	fd = creat(std_fname, 0666); //���� ����

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname); //�־��� ���´�� tmp�� ���

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL); //pid�� �̿��� ���μ��� ����
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname); //�л� ���α׷��� ���� ���α׷��� ���� ä�� ��� ����
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	sprintf(command, "ps | grep %s", name);
	redirection(command, fd, STDOUT);

	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	pid = atoi(strtok(tmp, " "));
	close(fd);

	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		if(len1 == 0 && len2 == 0)
			break;

		to_lower_case(&c1);
		to_lower_case(&c2);

		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	saved = dup(old);
	dup2(new, old);

	system(command);

	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.');

	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	else if (!strcmp(extension, ".c"))
		return CFILE;
	else
		return -1;
}


/**
�� �� : rmdirs(const char *path)
�� �� : ���丮�� ������ �ϳ��� �����ϸ� ���������� ���丮 ����
*/
void rmdirs(const char *path)
{
	struct dirent *dirp; //���丮 ���� ��Ÿ���� ����ü
	struct stat statbuf;
	DIR *dp;
	char tmp[BUFLEN];
	
	if((dp = opendir(path)) == NULL) //���ڷ� ���� ���丮 ����
		return;

	/* ���丮 ���� ������ read */
	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //���丮 �̸��� "."�̰ų� ".."�̶��
			continue; //���� �ǳʶ�

		sprintf(tmp, "%s/%s", path, dirp->d_name); //path�� ���丮 �̸��� tmp �迭�� ���

		if(lstat(tmp, &statbuf) == -1) //���� ���� Ȯ��
			continue;

		if(S_ISDIR(statbuf.st_mode)) //������ ������ ���丮���
			rmdirs(tmp); //���� ������� ȣ��
		else
			unlink(tmp); //tmp�� ��ũ ����
	}

	closedir(dp); //���丮 close
	rmdir(path); //��� ������� ȣ��
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}


/**
�� �� : print_usage()
�� �� : ���α׷��� ���� ���
*/
void print_usage()
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n");
	printf("Option : \n");
	printf(" -m			modify question's score\n");
	printf(" -e <DIRNAME>		print error on 'DIRNAME/ID/qname_error.txt' file \n");
	printf(" -t <QNAMES>		compile QNAME.C with -lpthread option\n");
	printf(" -i <IDS>		print ID's wrong questions\n");
	printf(" -h			print usage\n");
}
