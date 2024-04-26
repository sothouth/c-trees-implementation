#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<Windows.h>
#include<direct.h>
#include<io.h>

int t = 50;//49-99
int n = 0;
//btree最高4阶
char name[50], now[50];
FILE* filein(int dr) {
    sprintf(name, "bdata\\data_%d.dat", dr);
    FILE* F = fopen(name, "r");
    return F;
}

FILE* fileout(int dr) {
    sprintf(name, "bdata\\data_%d.dat", dr);
    FILE* F = fopen(name, "w");
    return F;
}

FILE* F;
char* charl;

struct snode {//检索结果链表
    char name[30];
    snode* next;
    snode(char* wname) {
        if (wname)
            strcpy(name, wname);
        next = 0;
    }
}*snodel;

struct keynode {//关键字结构体
    char key[20];
    snode* head;
    keynode() {
        head = new snode(0);
        head->next = new snode(0);
    }
    keynode(char* wkey) {
        head = new snode(0);
        head->next = new snode(0);
        strcpy(key, wkey);
    }
    keynode(char* wkey, char* name) {
        head = new snode(0);
        head->next = new snode(0);
        strcpy(key, wkey);
        add(name);
    }
    void add(char* name) {
        snodel = new snode(name);
        snodel->next = head->next;
        head->next = snodel;
        return;
    }
    void print() {
        for (snode* now = head->next; now->next != 0; now = now->next)
            printf("%s\n", now->name);
        return;
    }
    ~keynode() {
        while (head->next != 0 && (snodel = head->next))
            delete head, snodel = (head = snodel)->next;
        delete head;
    }
}*keynodel;

struct node {//b树节点
    int n;//key数
    int fdr, dr;
    keynode* key[100];
    int sdr[101];
    bool leaf;
    node(int fdr, bool isl, int wdr = 0) :fdr(fdr), leaf(isl) {
        n = 0;
        dr = wdr ? wdr : ++::n;
        memset(key, 0, sizeof(key));
        memset(sdr, 0, sizeof(sdr));
    }
    int findkeynodem(char* wkey) {//找到大于等于wkey的最小节点
        int now = 0;
        while (now < n && strcmp(key[now]->key, wkey) < 0)
            ++now;
        return now;
    }
    void addkey(char* wkey, int i) {//初始化
        key[i] = new keynode(wkey);
        return;
    }
    void addkey(keynode* wkeynode, int newdr) {//增加key和sdr
        int wp = findkeynodem(wkeynode->key);
        memcpy(key + wp + 1, key + wp, sizeof(int) * (n - wp + 1));
        memcpy(sdr + wp + 2, sdr + wp + 1, sizeof(int) * (n - wp + 2));
        key[wp] = wkeynode;
        sdr[wp + 1] = newdr;
        ++n;
        return;
    }
    keynode* addkey(char* wkey, char* name) {//增加key节点
        int wp = findkeynodem(wkey);
        if (key[wp] == 0) {
            key[wp] = new keynode(wkey, name);
            ++n;
            return key[wp];
        }
        if (!strcmp(key[wp]->key, wkey)) {
            key[wp]->add(name);
            return key[wp];
        }
        ++n;
        memcpy(key + wp + 1, key + wp, sizeof(int) * (n - wp + 1));
        key[wp] = new keynode(wkey, name);
        return key[wp];
    }
    ~node() {
        for (int i = 0; i < n; ++i)
            delete key[i];
    }
}*nodel;

node* read(node* nownode, int n) {
    F = filein(nownode->sdr[n]);
    if (F == 0)
        return 0;
    char str[30];
    node* now = new node(nownode->dr, 1, nownode->sdr[n]);
    fscanf(F, "%d%d%d", &now->n, &now->dr, &now->leaf);
    for (int i = 0; i < 100; ++i)
        fscanf(F, "%d", now->sdr + i);
    for (int i = 0; i < now->n; ++i) {
        fscanf(F, "%s", str);
        now->addkey(str, i);
        while (fscanf(F, "%s", str) && str[0] != 'E')
            now->key[i]->add(str);
    }
    fclose(F);
    return now;
}
node* read(int dr, int fdr) {//从磁盘中读取节点
    F = filein(dr);
    if (F == 0)
        return 0;
    char str[30];
    node* now = new node(fdr, 1, dr);
    fscanf(F, "%d%d%d", &now->n, &now->dr, &now->leaf);
    for (int i = 0; i < 100; ++i)
        fscanf(F, "%d", now->sdr + i);
    for (int i = 0; i < now->n; ++i) {
        fscanf(F, "%s", str);
        now->addkey(str, i);
        while (fscanf(F, "%s", str) && str[0] != 'E')
            now->key[i]->add(str);
    }
    fclose(F);
    return now;
}

int getroot();

node* root;

void saveroot() {
    FILE* F = fopen("bdata\\root.dat", "w");
    fprintf(F, "%d", root->dr);
    fclose(F);
    return;
}
int getroot() {
    int l;
    FILE* F = fopen("bdata\\root.dat", "r");
    if (F == 0)
        return -1;
    fscanf(F, "%d", &l);
    fclose(F);
    return l;
}

void udwrite(node* now) {//不删除的写入
    F = fileout(now->dr);
    char str[30];
    fprintf(F, "%d %d %d\n", now->n, now->dr, now->leaf);
    for (int i = 0; i < 100; ++i)
        fprintf(F, "%d ", now->sdr[i]);
    fprintf(F, "\n");
    for (int i = 0; i < now->n; ++i) {
        fprintf(F, "%s ", now->key[i]->key);
        for (snodel = now->key[i]->head->next; snodel->next != 0; snodel = snodel->next)
            fprintf(F, "%s ", snodel->name);
        fprintf(F, "E\n");
    }
    fclose(F);
    return;
}

void write(node* now) {//向磁盘中写入节点，并将其从内存中删除
    F = fileout(now->dr);
    char str[30];
    fprintf(F, "%d %d %d\n", now->n, now->dr, now->leaf);
    for (int i = 0; i < 100; ++i)
        fprintf(F, "%d ", now->sdr[i]);
    fprintf(F, "\n");
    for (int i = 0; i < now->n; ++i) {
        fprintf(F, "%s ", now->key[i]->key);
        for (snodel = now->key[i]->head->next; snodel->next != 0; snodel = snodel->next)
            fprintf(F, "%s ", snodel->name);
        fprintf(F, "E\n");
    }
    fclose(F);
    if (now != root)
        delete now;
    return;
}

node* open(node* now, int n) {
    nodel = read(now, n);
    write(now);
    return nodel;
}

keynode* search(char* wkey) {
    node* nodel = root;
    int now;
    while (true) {
        now = nodel->findkeynodem(wkey);
        if (now != nodel->n && !strcmp(wkey, nodel->key[now]->key))
            return nodel->key[now];
        nodel = open(nodel, now);
        if (nodel == 0)
            return 0;
    }
}

void create() {
    root = new node(0, 1);
    write(root);
    return;
}

void splidchild(node* now, node* lc) {
    node* rc = new node(now->dr, lc->leaf);
    memcpy(rc->key, lc->key + 50, sizeof(int) * 49);
    now->addkey(lc->key[49], rc->dr);
    memset(lc->key + 49, 0, sizeof(int) * 50);
    if (lc->leaf == 0) {
        memcpy(rc->sdr, lc->sdr + 50, sizeof(int) * 50);
        memset(lc->sdr + 50, 0, sizeof(int) * 50);
    }
    lc->n = 49;
    rc->n = 49;
    udwrite(now);
    write(rc);
    write(lc);
    return;
}

void insertnonfull(node* now, char* wkey, char* name) {
    int i = now->n - 1;
    node* x;
    if (now->leaf) {
        now->addkey(wkey, name);
        write(now);
        return;
    }
    else {
        i = now->findkeynodem(wkey);
        if (i != now->n && strcmp(now->key[i]->key, wkey) == 0) {
            now->key[i]->add(name);
            write(now);
            return;
        }
        x = read(now, i);
        if (x->n == 99) {
            splidchild(now, x);
            if (strcmp(wkey, now->key[i]->key) > 0)
                ++i;
            x = read(now, i);
        }
        write(now);
        insertnonfull(x, wkey, name);
        return;
    }
}

void insert(char* wkey, char* name) {
    if (root->n == 99) {
        node* lc = root;
        root = new node(0, 0);
        root->sdr[0] = lc->dr;
        splidchild(root, lc);
    }
    insertnonfull(root, wkey, name);
    return;
}

FILE* fileopen() {
    FILE* F = fopen("data.dat", "r");
    return F;
}

int main() {
    if (_access("bdata", 0) == -1)
        _mkdir("bdata");
    char order[10] = { 0 };
    char name[30];
    char key[20];
    keynode* keynodel;
    scanf("%s", order);
    if (strcmp(order, "read") == 0) {
        FILE* F = fileopen();
        create();
        int i = -1;
        while (~fscanf(F, "%s", name)) {
            printf("%d\n", ++i);
            while (fscanf(F, "%s", key) && key[0] != 'E')
                insert(key, name);
        }
        fclose(F);
    }
    else {
        root = read(getroot(), 0);
    }
    while (scanf("%s", key)) {
        system("cls");
        if (key[0] == 'E')
            break;
        keynodel = search(key);
        if (keynodel == 0)
            printf("N\n");
        keynodel->print();
    }
    saveroot();
    delete root;
    return 0;
}
