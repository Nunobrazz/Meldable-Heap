#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEFAULT_SOURCE

typedef struct node* node;
struct node {
    int v; /* The value to store */
    node leftChild; /* Left child */
    node rightChild; /* Right child */
    node* hook; /* Pointer to field in the father node */
};
node A;
int SIZE;

static int coinToss(void) {
    static unsigned int M = 0;
    static unsigned int P;
    int r;
    if(0 == M){
        M = ~0;
        P = random();
    }
    r = P & 1;
    M = M>>1;
    P = P>>1;
    printf("Flipped a %d\n", r);
    return r;
}

int ptr2loc(node v, node A) {
    int r;
    r = -1;
    if(NULL != v)
        r = ((size_t) v - (size_t) A) / sizeof(struct node);
    return (int)r;
}

void showNode(node v) {
    int f;
    if(NULL == v)
        printf("NULL\n");
    else {
        printf("node: %d ", ptr2loc(v, A));
        printf("v: %d ", v->v);
        printf("leftChild: %d ", ptr2loc(v->leftChild, A));
        printf("rightChild: %d ", ptr2loc(v->rightChild, A));
        f = ptr2loc((node)v->hook, A);
        printf("father: %c %d", &(A[f].leftChild) == v->hook ? 'l' : 'r', f);
        printf("\n");
    }
}

void set(node node, int value){
    assert(node->leftChild == NULL && node->rightChild == NULL && node->hook == NULL);
    node->v = value;
    printf("set A[%d] to %d\n",ptr2loc(node,A),value);
}

node meld(node node1, node node2){
    node tmp_node1;
    printf("Meld A[%d] A[%d]\n", ptr2loc(node1, A), ptr2loc(node2, A));

    if (node2 == NULL)
        return node1;
    if (node1 == NULL)
        return node2;

    if (node1->v > node2->v){
        /* Just set node2 to node1 to for the coinToss if*/
        printf("Swap to A[%d] A[%d]\n", ptr2loc(node2, A), ptr2loc(node1, A));
        tmp_node1 = node1;
        node1 = node2;
        node2 = tmp_node1;
    }
    if (coinToss()){
        node rightChild = meld(node1->rightChild, node2);
        rightChild->hook = &node1->rightChild;
        node1->rightChild = rightChild;
        printf("link A[%d] as %s of A[%d]\n", ptr2loc(rightChild, A), "rightChild", ptr2loc(node1, A));
    }
    else{
        node leftChild = meld(node1->leftChild, node2);
        leftChild->hook = &node1->leftChild;
        node1->leftChild = leftChild;
        printf("link A[%d] as %s of A[%d]\n", ptr2loc(leftChild, A), "leftChild", ptr2loc(node1, A));
    }

    return node1;
}

void showHeap(node n){
    showNode(n);
    if (n->leftChild != NULL)
        showHeap(n->leftChild);
    if (n->rightChild != NULL)
        showHeap(n->rightChild);
}

node root(node n){
    int f;
    node rootVar = n;
    printf("root A[%d] is ", ptr2loc(n, A));
    while (rootVar->hook != NULL){
        f = ptr2loc((node)rootVar->hook, A);
        rootVar = &(A[f]);
    }
    printf("A[%d]\n", ptr2loc(rootVar, A));
    return rootVar;
}

int min(node n){
    node rootVar;
    printf("min A[%d]\n", ptr2loc(n, A));
    rootVar = root(n);
    printf("%d\n", rootVar->v);
    return rootVar->v;
}

int argMin(node n){
    node rootVar;
    int loc;
    rootVar = root(n);
    loc = ptr2loc(rootVar, A);
    printf("%d\n", loc);
    return loc;
}

void separate(node n ){

    if (((node) n->hook)->leftChild == n)
        ((node)n->hook)->leftChild = NULL;
    else if (((node)n->hook)->rightChild == n)
        ((node)n->hook)->rightChild = NULL;
    n->hook = NULL;
}

void decreaseKey(node n, int value){
    node rootVar, result;
    if(n != NULL)
        assert(n->v >= value);

    printf("decKey A[%d] to %d\n", ptr2loc(n, A), value);

    if (n->hook == NULL){
        n->v = value;
        printf("%d\n",ptr2loc(n, A));
        return;
    }
    rootVar = root(n);

    /*separate node from heap*/
    *n->hook = NULL;
    n->hook = NULL;

    n->v = value;

    result = meld(rootVar, n);
    printf("%d\n",ptr2loc(result, A));
}

node extractMin(node n){
    node result, rootVar, leftChild, rightChild;
    printf("extractMin A[%d]\n", ptr2loc(n,A));
    rootVar = root(n);
    leftChild = rootVar->leftChild;
    rightChild = rootVar->rightChild;
    rootVar->v = 0;

    /* separate both subtrees*/
    if (leftChild == NULL && rightChild == NULL){
        return n;
    }
    else if (leftChild == NULL && rightChild != NULL){
        *rightChild->hook = NULL;
        rightChild->hook = NULL;
        return rightChild;
    }
    else if (leftChild != NULL && rightChild == NULL){
        *leftChild->hook = NULL;
        leftChild->hook = NULL;
        return leftChild;
    }
    else{
        *leftChild->hook = NULL;
        *rightChild->hook = NULL;
        leftChild->hook = NULL;
        rightChild->hook = NULL;
        result = meld(leftChild, rightChild);
        return result;
    }
}

node delete(node n){
    node rootVar, min;

    printf("delete A[%d]\n", ptr2loc(n,A));

    if (n->hook == NULL){
        min = extractMin(n);
        printf("%d\n",ptr2loc(min, A));
        return min;
    }
    rootVar = root(n);
    /* cut the sub-tree */
    *n->hook = NULL;
    n->hook = NULL;

    min = extractMin(n);

    if (min->rightChild == NULL && min->leftChild == NULL){
        printf("%d\n",ptr2loc(rootVar, A));
        return rootVar;
    }
    else{
        node result = meld(rootVar, min);
        printf("%d\n",ptr2loc(result, A));
        return result;
    }
}
void freeArray(){
    free(A);
}
void finalConfiguration(){
    int i;
    printf("Final configuration:\n");
    for (i = 0; i < SIZE ; i++){
        showNode(&A[i]);
    }
}
void parseOperations(){
    char operation;
    int arg1, arg2;
    while (1){
        operation = getchar();
        switch (operation){
        case 'S':
            scanf(" %d", &arg1);
            showNode(&A[arg1]);
            break;
        case 'P':
            scanf(" %d", &arg1);
            showHeap(&A[arg1]);
            break;
        case 'V':
            scanf(" %d", &arg1);
            scanf(" %d", &arg2);
            set(&A[arg1], arg2);
            break;
        case 'U':
            scanf(" %d", &arg1);
            scanf(" %d", &arg2);
            printf("%d\n",ptr2loc(meld(&A[arg1], &A[arg2]), A));
            break;

        case 'R':
            scanf(" %d", &arg1);
            scanf(" %d", &arg2);
            decreaseKey(&A[arg1], arg2);
            break;

        case 'M':
            scanf(" %d", &arg1);
            min(&A[arg1]);
            break;

        case 'A':
            scanf(" %d", &arg1);
            argMin(&A[arg1]);
            break;

        case 'E':
            scanf(" %d", &arg1);
            printf("%d\n",ptr2loc(extractMin(&A[arg1]), A));
            break;

        case 'D':
            scanf(" %d", &arg1);
            delete(&A[arg1]);
            break;

        case 'X':
            finalConfiguration();
            freeArray();
            return;
            break;
        default:
            printf("Unknown Operation %c\n", operation);
            break;
        }
        getchar();
    }
}

int main(int argc, char const *argv[]) {
    int numNodes;
    unsigned int seed;
    int i;
    scanf("%d %u\n", &numNodes, &seed);
    SIZE = numNodes;
    srandom(seed);
    A = (node)calloc(numNodes, sizeof(struct node));
    for(i = 0; i < numNodes; i++){
        A[i].v = 0;
        A[i].leftChild = NULL;
        A[i].rightChild = NULL;
        A[i].hook = NULL;
    }
    parseOperations();
    return 0;
}
