#include <stdio.h>
#include <stdlib.h>

// AVL Node
typedef struct Node {
    int player_id;
    int score;
    int height;
    int size;
    struct Node *left, *right;
} Node;

// Utility functions
int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(Node *n) {
    return n ? n->height : 0;
}

int size(Node *n) {
    return n ? n->size : 0;
}

Node* createNode(int id, int score) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->player_id = id;
    node->score = score;
    node->height = 1;
    node->size = 1;
    node->left = node->right = NULL;
    return node;
}

// Update node
void update(Node *n) {
    if (!n) return;
    n->height = 1 + max(height(n->left), height(n->right));
    n->size = 1 + size(n->left) + size(n->right);
}

// Rotations
Node* rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;

    x->right = y;
    y->left = T2;

    update(y);
    update(x);
    return x;
}

Node* leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    update(x);
    update(y);
    return y;
}

int getBalance(Node *n) {
    return n ? height(n->left) - height(n->right) : 0;
}

// Insert
Node* insert(Node* root, int id, int score) {
    if (!root) return createNode(id, score);

    if (score < root->score)
        root->left = insert(root->left, id, score);
    else
        root->right = insert(root->right, id, score);

    update(root);

    int balance = getBalance(root);

    if (balance > 1 && score < root->left->score)
        return rightRotate(root);

    if (balance < -1 && score > root->right->score)
        return leftRotate(root);

    if (balance > 1 && score > root->left->score) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && score < root->right->score) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Find minimum node
Node* minValueNode(Node* node) {
    while (node->left)
        node = node->left;
    return node;
}

// Delete
Node* deleteNode(Node* root, int score) {
    if (!root) return root;

    if (score < root->score)
        root->left = deleteNode(root->left, score);
    else if (score > root->score)
        root->right = deleteNode(root->right, score);
    else {
        if (!root->left || !root->right) {
            Node *temp = root->left ? root->left : root->right;

            if (!temp) {
                free(root);
                return NULL;
            } else {
                *root = *temp;
                free(temp);
            }
        } else {
            Node* temp = minValueNode(root->right);
            root->score = temp->score;
            root->player_id = temp->player_id;
            root->right = deleteNode(root->right, temp->score);
        }
    }

    update(root);

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Search player by ID
Node* search(Node* root, int id) {
    if (!root) return NULL;

    if (root->player_id == id)
        return root;

    Node* res = search(root->left, id);
    if (res) return res;

    return search(root->right, id);
}

// Update score
Node* updateScore(Node* root, int id, int newScore) {
    Node* player = search(root, id);
    if (!player) {
        printf("Player not found!\n");
        return root;
    }

    root = deleteNode(root, player->score);
    root = insert(root, id, newScore);

    printf("Score updated successfully!\n");
    return root;
}

// Get rank
int getRank(Node* root, int score) {
    if (!root) return 0;

    if (score < root->score)
        return 1 + size(root->right) + getRank(root->left, score);
    else if (score > root->score)
        return getRank(root->right, score);
    else
        return size(root->right) + 1;
}

// Display leaderboard
void display(Node* root) {
    if (!root) return;
    display(root->right);
    printf("Player %d -> %d\n", root->player_id, root->score);
    display(root->left);
}

// Top-K
void topK(Node* root, int *k) {
    if (!root || *k <= 0) return;

    topK(root->right, k);

    if (*k > 0) {
        printf("Player %d -> %d\n", root->player_id, root->score);
        (*k)--;
    }

    topK(root->left, k);
}

// MAIN
int main() {
    Node* root = NULL;
    int choice, id, score, k;

    while (1) {
        printf("\n--- Leaderboard Menu ---\n");
        printf("1. Add Player\n");
        printf("2. Update Score\n");
        printf("3. Get Rank (by score)\n");
        printf("4. Top-K Players\n");
        printf("5. Display Leaderboard\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Enter Player ID and Score: ");
            scanf("%d %d", &id, &score);
            root = insert(root, id, score);
            break;

        case 2:
            printf("Enter Player ID and New Score: ");
            scanf("%d %d", &id, &score);
            root = updateScore(root, id, score);
            break;

        case 3:
            printf("Enter Score: ");
            scanf("%d", &score);
            printf("Rank: %d\n", getRank(root, score));
            break;

        case 4:
            printf("Enter K: ");
            scanf("%d", &k);
            topK(root, &k);
            break;

        case 5:
            printf("\nLeaderboard:\n");
            display(root);
            break;

        case 6:
            printf("Exiting...\n");
            exit(0);

        default:
            printf("Invalid choice!\n");
        }
    }
}