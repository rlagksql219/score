#ifndef BLANK_H_
#define BLANK_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef BUFLEN
	#define BUFLEN 1024
#endif

#define OPERATOR_CNT 24
#define DATATYPE_SIZE 35
#define MINLEN 64
#define TOKEN_CNT 50

typedef struct node{ /* 트리 노드 구조체 */
	int parentheses;
	char *name; 
	struct node *parent;
	struct node *child_head;
	struct node *prev;
	struct node *next;
}node;

typedef struct operator_precedence{ /* 연산자의 우선순위를 저장하는 구조체 */
	char *operator; //연산자
	int precedence; //우선순위
}operator_precedence;

void compare_tree(node *root1,  node *root2, int *result); /* 루트 노드로 두개의 트리 비교 */
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses); /* 연산자 우선순위에 따라 트리 생성 */
node *change_sibling(node *parent); /* 노드의 자식들끼리 위치 교환 */
node *create_node(char *name, int parentheses); /* 새로운 노드 생성 */
int get_precedence(char *op); /* 연산자의 우선순위 리턴 */
int is_operator(char *op); /* 인자로 넘겨준 문자열이 연산자인지 확인 */
void print(node *cur); /* 노드 출력 */
node *get_operator(node *cur); /* 현재 노드의 부모 노드인 operator 리턴 */
node *get_root(node *cur); /* 루트 노드 리턴 */
node *get_high_precedence_node(node *cur, node *new); /* 노드의 우선순위를 비교하여 우선순위가 더 높은 노드 리턴 */
node *get_most_high_precedence_node(node *cur, node *new); /* 연산자들끼리의 우선순위를 비교하여 우선순위가 더 높은 노드 리턴 */
node *insert_node(node *old, node *new); /* 새로운 노드 삽입 */
node *get_last_child(node *cur); /* 현재 노드의 마지막 자식 노드를 찾아 리턴 */
void free_node(node *cur); /* 동적으로 할당한 노드의 메모리 해제 */
int get_sibling_cnt(node *cur); /* 노드의 sibling 수를 세서 리턴 */

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]); /* 문자열을 읽어 토큰 배열에 저장 */
int is_typeStatement(char *str); /* 문자열의 datatype 검사 */
int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]); /* 토큰에서 발견한 자료형의 row 리턴 */
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]); /* 토큰에서 발견한 구조체 자료형의 row 리턴 */
int is_character(char c); /* 문자가 character인지 검사 */
int all_star(char *str); /* 문자열이 전부 "*"로 이루어져있는지 확인 */
int all_character(char *str); /* 문자열이 모두 문자로 이루어져있는지 확인 */
int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]); /* 해당 row의 토큰 초기화 */
void clear_tokens(char tokens[TOKEN_CNT][MINLEN]); /* 토큰 초기화 */
int get_token_cnt(char tokens[TOKEN_CNT][MINLEN]); /* 토큰의 count를 셈 */
char *rtrim(char *_str); /* 문자열의 오른쪽 공백 제거 */
char *ltrim(char *_str); /* 문자열의 왼쪽 공백 제거 */
void remove_space(char *str); /* 문자열의 공백 제거 */
int check_brackets(char *str); /* 괄호의 짝이 맞는지 검사 */
char* remove_extraspace(char *str); /* 문자열의 공백 제거 */

#endif
