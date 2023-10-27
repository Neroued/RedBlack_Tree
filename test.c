#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"
#include "tree_queue.h"

static void PrintNode(const Node *node)
{
    printf("%d |", node->item.num);
}

int main(void)
{
    srand(1);
    RBTree *pt = (RBTree *)malloc(sizeof(RBTree));
    InitializeRBTree(pt);
    //PrintTree(pt);

    Item *item = (Item *)malloc(sizeof(Item));
    int j = -1;
    int nums[5] = {1,2,3,4,5};
    for (int i = 0; i < 63; i++)
    {
        item->num = rand();
        RB_Insert(item, pt);
    }
    //printf("%d ", pt->root->left->left->left->left->left->item.num);
    RB_Delete(pt, pt->root->left->right->right->left);
    PrintTree(pt);

    item->num = 15724;
    PrintNode(SearchNode(item, pt));


}