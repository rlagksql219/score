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

int mOption = false; //채점 전에 원하는 문제 점수 수정
int eOption = false; //error.txt에 에러 메시지 출력
int tOption = false; //컴파일 시 -lpthread 옵션 추가
int iOption = false; //해당 학생들의 틀린 문제 파일 출력


/**
함 수 : ssu_score(int argc, char *argv[])
기 능 : 정답 파일을 기준으로 학생들이 제출한 답안을 채점
*/
void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){ //인자로 받은 문자열과 "-h"가 일치하면
			print_usage(); //사용법 출력
			return;
		}
	}

	memset(saved_path, 0, BUFLEN); //배열 0으로 초기화
	if(argc >= 3 && strcmp(argv[1], "-i") != 0){ //인자가 제대로 들어온 경우
		strcpy(stuDir, argv[1]); //첫번째 인자를 stuDir로 복사
		strcpy(ansDir, argv[2]); //두번째 인자를 ansDir로 복사
	}

	if(!check_option(argc, argv)) //인자로 어떤 옵션이 주어졌는지 확인
		exit(1);

	if(!mOption && !eOption && !tOption && iOption){ //"-i" 옵션인 경우
		do_iOption(cIDs); //해당 학생들의 틀린 문제 파일 출력
		return;
	}

	getcwd(saved_path, BUFLEN); //현재 작업 디렉토리를 saved_path에 저장

	if(chdir(stuDir) < 0){ //작업 디렉토리를 stuDir로 변경
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN); //현재 작업 디렉토리를 stuDir에 저장

	chdir(saved_path); //작업 디렉토리를 saved_path로 변경
	if(chdir(ansDir) < 0){ //작업 디렉토리를 ansDir로 변경
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN); //현재 작업 디렉토리를 ansDir에 저장

	chdir(saved_path); //작업 디렉토리를 saved_path로 변경

	set_scoreTable(ansDir); //점수 테이블 생성
	set_idTable(stuDir); //id 테이블 생성

	printf("grading student's test papers..\n");
	score_students(); //학생들의 점수 계산

	if(iOption) { //-i 옵션이 지정되어 있는 경우
		do_iOption(cIDs); //해당 학생들의 틀린 문제 파일 출력
	}

	return;
}


/**
함 수 : check_option(int argc, char *argv[])
기 능 : 명령행 인자로 받은 옵션이 무엇인지 확인
*/
int check_option(int argc, char *argv[])
{
	int i, j;
	int c;

	/* 명령행 인자 분석 */
	while((c = getopt(argc, argv, "e:thmi")) != -1)
	{
		switch(c){
			case 'e': //인자로 -e 옵션 받은 경우
				eOption = true;
				strcpy(errorDir, optarg); //optarg를 errorDir에 복사

				if(access(errorDir, F_OK) < 0) //errorDir 존재 여부 판정
					mkdir(errorDir, 0755); //errorDir 생성
				else{
					rmdirs(errorDir); //errorDir 제거
					mkdir(errorDir, 0755); //다시 errorDir 생성
				}
				break;

			case 't': //인자로 -t 옵션 받은 경우
				tOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM) //최대 가변인자 개수 초과
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //명령행 인자를 threadFiles에 복사
					i++; 
					j++;
				}
				break;

		//	case 'm': //인자로 -m 옵션 받은 경우
				//구현
			
			case 'i': //인자로 -i 옵션 받은 경우
				iOption = true;
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j >= ARGNUM) //최대 가변인자 개수 초과
						printf("Maximum Number of Argument Exceeded.  :: %s\n", argv[i]);
					else
						strcpy(threadFiles[j], argv[i]); //명령행 인자를 cIDs에 복사
					i++; 
					j++;
				}
				break;

			case '?': //인자로 ? 받은 경우
				printf("Unkown option %c\n", optopt); //지정되지 않은 옵션
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
함 수 : do_iOption(char (*ids)[FILELEN])
기 능 : "-i" 옵션 수행동작인 인자로 받은 학생들의 틀린 문제 파일 출력
*/
void do_iOption(char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	int i = 0;
	char *p, *saved;

	if((fp = fopen("score.csv", "r")) == NULL){ //파일 읽기전용으로 오픈
		fprintf(stderr, "file open error for score.csv\n");
		return;
	}

	fscanf(fp, "%s\n", tmp); //오픈된 파일로부터 tmp 읽음
	

	/* 파일 끝까지 읽음 */
	while(fscanf(fp, "%s\n", tmp) != EOF)
	{
		p = strtok(tmp, ","); //","를 기준으로 문자열 자름
		
		if(!is_exist(ids, tmp)) //해당 학생의 id가 tmp에 존재하지 않는 경우
			continue; //내용 건너뜀

		printf("%s's wrong answer : ", tmp);
	
		/*  자른 문자열에 NULL 넣어줌 */
		while((p = strtok(NULL, ",")) != NULL) {
			if(!strcmp(p, "0")) { //p가 가리키는 점수가 0점인 경우
				saved = p;
				printf("%s\n", saved); //0점인 점수 출력
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
함 수 : set_scoreTable(char *ansDir)
기 능 : 점수 테이블 생성
*/
void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	sprintf(filename, "%s", "score_table.csv"); //filename에 주어진 형태대로 저장

	if(access(filename, F_OK) == 0) //파일 존재여부 판정
		read_scoreTable(filename); //파일을 읽어 score_table에 qname, score 형태로 저장
	else{
		make_scoreTable(ansDir); //입력받은 정보대로 score_table에 qname, score 형태로 저장
		write_scoreTable(filename); //scoreTable에 저장된 데이터를 새로 생성한 파일에 write
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
함 수 : make_scoreTable(char *ansDir)
기 능 : 파일이 존재하지 않는 경우, 2가지의 type으로 나눠서 문제의 score를 입력받고, score_table에 qname, score 형태로 저장
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

	num = get_create_type(); //문제 번호와 점수를 어떤 형태로 입력받을지 결정

	if(num == 1) //빈칸 문제와 프로그램 문제의 점수를 입력받는 경우
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore); //빈칸 문제 score
		printf("Input value of program question : ");
		scanf("%lf", &pscore); //프로그램 문제 score
	}

	if((dp = opendir(ansDir)) == NULL){ //DIR 구조체 얻고, 내부 구조체 초기화
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}	

	/*  DIR 구조체로 정보 읽음 */
	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //디렉토리 이름이 "."이거나 ".."인 경우에
			continue; //내용 건너뜀

		if((type = get_file_type(dirp->d_name)) < 0) //파일의 타입 정보를 type 변수에 저장
			continue;

		strcpy(score_table[idx++].qname, dirp->d_name); //dirp에 저장된 디렉토리 이름을 score_table의 qname 멤버에 저장

	}

	closedir(dp); //dp 구조체의 디렉토리 close
	sort_scoreTable(idx); //scoreTable의 문제 번호를 오름차순으로 정렬

	for(i = 0; i < idx; i++)
	{
		type = get_file_type(score_table[i].qname); //파일의 타입 정보를 type 변수에 저장

		if(num == 1) //score를 빈칸 문제와 프로그램 문제로 나누어 입력받는 경우
		{
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2) //모든 문제의 score를 입력받는 경우
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score; //score_table의 score 변수에 입력받은 정보 저장
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
함 수 : score_students()
기 능 : 학생들의 score 계산
*/
void score_students()
{
	double score = 0;
	int num;
	int fd;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);

	if((fd = creat("score.csv", 0666)) < 0){ //"score.csv" 이름의 파일 생성
		fprintf(stderr, "creat error for score.csv");
		return;
	}
	write_first_row(fd); //문제 번호의 나열과 sum의 형태를 가지도록 학생들의 성적 테이블의 첫번째 row 생성

	for(num = 0; num < size; num++)
	{
		if(!strcmp(id_table[num], ""))
			break;

		sprintf(tmp, "%s,", id_table[num]); //num으로 카운트되는 id_table의 요소를 tmp에 출력
		write(fd, tmp, strlen(tmp));  //tmp에서 해당 크기만큼을 fd가 의미하는 파일에 write

		score += score_student(fd, id_table[num]); //학생의 score를 인자로 받아 점수 누적
	}

	printf("Total average : %.2f\n", score / num); //전체 평균 출력

	close(fd);
}


/**
함 수 : score_student(int fd, char *id)
기 능 : 학생의 학번을 인자로 받아 score 계산
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
		if(score_table[i].score == 0) //score_table의 점수가 0인 경우
			break;

		sprintf(tmp, "%s/%s/%s", stuDir, id, score_table[i].qname); //stuDir, id, qname을 주어진 형태로 tmp에 출력

		if(access(tmp, F_OK) < 0) //tmp의 파일 존재 여부 판정
			result = false;
		else
		{
			if((type = get_file_type(score_table[i].qname)) < 0) //파일의 타입 정보를 type 변수에 저장
				continue;
			
			if(type == TEXTFILE) //파일이 TEXTFILE인 경우
				result = score_blank(id, score_table[i].qname); //빈칸 문제 채점
			else if(type == CFILE) //파일이 CFILE인 경우
				result = score_program(id, score_table[i].qname); //프로그램 문제 채점
		}

		if(result == false) //학생 답안이 정답이 아닌 경우
			write(fd, "0,", 2); //fd가 의미하는 파일에 0점을 의미하는 "0," 입력
		else{
			if(result == true){ //학생 답안이 정답인 경우
				score += score_table[i].score; //점수 누적
				sprintf(tmp, "%.2f,", score_table[i].score); //점수는 소수 둘째자리까지 tmp에 저장
			}
			else if(result < 0){ //학생 답안이 정답이지만 warning이 발생한 경우
				score = score + score_table[i].score + result; //해당 점수 누적
				sprintf(tmp, "%.2f,", score_table[i].score + result); //점수는 소수 둘째자리까지 tmp에 저장
			}
			write(fd, tmp, strlen(tmp)); //점수를 채점 결과 테이블에 write
		}
	}

	printf("%s is finished.. score : %.2f\n", id, score);  //채점을 진행하며 학생들의 점수 출력

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
함 수 : score_blank(char *id, char *filename)
기 능 : 주어진 문제가 빈칸 문제일 경우, 학생이 제출한 txt 파일 채점
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

	memset(qname, 0, sizeof(qname)); //qname 0으로 초기화
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); //qname에 filename 복사

	sprintf(tmp, "%s/%s/%s", stuDir, id, filename); //tmp에 다음과 같은 형태로 저장
	fd_std = open(tmp, O_RDONLY); //tmp를 읽기 전용으로 오픈
	strcpy(s_answer, get_answer(fd_std, s_answer)); //읽은 내용을 s_answer에 저장

	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	if(!check_brackets(s_answer)){ //괄호의 짝이 맞는지 검사
		close(fd_std);
		return false;
	}

	strcpy(s_answer, ltrim(rtrim(s_answer))); //문자열 공백제거

	if(s_answer[strlen(s_answer) - 1] == ';'){
		has_semicolon = true;
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	/* 학생이 제출한 txt 파일을 읽어들여 토큰 배열에 저장 */ 
	if(!make_tokens(s_answer, tokens)){
		close(fd_std); //파일 close
		return false;
	}

	idx = 0;
	std_root = make_tree(std_root, tokens, &idx, 0); //토큰에서 연산자 우선 순위에 따라 트리 생성

	sprintf(tmp, "%s/%s", ansDir, filename); //tmp에 다음과 같은 형태로 저장
	fd_ans = open(tmp, O_RDONLY); //tmp를 읽기 전용으로 오픈

	while(1)
	{
		ans_root = NULL;
		result = true;

		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx])); //메모리 초기화

		strcpy(a_answer, get_answer(fd_ans, a_answer)); //읽은 내용을 a_answer에 저장

		if(!strcmp(a_answer, ""))
			break;

		strcpy(a_answer, ltrim(rtrim(a_answer))); //문자열 공백제거

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

		/* 정답 txt 파일을 읽어들여 토큰 배열에 저장 */
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		ans_root = make_tree(ans_root, tokens, &idx, 0); //토큰에서 연산자 우선 순위에 따라 트리 생성

		compare_tree(std_root, ans_root, &result); //학생 답안으로 만든 트리와 정답으로 만든 트리를 비교하여 정답 확인

		if(result == true){ //정답인 경우
			close(fd_std); //파일 close
			close(fd_ans); //파일 close

			if(std_root != NULL) //학생 답안으로 만든 트리의 메모리 해제
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root); //정답으로 만든 트리의 메모리 해제
			return true;

		}
	}
	
	close(fd_std);
	close(fd_ans);

	if(std_root != NULL)
		free_node(std_root); //학생 답안으로 만든 트리의 메모리 해제
	if(ans_root != NULL)
		free_node(ans_root); //정답으로 만든 트리의 메모리 해제

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
함 수 : compile_program(char *id, char *filename)
기 능 : 학생이 답안으로 제출한 프로그램 컴파일
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

	memset(qname, 0, sizeof(qname)); //배열 0으로 초기화
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.'))); //filename을 해당 길이만큼 qname에 복사
	
	isthread = is_thread(qname); //qname이 실행흐름에 있는지 확인

	sprintf(tmp_f, "%s/%s", ansDir, filename);
	sprintf(tmp_e, "%s/%s.exe", ansDir, qname);

	if(tOption && isthread) //컴파일 할 때, -t 옵션으로 -lpthrad 옵션 추가 한 경우
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_e, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666); //tmp_e 파일 생성

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END); //파일 크기 저장
	close(fd);
	unlink(tmp_e); //tmp_e에 대한 링크 해제

	if(size > 0)
		return false;

	sprintf(tmp_f, "%s/%s/%s", stuDir, id, filename);
	sprintf(tmp_e, "%s/%s/%s.stdexe", stuDir, id, qname);

	if(tOption && isthread) //컴파일 할 때, -t 옵션으로 -lpthrad 옵션 추가 한 경우
		sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
	else
		sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);

	sprintf(tmp_f, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666); //tmp_f 파일 생성

	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END); //파일 크기 저장
	close(fd); //파일 close

	if(size > 0){
		if(eOption) //-e옵션이 지정된 경우
		{
			sprintf(tmp_e, "%s/%s", errorDir, id);
			if(access(tmp_e, F_OK) < 0) //파일 존재 여부 판정
				mkdir(tmp_e, 0755); //tmp_e 디렉토리 생성

			sprintf(tmp_e, "%s/%s/%s_error.txt", errorDir, id, qname);
			rename(tmp_f, tmp_e); //파일의 tmp_f의 이름을 tmp_e로 변경

			result = check_error_warning(tmp_e); //tmp_e의 에러체크
		}
		else{ 
			result = check_error_warning(tmp_f); //tmp_f의 에러체크
			unlink(tmp_f); //tmp_f에 대한 링크 해제
		}

		return result;
	}

	unlink(tmp_f); //tmp_f에 대한 링크 해제
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
함 수 : execute_program(char *id, char *filename)
기 능 : 학생이 답안으로 제출한 프로그램 실행
*/
int execute_program(char *id, char *filename)
{
	char std_fname[BUFLEN], ans_fname[BUFLEN];
	char tmp[BUFLEN];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname)); //배열 초기화
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	sprintf(ans_fname, "%s/%s.stdout", ansDir, qname); //주어진 형태대로 ans_fname에 출력
	fd = creat(ans_fname, 0666); //파일 생성

	sprintf(tmp, "%s/%s.exe", ansDir, qname); //주어진 형태대로 tmp에 출력
	redirection(tmp, fd, STDOUT);
	close(fd); //파일 close

	sprintf(std_fname, "%s/%s/%s.stdout", stuDir, id, qname); //주어진 형태대로 std_fname에 출력
	fd = creat(std_fname, 0666); //파일 생성

	sprintf(tmp, "%s/%s/%s.stdexe &", stuDir, id, qname); //주어진 형태대로 tmp에 출력

	start = time(NULL);
	redirection(tmp, fd, STDOUT);
	
	sprintf(tmp, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){
		end = time(NULL);

		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL); //pid를 이용해 프로세스 종료
			close(fd);
			return false;
		}
	}

	close(fd);

	return compare_resultfile(std_fname, ans_fname); //학생 프로그램을 정답 프로그램과 비교한 채점 결과 리턴
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
함 수 : rmdirs(const char *path)
기 능 : 디렉토리의 파일을 하나씩 제거하며 최종적으로 디렉토리 제거
*/
void rmdirs(const char *path)
{
	struct dirent *dirp; //디렉토리 정보 나타내는 구조체
	struct stat statbuf;
	DIR *dp;
	char tmp[BUFLEN];
	
	if((dp = opendir(path)) == NULL) //인자로 받은 디렉토리 오픈
		return;

	/* 디렉토리 정보 끝까지 read */
	while((dirp = readdir(dp)) != NULL)
	{
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) //디렉토리 이름이 "."이거나 ".."이라면
			continue; //내용 건너뜀

		sprintf(tmp, "%s/%s", path, dirp->d_name); //path와 디렉토리 이름을 tmp 배열에 출력

		if(lstat(tmp, &statbuf) == -1) //파일 정보 확인
			continue;

		if(S_ISDIR(statbuf.st_mode)) //파일의 종류가 디렉토리라면
			rmdirs(tmp); //제귀 방식으로 호출
		else
			unlink(tmp); //tmp의 링크 제거
	}

	closedir(dp); //디렉토리 close
	rmdir(path); //재귀 방식으로 호출
}

void to_lower_case(char *c)
{
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}


/**
함 수 : print_usage()
기 능 : 프로그램의 사용법 출력
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
