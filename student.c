#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student {
    char name[50];
    int midterm;
    int final;
    struct Student* left;
    struct Student* right;
};

struct Student* createNode(char* name, int midterm, int final) {
    struct Student* newNode = (struct Student*)malloc(sizeof(struct Student));
    strcpy(newNode->name, name);
    newNode->midterm = midterm;
    newNode->final = final;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

struct Student* insert(struct Student* root, char* name, int midterm, int final) {
    if (root == NULL) {
        return createNode(name, midterm, final);
    }
    if (strcmp(name, root->name) < 0) {
        root->left = insert(root->left, name, midterm, final);
    } else {
        root->right = insert(root->right, name, midterm, final);
    }
    return root;
}

void inorder(struct Student* root) {
    if (root != NULL) {
        inorder(root->left);
        printf("학생이름: %s, 중간: %d, 기말: %d\n", root->name, root->midterm, root->final);
        inorder(root->right);
    }
}

int main() {
    int n;
    struct Student* root = NULL;

    printf("학생 수를 입력하세요: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        char name[50];
        int midterm, final;
        printf("이름, 중간, 기말 점수를 입력하세요 (이름, 중간 점수, 기말 점수): ");
        scanf("%s %d %d", name, &midterm, &final);
        root = insert(root, name, midterm, final);
    }

    printf("\n학생 정보 (중위 순회 결과):\n");
    inorder(root);

    return 0;
}
