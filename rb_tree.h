// 红黑树
#ifndef _RB_TREE_H
#define _RB_TREE_H
#include <stdbool.h>

typedef struct item // 节点存储的对象
{
    int num;
} Item;

#define _RB_BLACK 0
#define _RB_RED 1

typedef struct node // 树节点
{
    int color;           // 颜色,0表示黑色，1表示白色
    struct node *parent; // 父节点
    struct node *left;   // 左子节点
    struct node *right;  // 右子节点
    Item item;           // 对象
} Node;

typedef struct rbtree // 树
{
    Node *root; // 树的根节点
    Node *nil;  // 空节点
} RBTree;

// 初始化红黑树
void InitializeRBTree(RBTree *pt);

// 节点的左旋和右旋
void LeftRotate(RBTree *pt, Node *node);
void RightRotate(RBTree *pt, Node *node);

// 插入节点
void RB_Insert(const Item *item, RBTree *pt);

// 删除节点
void RB_Delete(RBTree *pt, Node *node);

// 查找一个节点，返回对应节点的指针
Node *SearchNode(const Item *item, const RBTree *pt);

// 打印树
void PrintTree(const RBTree *pt);

#endif