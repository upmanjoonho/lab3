#include <iostream>
#include <string>
#include <cstdlib>
#include <search.h>

using namespace std;

struct Student {
    string name;
    float midterm;
    float final;
};

struct TreeNode {
    Student student;
    struct TreeNode *left;
    struct TreeNode *right;
};

TreeNode* createNode(const Student& student) {
    TreeNode* newNode = new TreeNode();
    newNode->student = student;
    newNode->left = newNode->right = nullptr;
    return newNode;
}

TreeNode* insert(TreeNode* root, const Student& student) {
    if (root == nullptr) {
        return createNode(student);
    }

    if (student.name < root->student.name) {
        root->left = insert(root->left, student);
    } else {
        root->right = insert(root->right, student);
    }

    return root;
}

void inorderTraversal(TreeNode* root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        cout << "이름: " << root->student.name
             << ", 중간 점수: " << root->student.midterm
             << ", 기말 점수: " << root->student.final << endl;
        inorderTraversal(root->right);
    }
}

void deleteTree(TreeNode* root) {
    if (root != nullptr) {
        deleteTree(root->left);
        deleteTree(root->right);
        delete root;
    }
}

int main() {
    int studentCount;

    cout << "학생 수를 입력하세요: ";
    cin >> studentCount;

    Student* students = new Student[studentCount];
    TreeNode* root = nullptr;

    for (int i = 0; i < studentCount; i++) {
        cout << "학생 " << (i + 1) << "의 이름, 중간 점수를 입력하세요: ";
        cin >> students[i].name >> students[i].midterm;
        cout << "학생 " << (i + 1) << "의 기말 점수를 입력하세요: ";
        cin >> students[i].final;

        root = insert(root, students[i]);
    }

    cout << "\n학생 정보 (이름, 중간 점수, 기말 점수):" << endl;
    inorderTraversal(root);

    deleteTree(root);
    delete[] students;

    return 0;
}
