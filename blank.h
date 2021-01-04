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

typedef struct node{ /* Ʈ�� ��� ����ü */
	int parentheses;
	char *name; 
	struct node *parent;
	struct node *child_head;
	struct node *prev;
	struct node *next;
}node;

typedef struct operator_precedence{ /* �������� �켱������ �����ϴ� ����ü */
	char *operator; //������
	int precedence; //�켱����
}operator_precedence;

void compare_tree(node *root1,  node *root2, int *result); /* ��Ʈ ���� �ΰ��� Ʈ�� �� */
node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses); /* ������ �켱������ ���� Ʈ�� ���� */
node *change_sibling(node *parent); /* ����� �ڽĵ鳢�� ��ġ ��ȯ */
node *create_node(char *name, int parentheses); /* ���ο� ��� ���� */
int get_precedence(char *op); /* �������� �켱���� ���� */
int is_operator(char *op); /* ���ڷ� �Ѱ��� ���ڿ��� ���������� Ȯ�� */
void print(node *cur); /* ��� ��� */
node *get_operator(node *cur); /* ���� ����� �θ� ����� operator ���� */
node *get_root(node *cur); /* ��Ʈ ��� ���� */
node *get_high_precedence_node(node *cur, node *new); /* ����� �켱������ ���Ͽ� �켱������ �� ���� ��� ���� */
node *get_most_high_precedence_node(node *cur, node *new); /* �����ڵ鳢���� �켱������ ���Ͽ� �켱������ �� ���� ��� ���� */
node *insert_node(node *old, node *new); /* ���ο� ��� ���� */
node *get_last_child(node *cur); /* ���� ����� ������ �ڽ� ��带 ã�� ���� */
void free_node(node *cur); /* �������� �Ҵ��� ����� �޸� ���� */
int get_sibling_cnt(node *cur); /* ����� sibling ���� ���� ���� */

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN]); /* ���ڿ��� �о� ��ū �迭�� ���� */
int is_typeStatement(char *str); /* ���ڿ��� datatype �˻� */
int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]); /* ��ū���� �߰��� �ڷ����� row ���� */
int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]); /* ��ū���� �߰��� ����ü �ڷ����� row ���� */
int is_character(char c); /* ���ڰ� character���� �˻� */
int all_star(char *str); /* ���ڿ��� ���� "*"�� �̷�����ִ��� Ȯ�� */
int all_character(char *str); /* ���ڿ��� ��� ���ڷ� �̷�����ִ��� Ȯ�� */
int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]); /* �ش� row�� ��ū �ʱ�ȭ */
void clear_tokens(char tokens[TOKEN_CNT][MINLEN]); /* ��ū �ʱ�ȭ */
int get_token_cnt(char tokens[TOKEN_CNT][MINLEN]); /* ��ū�� count�� �� */
char *rtrim(char *_str); /* ���ڿ��� ������ ���� ���� */
char *ltrim(char *_str); /* ���ڿ��� ���� ���� ���� */
void remove_space(char *str); /* ���ڿ��� ���� ���� */
int check_brackets(char *str); /* ��ȣ�� ¦�� �´��� �˻� */
char* remove_extraspace(char *str); /* ���ڿ��� ���� ���� */

#endif
