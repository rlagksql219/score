#ifndef MAIN_H_
#define MAIN_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1
#endif
#ifndef STDERR
	#define STDERR 2
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3
#endif
#ifndef CFILE
	#define CFILE 4
#endif
#ifndef OVER
	#define OVER 5
#endif
#ifndef WARNING
	#define WARNING -0.1
#endif
#ifndef ERROR
	#define ERROR 0
#endif

#define FILELEN 64
#define BUFLEN 1024
#define SNUM 100
#define QNUM 100
#define ARGNUM 5

struct ssu_scoreTable{ /* ���� ���̺� ����ü */
	char qname[FILELEN]; //���� ��ȣ
	double score; //����
};

void ssu_score(int argc, char *argv[]); /* ���� ������ �������� �л����� ������ ��� ä�� */
int check_option(int argc, char *argv[]); /* ����� ���ڷ� ���� �ɼ��� �������� Ȯ�� */
void print_usage(); /* ���α׷� ���� ��� */

void score_students(); /* �л����� score ��� */
double score_student(int fd, char *id); /* �л��� �й��� ���ڷ� �޾� score ��� */
void write_first_row(int fd); /* �л����� ������ ����ϴ� table�� ù��° row ���� */

char *get_answer(int fd, char *result); /* ���Ͽ��� ���� ������ �о� �迭 ���·� ���� */
int score_blank(char *id, char *filename); /* �л��� ������ txt ���� ä�� */
double score_program(char *id, char *filename); /* �л��� ������ ���α׷� ä�� */
double compile_program(char *id, char *filename); /* �л��� ������� ������ ���α׷� ������ */
int execute_program(char *id, char *filname); /* �л��� ������ ���α׷� ���� */
pid_t inBackground(char *name); //���μ��� ��ȣ�� �����ؼ� ����
double check_error_warning(char *filename); /* ������ �ÿ� error�� warning�� üũ */
int compare_resultfile(char *file1, char *file2); /* �л� ���α׷��� ���� ���α׷� �� */

// void do_cOption(char (*ids)[FILELEN]); /* ���ڷ� ���� �л����� ���� ��� */
void do_iOption(char (*ids)[FILELEN]); /* ���ڷ� ���� �л����� Ʋ�� ���� ���� ��� */
int is_exist(char (*src)[FILELEN], char *target); /* ���ڷ� ���� �л��� id�� �迭�� �����ϴ��� Ȯ�� */

int is_thread(char *qname); /* ���ڷ� ���� qname�� ���� �帧�� �ִ��� Ȯ�� */
void redirection(char *command, int newfd, int oldfd); /* ����� ���μ����� ��Ʈ���� ���Ͽ��� ����ϱ� ���� ǥ�� ��Ʈ���� �帧�� �ٲ��� */
int get_file_type(char *filename); /* ������ TEXTFILE���� CFILE���� ���� */
void rmdirs(const char *path); /* ���丮 ���� */
void to_lower_case(char *c); /* ��� ���ڿ��� �ҹ��ڷ� ���� */

void set_scoreTable(char *ansDir); /* ���� ���̺� ���� */
void read_scoreTable(char *path); /* ������ �о score_table�� qname, score ���·� ���� */
void make_scoreTable(char *ansDir); /* ������ �������� �ʴ� ���, �Է¹��� ������ score_table�� ���� */
void write_scoreTable(char *filename); /* score_table�� write */
void set_idTable(char *stuDir); /* id _table ���� */
int get_create_type(); /* ���� ���̺� ������ ���� ���� ��ȣ�� ������ � ���·� �Է� ���� ������ ���� */

void sort_idTable(int size); /* id_table�� ����Ǵ� �л��� id�� ������������ ���� */
void sort_scoreTable(int size); /* score_table�� ����Ǵ� ������ ��ȣ�� ������������ ���� */
void get_qname_number(char *qname, int *num1, int *num2); /* ���ڿ��� ��ū�� �и��Ͽ� ���� ��ȣ ���� */

#endif
