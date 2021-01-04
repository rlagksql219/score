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

struct ssu_scoreTable{ /* 점수 테이블 구조체 */
	char qname[FILELEN]; //문제 번호
	double score; //점수
};

void ssu_score(int argc, char *argv[]); /* 정답 파일을 기준으로 학생들이 제출한 답안 채점 */
int check_option(int argc, char *argv[]); /* 명령행 인자로 받은 옵션이 무엇인지 확인 */
void print_usage(); /* 프로그램 사용법 출력 */

void score_students(); /* 학생들의 score 계산 */
double score_student(int fd, char *id); /* 학생의 학번을 인자로 받아 score 계산 */
void write_first_row(int fd); /* 학생들의 성적을 계산하는 table의 첫번째 row 생성 */

char *get_answer(int fd, char *result); /* 파일에서 읽은 내용을 읽어 배열 형태로 저장 */
int score_blank(char *id, char *filename); /* 학생이 제출한 txt 파일 채점 */
double score_program(char *id, char *filename); /* 학생이 제출한 프로그램 채점 */
double compile_program(char *id, char *filename); /* 학생이 답안으로 제출한 프로그램 컴파일 */
int execute_program(char *id, char *filname); /* 학생이 제출한 프로그램 실행 */
pid_t inBackground(char *name); //프로세스 번호를 저장해서 리턴
double check_error_warning(char *filename); /* 컴파일 시에 error와 warning을 체크 */
int compare_resultfile(char *file1, char *file2); /* 학생 프로그램과 정답 프로그램 비교 */

// void do_cOption(char (*ids)[FILELEN]); /* 인자로 받은 학생들의 점수 출력 */
void do_iOption(char (*ids)[FILELEN]); /* 인자로 받은 학생들의 틀린 문제 파일 출력 */
int is_exist(char (*src)[FILELEN], char *target); /* 인자로 받은 학생의 id가 배열에 존재하는지 확인 */

int is_thread(char *qname); /* 인자로 받은 qname이 실행 흐름에 있는지 확인 */
void redirection(char *command, int newfd, int oldfd); /* 실행된 프로세스의 스트림을 파일에서 사용하기 위해 표준 스트림의 흐름을 바꿔줌 */
int get_file_type(char *filename); /* 파일이 TEXTFILE인지 CFILE인지 결정 */
void rmdirs(const char *path); /* 디렉토리 제거 */
void to_lower_case(char *c); /* 모든 문자열을 소문자로 변경 */

void set_scoreTable(char *ansDir); /* 점수 테이블 생성 */
void read_scoreTable(char *path); /* 파일을 읽어서 score_table에 qname, score 형태로 저장 */
void make_scoreTable(char *ansDir); /* 파일이 존재하지 않는 경우, 입력받은 점수로 score_table에 저장 */
void write_scoreTable(char *filename); /* score_table을 write */
void set_idTable(char *stuDir); /* id _table 생성 */
int get_create_type(); /* 점수 테이블 생성을 위해 문제 번호와 점수를 어떤 형태로 입력 받을 것인지 결정 */

void sort_idTable(int size); /* id_table에 저장되는 학생의 id를 오름차순으로 정렬 */
void sort_scoreTable(int size); /* score_table에 저장되는 문제의 번호를 오름차순으로 정렬 */
void get_qname_number(char *qname, int *num1, int *num2); /* 문자열의 토큰을 분리하여 문제 번호 저장 */

#endif
