#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}/* ------------------------------------------------------------
   QUEUE FUNCTIONS
   ------------------------------------------------------------ */
PokemonQueue* createQueue()
{
    PokemonQueue* q = (PokemonQueue*)malloc(sizeof(PokemonQueue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}
int isEmpty(PokemonQueue* q)
{
    if(q->front == NULL)
        return 1;
    return 0;
}
//
void enqueue(PokemonQueue* q, PokemonNode* treeNode)
{
    NodeQueue* newQueueNode = (NodeQueue*)malloc(sizeof(NodeQueue));
    newQueueNode->treeNode = treeNode;
    newQueueNode->next = NULL;
    if (q->rear == NULL)
    {
        q->front = newQueueNode;
        q->rear = newQueueNode;
    }
    else
    {
        q->rear->next = newQueueNode;
        q->rear = newQueueNode;
    }
}
//
PokemonNode* dequeue(PokemonQueue* q)
{
    if (isEmpty(q))
    {
        return NULL;
    }
    NodeQueue* temp = q->front;
    PokemonNode* treeNode = temp->treeNode;
    q->front = q->front->next;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    free(temp);
    return treeNode;
}
// --------------------------------------------------------------
// GENERIC FUNCTIONS
// --------------------------------------------------------------
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (root == NULL) return;
    PokemonQueue* q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q))
    {
        PokemonNode* currentNode = dequeue(q);
        visit(currentNode);
        if (currentNode->left != NULL)
        {
            enqueue(q, currentNode->left);
        }
        if (currentNode->right != NULL)
        {
            enqueue(q, currentNode->right);
        }
    }
    free(q);
}
//
void preOrderGeneric(PokemonNode* root, VisitNodeFunc visit)
{
    if (root == NULL) return;
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}
//
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (root == NULL) return;
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
    visit(root);
}
//
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (root == NULL) return;
    preOrderGeneric(root->left, visit);
    visit(root);
    preOrderGeneric(root->right, visit);
}
//
/*
PokemonQueue *alphabeticalGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (root == NULL) return;
    PokemonQueue* q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q))
    {
        PokemonNode* currentNode = dequeue(q);
        if (currentNode->left != NULL)
        {
            enqueue(q, currentNode->left);
        }
        if (currentNode->right != NULL)
        {
            enqueue(q, currentNode->right);
        }
    }
    free(q);
}
*/
// --------------------------------------------------------------
// PART ONE
// --------------------------------------------------------------
OwnerNode *createOwner(char *ownerName, PokemonNode *starter)
{
    OwnerNode *newNode = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (!newNode)
    {
        printf("Error: malloc failed\n");
        return NULL;
    }
    newNode->ownerName = ownerName;
    newNode->pokedexRoot = starter;
    newNode->next = NULL;
    if (ownerHead == NULL)
    {
        ownerHead = newNode;
        newNode->prev = NULL;
    }
    else
    {
        OwnerNode *temp = ownerHead;
        while (temp->next != NULL)
        {
            if(temp->ownerName == ownerName)//NOT WORKING---------------------------------------------------------------
            {
                printf("Error: duplicate name.\n");
                return NULL;
            }
            temp = temp->next;
        }
        if(temp->ownerName == ownerName)
        {
            printf("Error: duplicate name.\n");
            return NULL;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
    return newNode;
}
//
PokemonNode *createPokemonNode(PokemonData *data,int choice)
{
    PokemonNode *node = (PokemonNode*)malloc(sizeof(PokemonNode));
    if (!node)
    {
        printf("Error: malloc failed\n");
        return NULL;
    }
    choice--;
    node->left = NULL;
    node->right = NULL;
    data->id = pokedex[choice].id;
    data->name = strdup(pokedex[choice].name);
    data->TYPE = pokedex[choice].TYPE;
    data->hp = pokedex[choice].hp;
    data->attack = pokedex[choice].attack;
    data->CAN_EVOLVE = pokedex[choice].CAN_EVOLVE;
    node->data = data;
    return node;
}
//
void openPokedexMenu()
{
    printf("Your name: ");
    char *name = getDynamicInput();
    printf("Choose Starter:\n");
    printf("1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    int starter = readIntSafe("Your choice: ");
    int rootID = 0;
    switch (starter)
    {
    case 1: rootID = 1 ;break;
    case 2: rootID = 4; break;
    case 3: rootID = 7; break;
    default:break;
    }
    PokemonData *data = (PokemonData*)malloc(sizeof(PokemonData));
    if (!data)
    {
        printf("Error: malloc failed\n");
        return;
    }
    PokemonNode *root = createPokemonNode(data,rootID);
    OwnerNode *newOwner = createOwner(name,root);
    if (!newOwner)
    {
        printf("Error: malloc failed\n");
        free(data);
        free(root);
        return;
    }
    printf("New Pokedex created for %s with starter %s.",newOwner->ownerName,newOwner->pokedexRoot->data->name);
}
// --------------------------------------------------------------
// PART TWO
// --------------------------------------------------------------
// --------------------------------------------------------------
// 1) ADD POKEMON
// --------------------------------------------------------------
void addPokemon(OwnerNode *owner)
{
    int pokemonID = readIntSafe("Enter ID to add: ");
    if(searchPokemonBFS(owner->pokedexRoot,pokemonID)!=NULL)
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n",pokemonID);
    else
    {
        PokemonData *data = (PokemonData*)malloc(sizeof(PokemonData));
        if (!data)
        {
            printf("Error: malloc failed\n");
            return;
        }
        PokemonNode *newNode = createPokemonNode(data,pokemonID);
        if(insertPokemonNode(owner->pokedexRoot,newNode)!=NULL)
            printf("Pokemon %s (ID %d) added.\n",newNode->data->name,newNode->data->id);
        if (insertPokemonNode(owner->pokedexRoot, newNode) == NULL)
        {
            printf("Failed to insert Pokemon. Releasing memory.\n");
            free(newNode->data->name);
            free(newNode->data);
            free(newNode);
        }
    }
}
//
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode)
{
    if (root == NULL) return NULL;
    if (root->data->id > newNode->data->id)
    {
        if (root->left != NULL)
            return insertPokemonNode(root->left, newNode);
        root->left = newNode;
        if(root->left == NULL)
            return NULL;
    }
    if (root->data->id < newNode->data->id)
    {
        if (root->right != NULL)
            return insertPokemonNode(root->right, newNode);
        root->right = newNode;
        if(root->right == NULL) return NULL;
    }
    return root;
}
//
PokemonNode *searchPokemonBFS(PokemonNode *root, int id)
{
    if (root == NULL) return NULL;
    PokemonQueue* q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q))
    {
        PokemonNode* currentNode = dequeue(q);
        if (currentNode->data->id == id)
        {
            free(q);
            return currentNode;
        }
        if (currentNode->left != NULL)
            enqueue(q, currentNode->left);
        if (currentNode->right != NULL)
            enqueue(q, currentNode->right);
    }
    free(q);
    return NULL;
}
// --------------------------------------------------------------
// 2) DISPLAY POKEDEX
// --------------------------------------------------------------
void displayBFS(PokemonNode *root)
{
    BFSGeneric(root,printPokemonNode);
}
//
void preOrderTraversal(PokemonNode *root)
{
    preOrderGeneric(root,printPokemonNode);
}
//
void inOrderTraversal(PokemonNode *root)
{
    inOrderGeneric(root,printPokemonNode);
}
//
void postOrderTraversal(PokemonNode *root)
{
    postOrderGeneric(root,printPokemonNode);
}
//
void displayAlphabetical(PokemonNode *root)
{
    if (root == NULL) return;
    PokemonQueue* q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q))
    {
        PokemonNode* currentNode = dequeue(q);
        //visit(currentNode);
        if (currentNode->left != NULL)
        {
            enqueue(q, currentNode->left);
        }
        if (currentNode->right != NULL)
        {
            enqueue(q, currentNode->right);
        }
    }
    free(q);
}
// --------------------------------------------------------------
// 3) RELEASE POKEMON BY ID
// --------------------------------------------------------------
void freePokemonNode(PokemonNode *node)
{

    if (node == NULL) {
        // If there isn't data to free get out of the func
        return;
    }
    // Check if the node's data is NULL
    if (node->data == NULL) {
        free(node);
        return;
    }
    // First, free the name only if it's not NULL
    if (node->data->name != NULL) {
        free(node->data->name);
    }
    // Then, free the struct of data
    free(node->data);
    // Finally, free the node itself
    free(node);
}
void freePokemon(OwnerNode *owner)
{
    if (owner->pokedexRoot == NULL) {
        printf("No Pokemon to release.\n");
        return;
    }

    int id = readIntSafe("Enter Pokemon ID to release: ");
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, id);
}
//
PokemonNode *removeNodeBST(PokemonNode *root, const int id)
{ if (root == NULL) {
        printf("[DEBUG] Node with ID %d not found (root is NULL).\n", id);
        return NULL;
    }

    // חיפוש הצומת למחיקה
    if (id < root->data->id)
    {
        printf("[DEBUG] Traversing left: looking for ID %d in left subtree.\n", id);
        root->left = removeNodeBST(root->left, id);
    } else if (id > root->data->id) {
        printf("[DEBUG] Traversing right: looking for ID %d in right subtree.\n", id);
        root->right = removeNodeBST(root->right, id);
    } else {
        // הצומת נמצא
        printf("[DEBUG] Node with ID %d found.\n", id);

        // מקרה 1: הצומת הוא עלה
        if (root->left == NULL && root->right == NULL) {
            printf("[DEBUG] Node with ID %d is a leaf. Deleting node.\n", id);
            freePokemonNode(root);
            return NULL;
        }

        // מקרה 2: הצומת עם ילד ימין בלבד
        if (root->left == NULL) {
            printf("[DEBUG] Node with ID %d has only a right child. Promoting right child.\n", id);
            PokemonNode *temp = root->right;
            free(root->data->name);
            printf("[DEBUG] Freed name.\n");
            free(root->data);
            printf("[DEBUG] Freed data.\n");
            free(root);
            printf("[DEBUG] Freed node.\n");
            return temp;
        }

        // מקרה 2: הצומת עם ילד שמאל בלבד
        if (root->right == NULL) {
            printf("[DEBUG] Node with ID %d has only a left child. Promoting left child.\n", id);
            PokemonNode *temp = root->left;
            free(root->data->name);
            printf("[DEBUG] Freed name.\n");
            free(root->data);
            printf("[DEBUG] Freed data.\n");
            free(root);
            printf("[DEBUG] Freed node.\n");
            return temp;
        }

        // מקרה 3: הצומת עם שני ילדים
        printf("[DEBUG] Node with ID %d has two children. Replacing with inorder successor.\n", id);
        PokemonNode *temp = findMin(root->right);
        printf("[DEBUG] Inorder successor found: ID %d, Name: %s.\n", temp->data->id, temp->data->name);

        // החלף את הנתונים של הצומת הנוכחי בנתוני הצומת המינימלי
        root->data->id = temp->data->id;
        free(root->data->name);
        printf("[DEBUG] Freed name of the current node.\n");
        root->data->name = strdup(temp->data->name);
        root->data->hp = temp->data->hp;
        root->data->attack = temp->data->attack;

        // מחק את הצומת המינימלי מתת-העץ הימני
        printf("[DEBUG] Deleting inorder successor with ID %d.\n", temp->data->id);
        root->right = removeNodeBST(root->right, temp->data->id);
    }

    return root;}
//
PokemonNode* findMin(PokemonNode* root) {
    while (root && root->left != NULL) {
        root = root->left;
    }
    return root;
}
//
PokemonNode *findParent(PokemonNode* root, int id)
{
    if (root == NULL || (root->left == NULL && root->right == NULL))
        return NULL;
    if ((root->left != NULL && root->left->data->id == id) || (root->right != NULL && root->right->data->id == id))
        return root;
    if (id < root->data->id)
        return findParent(root->left, id);
    return findParent(root->right, id);
}
//
PokemonNode *removePokemonByID(PokemonNode *root, int id)
{
    if (!root) {
        printf("No Pokemon in the Pokedex.\n");
        return NULL;
    }

    PokemonNode *newRoot = removeNodeBST(root, id);
    if (newRoot == NULL && searchPokemonBFS(root, id)) {
        printf("Error removing Pokemon with ID %d.\n", id);
    } else {
        printf("Pokemon with ID %d successfully removed.\n", id);
    }

    return newRoot;
}
// --------------------------------------------------------------
// 4) POKEMON FIGHT
// --------------------------------------------------------------
void pokemonFight(OwnerNode *owner)
{
    PokemonNode *root = owner->pokedexRoot;
    int first = readIntSafe("Enter ID of the first Pokemon: ");
    int second = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *fighter1 = searchPokemonBFS(root,first);
    PokemonNode *fighter2 = searchPokemonBFS(root,second);
    if(fighter1 == NULL || fighter2 == NULL)
    {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    const double power1 = fighter1->data->attack*1.5 + fighter1->data->hp*1.2;
    const double power2 = fighter2->data->attack*1.5 + fighter2->data->hp*1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n",fighter1->data->name,power1);
    printf("Pokemon 2: %s (Score = %.2f)\n",fighter2->data->name,power2);
    if (power1 > power2) printf("%s wins!\n" ,fighter1->data->name);
    if (power1 < power2) printf("%s wins!\n" ,fighter2->data->name);
    if (power1 == power2) printf("It’s a tie!\n");
}
// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        //displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    // list owners
    printf("\nExisting Pokedexes:\n");
    int ownerPlace = 0;
    OwnerNode *temp = ownerHead;
    while (temp != NULL)
    {
        ownerPlace++;
        printf("%d. %s\n",ownerPlace,temp->ownerName);
        temp = temp->next;
    }
    int choice = readIntSafe("Choose a Pokedex by number: ");
    OwnerNode *currentOwner = ownerHead;
    for (int i = 1; i < choice; i++)
        currentOwner = currentOwner->next;
    printf("\nEntering %s's Pokedex...\n", currentOwner->ownerName);
    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", currentOwner->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(currentOwner);
            break;
        case 2:
            displayMenu(currentOwner);
            break;
        case 3:
            freePokemon(currentOwner);
            break;
        case 4:
            pokemonFight(currentOwner);
            break;
        case 5:
            //evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}
// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
           // deletePokedex();
            break;
        case 4:
          //  mergePokedexMenu();
            break;
        case 5:
           // sortOwners();
            break;
        case 6:
          //  printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    //freeAllOwners();
    return 0;
}
