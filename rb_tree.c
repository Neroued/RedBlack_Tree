// 红黑树的实现
#include <stdio.h>
#include <stdlib.h>

#include "tree_queue.h"
#include "rb_tree.h"

static bool ItemEqual(const Item *i1, const Item *i2);     // 两个Item是否等价
static void CopyItem(const Item *i1, Item *i2);            // 将i1的内容复制到i2
static bool ItemBigger(const Item *i1, const Item *i2);    // 两个item哪个更大
static void RB_Insert_Fixup(RBTree *pt, Node *node);       // 插入后维护红黑树的性质
static void RB_Transplant(RBTree *pt, Node *n1, Node *n2); // 用n2子树代替n1
static void RB_Delete_Fixup(RBTree *pt, Node *node);       // 删除后维护红黑树的性质
static Node *SubTreeMin(RBTree *pt, Node *node);           // 子树中的最小节点
static void PrintNode(const Node *node);

void InitializeRBTree(RBTree *pt)
{
    pt->nil = (Node *)malloc(sizeof(Node));
    pt->root = pt->nil;
    pt->nil->color = _RB_BLACK;
    pt->nil->left = pt->nil;
    pt->nil->right = pt->nil;
    pt->nil->parent = pt->nil;
}

void LeftRotate(RBTree *pt, Node *node) // 左旋，效果是将node变为node右子节点的左子节点
{
    Node *temp = node->right;      // node的右子节点
    node->right = temp->left;      // 将右子节点的左子树接到node的右边
    if (temp->left != pt->nil)     // 若右子树的左子树存在
        temp->left->parent = node; // 将node作为左子树的父节点
    temp->parent = node->parent;   // 右子节点的父节点改为node的父节点
    if (node->parent == pt->nil)   // 修改node父节点的信息
        pt->root = temp;
    else if (node == node->parent->left)
        node->parent->left = temp;
    else
        node->parent->right = temp;
    temp->left = node; // node接到原本右子节点的左边
    node->parent = temp;
}

void RightRotate(RBTree *pt, Node *node) // 右旋，将node变为node左子节点的右子节点
{
    Node *temp = node->left;        // node的左子节点
    node->left = temp->right;       // 左子节点的右子树接到node的左边
    if (temp->right != pt->nil)     // 若左子节点的右子树存在
        temp->right->parent = node; // 左子节点的右子树的父节点变为node
    temp->parent = node->parent;    // node的父节点变为temp的父节点
    if (node->parent == pt->nil)    // node为根节点
        pt->root = temp;
    else if (node == node->parent->left) // 若node是一个左子节点
        node->parent->left = temp;
    else // 若node是一个右子节点
        node->parent->right = temp;
    temp->right = node; // node接到temp的右侧
    node->parent = temp;
}

void RB_Insert(const Item *item, RBTree *pt)
{ // 红黑树的插入和对二叉搜索树的插入类似，但多一个维护红黑树性质的操作
    Node *new_node = (Node *)malloc(sizeof(Node));
    CopyItem(item, &(new_node->item));
    // 新插入节点设置为红色，之后通过Delete_Fixup维护
    new_node->parent = pt->nil;

    Node *x = pt->root;
    Node *y = pt->nil;
    while (x != pt->nil) // 当树非空，查找合适位置
    {
        y = x;
        if (ItemBigger(item, &(x->item)))
            x = x->right;
        else
            x = x->left;
    }
    new_node->parent = y;
    if (y == pt->nil) // 表示树为空
        pt->root = new_node;
    else if (ItemBigger(item, &(y->item)))
        y->right = new_node;
    else
        y->left = new_node;
    new_node->color = _RB_RED;
    new_node->left = pt->nil;
    new_node->right = pt->nil;
    RB_Insert_Fixup(pt, new_node); // 在插入后维护红黑树的性质
}

static void RB_Insert_Fixup(RBTree *pt, Node *node) // 维护红黑树的性质
{
    Node *temp;
    while (node->parent->color == _RB_RED) // 当父节点不是红色时表明该节点需要维护
    {
        if (node->parent == node->parent->parent->left) // 若父节点是一个左子节点
        {
            temp = node->parent->parent->right; // 叔节点
            if (temp->color == _RB_RED)         // 叔节点为红色，表示情况1
            {
                node->parent->color = _RB_BLACK; // 把叔节点和父节点都变为黑色
                temp->color = _RB_BLACK;
                node->parent->parent->color = _RB_RED; // 爷节点变为红色
                node = node->parent->parent;           // 当前node的维护已完成，但爷节点的性质可能改变，需要维护
            }
            else
            {
                if (node == node->parent->right) // 叔节点为黑色，且node与父节点不在同一直线上，表示情况4
                {
                    node = node->parent;  // 将父节点作为新node节点
                    LeftRotate(pt, node); // 左旋使得两者在同一直线上，转化为情况3
                }
                node->parent->color = _RB_BLACK; // 情况3，调换父节点与爷节点的颜色
                node->parent->parent->color = _RB_RED;
                RightRotate(pt, node->parent->parent); // 右旋，保证红黑树的性质
            }
        }
        else // 若父节点是一个右子节点，将上述的左右调换便可
        {
            temp = node->parent->parent->left;
            if (temp->color == _RB_RED)
            {
                node->parent->color = _RB_BLACK;
                temp->color = _RB_BLACK;
                node->parent->parent->color = _RB_RED;
                node = node->parent->parent;
            }
            else
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    RightRotate(pt, node);
                }
                node->parent->color = _RB_BLACK;
                node->parent->parent->color = _RB_RED;
                LeftRotate(pt, node->parent->parent);
            }
        }
    }
    pt->root->color = _RB_BLACK; // 操作后保证树的根节点为黑色
}

static void RB_Transplant(RBTree *pt, Node *n1, Node *n2)
{
    if (n1->parent == pt->nil) // n1是根节点
        pt->root == n2;
    else if (n1 == n1->parent->left) // n1是个左子节点
        n1->parent->left = n2;
    else
        n1->parent->right = n2;
}

void RB_Delete(RBTree *pt, Node *node)
{
    Node *temp = node; // 用于记录
    Node *index;
    int temp_origin_color = node->color; // 记录要被删除节点的颜色
    if (node->left == pt->nil)           // 只有单边子树，直接用子节点替代node节点即可
    {
        index = node->left;
        RB_Transplant(pt, node, node->right);
    }
    else if (node->right == pt->nil) // 同上
    {
        index = node->right;
        RB_Transplant(pt, node, node->left);
    }
    else // 存在两个子节点或没有子节点
    {
        // 找出node的后继，用后继代替node节点，再删除后继
        temp == SubTreeMin(pt, node->right);
        // 记录后继的颜色，若后继为红色则直接删除，反之需要维护
        temp_origin_color = temp->color;
        index = temp->right; // 记录temp右子节点
        if (temp->parent == node)
            index->parent = temp; // index可能为Nil节点，将index父节点设置为要删除的节点，方便后续维护
        else
        {
            RB_Transplant(pt, temp, temp->right); // 情况2
            temp->right = node->right;
            temp->right->parent = temp;
        }
        RB_Transplant(pt, node, temp); // 用后继代替node
        temp->left = node->left;       // 接上node的左子树
        temp->left->parent = temp;
        temp->color = node->color; // 修改颜色
    }
    if (temp_origin_color == _RB_BLACK) // 若删除的是黑色节点，则需要进行维护
        RB_Delete_Fixup(pt, index);
}

static void RB_Delete_Fixup(RBTree *pt, Node *node)
{
    Node *temp;
    while (node != pt->root && node->color == _RB_BLACK)
    {
        if (node == node->parent->left) // node是左子节点
        {
            temp = node->parent->right; // 兄弟节点
            if (temp->color == _RB_RED) // 若兄弟节点为红色，对应情况1
            {
                temp->color = _RB_BLACK; // 交换两个节点颜色
                node->parent->color = _RB_RED;
                LeftRotate(pt, node->parent); // 左旋
                temp = node->parent->right;   // 兄弟节点位置变化
            }
            if (temp->left->color == _RB_BLACK && temp->right->color == _RB_BLACK) // 情况2
            {
                temp->color = _RB_RED; // 剥离兄弟节点的黑色
                node = node->parent;   // 将父节点作为下一轮处理的节点
            }
            else
            {
                if (temp->right->color == _RB_BLACK) // 情况3
                {
                    temp->left->color == _RB_BLACK; // 交换两个节点的颜色
                    temp->color = _RB_RED;
                    RightRotate(pt, temp);      // 右旋
                    temp = node->parent->right; // 兄弟节点发生变化
                }
                temp->color = node->parent->color; // 情况4，交换兄弟节点与父节点的颜色
                node->parent->color = _RB_BLACK;
                temp->right->color = _RB_BLACK; // 把node的额外黑色给兄弟节点的右子节点
                LeftRotate(pt, node->parent);   // 左旋
                node = pt->root;                // 维护完成退出循环
            }
        }
        else // node是右子节点，同上，仅交换了左右
        {
            temp = node->parent->left;
            if (temp->color == _RB_RED)
            {
                temp->color = _RB_BLACK;
                node->parent->color = _RB_RED;
                RightRotate(pt, node->parent);
                temp = node->parent->left;
            }
            if (temp->right->color == _RB_BLACK && temp->left->color == _RB_BLACK)
            {
                temp->color = _RB_RED;
                node = node->parent;
            }
            else
            {
                if (temp->left->color == _RB_BLACK)
                {
                    temp->right->color == _RB_BLACK;
                    temp->color = _RB_RED;
                    LeftRotate(pt, temp);
                    temp = node->parent->left;
                }
                temp->color = node->parent->color;
                node->parent->color = _RB_BLACK;
                temp->left->color = _RB_BLACK;
                RightRotate(pt, node->parent);
                node = pt->root;
            }
        }
    }
    // 退出时，node可能为根节点或pt->nil或某个为红色且具有额外黑色的节点，因此将node染为黑色
    node->color = _RB_BLACK;
    // 在删除操作中可能会把pt->nil->parent暂时设置为某个节点，应当在对该节点操作完成后复原nil
    pt->nil->parent = pt->nil;
}

void PrintTree(const RBTree *pt)
{
    // 要按层打印树，首先我们需要引入两个队列，分别存储当前层的节点与下一层的节点。
    Queue *que0 = (Queue *)malloc(sizeof(Queue));
    Queue *que1 = (Queue *)malloc(sizeof(Queue));
    InitializeQueue(que0);
    InitializeQueue(que1);

    // 打印一层，当下一层为空时结束
    if (pt->root != pt->nil) // 当树不为空
    {
        // 先将第一层装入que0
        EnQueue(pt->root, que0);
        Node *index;
        do
        {
            // 将que0中所有元素的子节点装入que1
            while (!QueueIsEmpty(que0))
            {
                index = DeQueue(que0);      // 从que0中取出一个元素
                PrintNode(index);           // 打印该节点
                if (index->left != pt->nil) // 将子节点装入que1
                    EnQueue(index->left, que1);
                if (index->right != pt->nil)
                    EnQueue(index->right, que1);
            }
            printf("\n");
            Queue *tmp = que0; // 交换两个队列
            que0 = que1;
            que1 = tmp;
        } while (!QueueIsEmpty(que0)); // 当下一层中没有节点时表示树已经打印完
    }
    else
        printf("树为空！");
    DeleteQueue(que0); // 删除队列
    DeleteQueue(que1);
}

Node *SearchNode(const Item *item, const RBTree *pt)
{
    Node *index = pt->root;
    while (index != pt->nil && !ItemEqual(&(index->item), item))
    {
        if (ItemBigger(item, &(index->item)))
            index = index->right;
        else
            index = index->left;
    }
    return index;
}


static void CopyItem(const Item *i1, Item *i2)
{
    i2->num = i1->num;
}

static bool ItemBigger(const Item *i1, const Item *i2)
{
    if (i1->num > i2->num)
        return true;
    return false;
}

static Node *SubTreeMin(RBTree *pt, Node *node)
{
    if (node == pt->nil)
        return pt->nil;
    Node *index = node;
    while (index->left != pt->nil)
        index = index->left;
    return index;
}

static void PrintNode(const Node *node)
{
    printf("%d |", node->item.num);
}

static bool ItemEqual(const Item *i1, const Item *i2)
{
    if (i1->num == i2->num)
        return true;
    return false;
}