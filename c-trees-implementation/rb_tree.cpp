#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>

char name[20];
FILE* fileopen(int n) {
    sprintf(name, "date\\date%d.txt", n);
    FILE* F = fopen(name, "r");
    if (F)
        return F;
    return 0;
}
char* NU = new char[5];
struct snode {//存储name的结点，构成链表
    char* ans;
    snode* next;
    snode(char* name = NU) {
        ans = name;
        next = 0;
    }
};
struct node {//红黑树结点，构成红黑树
    char* key;
    bool rb;
    snode* head;
    node* l, * r, * f;
    node(char* now, node* f, bool rb) :f(f), rb(rb) {
        key = (char*)malloc(sizeof(now));
        strcpy(key, now);
        head = new snode();
        head->next = new snode();
        l = r = 0;
    }
    void add(char* name) {
        snode* now = new snode(name);
        now->next = head->next;
        head->next = now;
        return;
    }
}*end = new node(name, 0, 1);

void leftrotate(node* x) {
    node* y = x->r;
    x->r = y->l;
    if (y->l != end)
        y->l->f = x;
    y->f = x->f;
    if (x->f == end)
        end->f = y;
    else if (x == x->f->l)
        x->f->l = y;
    else
        x->f->r = y;
    y->l = x;
    x->f = y;
    return;
}

void rightrotate(node* x) {
    node* y = x->l;
    x->l = y->r;
    if (y->r != end)
        y->r->f = x;
    y->f = x->f;
    if (x->f == end)
        end->f = y;
    else if (x == x->f->r)
        x->f->r = y;
    else
        x->f->l = y;
    y->r = x;
    x->f = y;
    return;
}

void fixup(node* z) {
    node* y;
    while (z->f->rb == 0) {//对z进行重平衡
        if (z->f == z->f->f->l) {
            y = z->f->f->r;
            if (y->rb == 0) {
                z->f->rb = 1;
                y->rb = 1;
                z->f->f->rb = 0;
                z = z->f->f;
            }
            else {
                if (z == z->f->r) {
                    z = z->f;
                    leftrotate(z);
                }
                z->f->rb = 1;
                z->f->f->rb = 0;
                rightrotate(z->f->f);
            }
        }
        else {
            y = z->f->f->l;
            if (y->rb == 0) {
                z->f->rb = 1;
                y->rb = 1;
                z->f->f->rb = 0;
                z = z->f->f;
            }
            else {
                if (z == z->f->l) {
                    z = z->f;
                    rightrotate(z);
                }
                z->f->rb = 1;
                z->f->f->rb = 0;
                leftrotate(z->f->f);
            }
        }
    }
    end->f->rb = 1;//根节点颜色变黑色
    return;
}

void insert(char* wa, char* name) {
    node* y = end;//y=结束结点
    node* x = end->f;//x=树根节点
    while (x != end) {
        y = x;
        if (strcmp(wa, x->key) == 0) {
            x->add(name);
            return;
        }
        if (strcmp(wa, x->key) < 0)
            x = x->l;
        else
            x = x->r;
    }
    node* z = new node(wa, y, 0);//插入新结点
    z->add(name);
    if (y == end)
        end->f = z;
    else if (strcmp(z->key, y->key) < 0)
        y->l = z;
    else
        y->r = z;
    z->l = end;
    z->r = end;
    fixup(z);
    return;
}

node* find(char* wa) {//查找过程
    node* now = end->f;
    while (now != end) {
        if (!strcmp(now->key, wa))
            return now;
        if (strcmp(now->key, wa) > 0)
            now = now->l;
        else
            now = now->r;
    }
    return now;
}

void print(node* now) {
    if (now == end) {
        printf("NO\n");
        return;
    }
    snode* no = now->head;
    while (no->next->next != 0) {
        printf("%s\n", no->next->ans);
        no = no->next;
    }
    return;
}
int main() {
    NU[0] = 0;
    FILE* F;
    end->l = end->r = 0, end->f = end, end->key = new char[4];
    strcpy(end->key, "END");//null结点初始化
    char* name;
    char now[20] = { 0 };
    for (int i = 0; i < 1000; ++i) {
        printf("处理中，请稍候%d", i);
        F = fileopen(i);
        name = (char*)malloc(sizeof(char) * 30);
        fscanf(F, "%s", name);
        while (fscanf(F, "%s", now) != -1)
            insert(now, name);//构建红黑树
        fclose(F);
        system("cls");
    }
    while (scanf("%s", now)) {
        system("cls");
        print(find(now));
    }
    return 0;
}
