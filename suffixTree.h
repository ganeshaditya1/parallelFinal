#ifndef suffixtree
#define suffixtree
#define MAX_CHAR 256


using namespace std;

struct SuffixTreeNode {
    struct SuffixTreeNode *children[MAX_CHAR];
 
    //pointer to other node via suffix link
    struct SuffixTreeNode *suffixLink;
 
    /*(start, end) interval specifies the edge, by which the
     node is connected to its parent node. Each edge will
     connect two nodes,  one parent and one child, and
     (start, end) interval of a given edge  will be stored
     in the child node. Lets say there are two nods A and B
     connected by an edge with indices (5, 8) then this
     indices (5, 8) will be stored in node B. */
    int start;
    int *end;
 
    /*for leaf nodes, it stores the index of suffix for
      the path  from root to leaf*/
    int suffixIndex;
};
 
typedef struct SuffixTreeNode Node;

void buildSuffixTree(char *);
Node* getRootNode();
#endif
