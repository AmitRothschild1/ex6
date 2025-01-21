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
}
/* ------------------------------------------------------------
   QUEUE FUNCTIONS
   ------------------------------------------------------------ */
PokemonQueue* createQueue()
{
    PokemonQueue* q = (PokemonQueue*)malloc(sizeof(PokemonQueue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}
//
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
// --------------------------------------------------------------
// PART ONE
// --------------------------------------------------------------

//
OwnerNode *createOwner(char *ownerName, PokemonNode *starter)
{
    OwnerNode *newNode = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (!newNode)
    {
        printf("Error: malloc failed\n");
        return NULL;
    }
    //if is name exist
    newNode->ownerName = myStrdup(ownerName);
    if (!newNode->ownerName) {
        printf("Error: memory allocation failed for ownerName.\n");
        free(newNode);
        return NULL;
    }
    newNode->pokedexRoot = starter;
    if (ownerHead == NULL)
    {
        newNode->next = newNode->prev = newNode;
        ownerHead = newNode;
    }
    else
    {
        OwnerNode *temp = ownerHead;

        temp = ownerHead->prev; // הצומת האחרון
        temp->next = newNode;
        newNode->prev = temp;
        newNode->next = ownerHead;
        ownerHead->prev = newNode;
    }
    return newNode;
}


int isNameExist(char *ownerName)
{
    if (ownerHead == NULL) return 0;
    OwnerNode *temp = ownerHead;
    do
    {
        if(!strcmp(temp->ownerName,ownerName))
        {
            return 1;
        }
        temp = temp->next;
    }while (temp->next != ownerHead);
    return 0;
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
    data->name = myStrdup(pokedex[choice].name);
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
    if (isNameExist(name))
    {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n",name);
        free(name);
        return;
    }
    printf("Choose Starter:\n");
    printf("1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    int starter = readIntSafe("Your choice: ");
    int rootID = 0;
    switch (starter)
    {
    case 1: rootID = 1 ;break;
    case 2: rootID = 4; break;
    case 3: rootID = 7; break;
    default:
        printf("Invalid starter choice.\n");
        free(name);
        return;
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
        free(data);
        free(root);
        return;
    }
    printf("New Pokedex created for %s with starter %s.",newOwner->ownerName,newOwner->pokedexRoot->data->name);
}
//

// --------------------------------------------------------------
// PART TWO
// --------------------------------------------------------------
// --------------------------------------------------------------
// 1) ADD POKEMON
// --------------------------------------------------------------
void addPokemon(OwnerNode *owner)
{
    int pokemonID = readIntSafe("Enter ID to add: ");
    if (pokemonID<1 || pokemonID>151)
    {
        printf("Error: pokemonID is out of range.\n");
        return;
    }
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
        PokemonNode *inserted = insertPokemonNode(owner->pokedexRoot,newNode);
        if(owner->pokedexRoot == NULL)
        {
            owner->pokedexRoot = inserted;
        }
        if(inserted!=NULL)
            printf("Pokemon %s (ID %d) added.\n",newNode->data->name,newNode->data->id);
        if (inserted == NULL)
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
    //if (root == NULL) return NULL;
    if (root == NULL)
    {
        root = newNode;
        return root;
    }
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
    if (!root)
    {
        printf("No Pokemon to display.\n");
        return;
    }
    NodeArray na;
    initNodeArray(&na, 10);

    collectAll(root, &na);

    qsort(na.nodes, na.size, sizeof(PokemonNode *), compareByNameNode);

    // הדפסת הצמתים במערך
    printf("Pokemon (Alphabetical Order):\n");
    for (int i = 0; i < na.size; i++)
    {
        printPokemonNode(na.nodes[i]);
    }

    // שחרור זיכרון של המערך
    free(na.nodes);
}
int compareByNameNode(const void *a, const void *b)
{
    const PokemonNode *nodeA = *(const PokemonNode **)a;
    const PokemonNode *nodeB = *(const PokemonNode **)b;

    return strcmp(nodeA->data->name, nodeB->data->name);
}
//
void collectAll(PokemonNode *root, NodeArray *na)
{
    if (!root || !na) return;
    collectAll(root->left, na);
    addNode(na, root);
    collectAll(root->right, na);
}
//
void initNodeArray(NodeArray *na, int cap)
{
    if (!na || cap <= 0)return;

    na->nodes = (PokemonNode **)malloc(cap * sizeof(PokemonNode *));
    if (!na->nodes)
    {
        printf("Memory allocation failed for NodeArray.\n");
        na->size = 0;
        na->capacity = 0;
        return;
    }

    na->size = 0;
    na->capacity = cap;
}
//
void addNode(NodeArray *na, PokemonNode *node)
{
    if (!na || !node)
    {
        printf("Invalid parameters for adding a node.\n");
        return;
    }

    if (na->size == na->capacity)
    {
        na->capacity *= 2;
        na->nodes = (PokemonNode **)realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
        if (!na->nodes)
        {
            printf("Memory allocation failed during resize.\n");
            na->size = 0;
            na->capacity = 0;
            return;
        }
    }
    na->nodes[na->size++] = node;
}
// --------------------------------------------------------------
// 3) RELEASE POKEMON BY ID
// --------------------------------------------------------------
void freePokemonNode(PokemonNode *node)
{

    if (node == NULL) {
        return;
    }
    if (node->data == NULL) {
        free(node);
        return;
    }
    if (node->data->name != NULL) {
        free(node->data->name);
    }
    free(node->data);
    free(node);
}
void freePokemon(OwnerNode *owner)
{
    if (owner->pokedexRoot == NULL)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    int id = readIntSafe("Enter Pokemon ID to release: ");
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, id);
}
//
PokemonNode *removeNodeBST(PokemonNode *root, const int id)
{
    if (root == NULL)
    {
        printf("No Pokemon with ID %d found.\n", id);
        return NULL;
    }
    if (id < root->data->id)
    {
        root->left = removeNodeBST(root->left, id);
    }
    else if (id > root->data->id)
    {
        root->right = removeNodeBST(root->right, id);
    }
    else
    {
        if (root->left == NULL && root->right == NULL)
        {
            freePokemonNode(root);
            return NULL;
        }
        if (root->left == NULL)
        {
            PokemonNode *temp = root->right;
            free(root->data->name);
            free(root->data);
            free(root);
            return temp;
        }
        if (root->right == NULL)
        {
            PokemonNode *temp = root->left;
            free(root->data->name);
            free(root->data);
            free(root);
            return temp;
        }

        PokemonNode *temp = findMin(root->right);
        root->data->id = temp->data->id;
        free(root->data->name);
        root->data->name = myStrdup(temp->data->name);
        root->data->hp = temp->data->hp;
        root->data->attack = temp->data->attack;
        root->data->CAN_EVOLVE = temp->data->CAN_EVOLVE;
        root->right = removeNodeBST(root->right, temp->data->id);
    }
    return root;
}
//
PokemonNode* findMin(PokemonNode* root)
{
    if (root == NULL) return NULL;
    while (root && root->left != NULL)
    {
        root = root->left;
    }
    return root;
}
//
PokemonNode *removePokemonByID(PokemonNode *root, int id)
{
    if (!root)
    {
        printf("No Pokemon to release.\n");
        return NULL;
    }
    PokemonNode *wanted = searchPokemonBFS(root, id);
    if (wanted == NULL)
    {
        printf("No Pokemon with ID %d found.\n", id);
        return wanted;
    }
    char *pokemonName = myStrdup(wanted->data->name);
    if (!pokemonName) {
        printf("[ERROR] Memory allocation failed for name backup.\n");
        return root;
    }
    PokemonNode *newRoot = removeNodeBST(root, id);
    if (newRoot == NULL)
    {
        if (root->left == NULL && root->right == NULL)
        {
            printf("Removing Pokemon %s (ID %d).\n",pokemonName,id);
            return newRoot;
        }
        printf("Error removing Pokemon with ID %d.\n", id);
        return NULL;
    }
    printf("Removing Pokemon %s (ID %d).\n",newRoot->data->name,id);
    return newRoot;
}
// --------------------------------------------------------------
// 4) POKEMON FIGHT
// --------------------------------------------------------------
void pokemonFight(OwnerNode *owner)
{
    if (owner->pokedexRoot == NULL)
    {
        printf("Pokedex is empty.\n");
        return;
    }
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
    if (power1 == power2) printf("It's a tie!\n");
}
// --------------------------------------------------------------
// 5) EVOLVE POKEMON
// --------------------------------------------------------------
void evolvePokemon(OwnerNode *owner)
{
    if (owner->pokedexRoot == NULL)
    {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode *wanted = searchPokemonBFS(owner->pokedexRoot,id);
    if (wanted == NULL)
    {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }
    if (wanted->data->CAN_EVOLVE == 0)
    {
        printf("Can not evolve.\n");
        return;
    }
    PokemonNode *evolved = searchPokemonBFS(owner->pokedexRoot,id+1);
    if(evolved!=NULL)
    {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
            id+1,evolved->data->name,wanted->data->name,id);
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot,id);
        return;
    }
    PokemonData *data = (PokemonData*)malloc(sizeof(PokemonData));
    PokemonNode *temp = createPokemonNode(data,id+1);
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",wanted->data->name,id,temp->data->name,id+1);
    wanted->data->id = temp->data->id;
    free(wanted->data->name);
    wanted->data->name = myStrdup(temp->data->name);
    wanted->data->hp = temp->data->hp;
    wanted->data->attack = temp->data->attack;
    wanted->data->CAN_EVOLVE = temp->data->CAN_EVOLVE;
    freePokemonNode(temp);
}
// --------------------------------------------------------------
// PART THREE
// --------------------------------------------------------------
void deletePokedex()
{
    if(ownerHead == NULL)
    {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("=== Delete a Pokedex ===\n");
    OwnerNode *temp = ownerHead;
    int ownerPlace = 0;
    ownerPlace++;
    printf("%d. %s\n",ownerPlace,temp->ownerName);
    temp = temp->next;
    while (temp != ownerHead)
    {
        ownerPlace++;
        printf("%d. %s\n",ownerPlace,temp->ownerName);
        temp = temp->next;
    }
    temp = ownerHead;
    int ownerNum = readIntSafe("Choose a Pokedex to delete by number: ");
    for (int i = 1; i < ownerNum; i++)
    {
        temp = ownerHead->next;
    }
    if (temp == NULL)//NOT NULL ANYMORE
    {
        printf("Invalid number.\n");
    }
    else
    {
        printf("Deleting %s's entire Pokedex...\n",temp->ownerName);
        freeOwnerNode(temp);
        printf("Pokedex deleted.\n");
    }
}
//
void freeOwnerNode(OwnerNode *owner)
{
    if (!ownerHead || !owner) {
        return;
    }

    if (ownerHead == owner && ownerHead->next == ownerHead) {
        free(owner->ownerName);
        freePokemonTree(owner->pokedexRoot);
        free(owner);
        ownerHead = NULL;
        return;
    }

    if (ownerHead == owner) {
        ownerHead = owner->next;
    }

    owner->prev->next = owner->next;
    owner->next->prev = owner->prev;

    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    free(owner);
}
//
void freePokemonTree(PokemonNode *root)
{
    postOrderGeneric(root,freePokemonNode);
}
// --------------------------------------------------------------
// PART FOUR
// --------------------------------------------------------------
void mergePokedexMenu(void)
{
    if (ownerHead == NULL)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *name1 = getDynamicInput();
    printf("Enter name of second owner: ");
    char *name2 = getDynamicInput();
    OwnerNode *first = findOwnerByName(name1);
    OwnerNode *second = findOwnerByName(name2);
    if (first == NULL || second == NULL)
    {
        printf("Names have not been found.\n");
        return;
    }
    printf("Merging %s and %s...\n", name1, name2);
    copyPokedex(second->pokedexRoot, &first->pokedexRoot);
    removeOwnerFromCircularList(second);

    printf("Owner '%s' has been removed after merging.\n",name2);
}
//
OwnerNode *findOwnerByName(const char *name)
{
    if (!ownerHead || !name) return NULL;

    OwnerNode *current = ownerHead;
    do
    {
        if (strcmp(current->ownerName, name) == 0) return current;
        current = current->next;
    }
    while (current != ownerHead);

    return NULL;
}
//
void copyPokedex(PokemonNode *root, PokemonNode **targetRoot)
{
    if (!root) {
        return;
    }

    PokemonNode *newNode = malloc(sizeof(PokemonNode));
    if (!newNode) {
        printf("Memory allocation error while copying Pokedex.\n");
        return;
    }
    newNode->data = malloc(sizeof(PokemonData));
    if (!newNode->data)
    {
        printf("Memory allocation error for PokemonData.\n");
        free(newNode);
        return;
    }
    newNode->data->id = root->data->id;
    newNode->data->name = myStrdup(root->data->name);
    newNode->data->attack = root->data->attack;
    newNode->data->hp = root->data->hp;
    newNode->data->TYPE = root->data->TYPE;
    newNode->data->CAN_EVOLVE = root->data->CAN_EVOLVE;
    newNode->left = newNode->right = NULL;

    if (*targetRoot == NULL)
    {
        *targetRoot = newNode;
    }
    else if (searchPokemonBFS(*targetRoot, newNode->data->id) != NULL)
    {
        return;
    }
    else
        {
        PokemonNode *temp = *targetRoot;
        while (1) {
            if (newNode->data->id < temp->data->id) {
                if (!temp->left) {
                    temp->left = newNode;
                    break;
                }
                temp = temp->left;
            } else {
                if (!temp->right) {
                    temp->right = newNode;
                    break;
                }
                temp = temp->right;
            }
        }
    }

    copyPokedex(root->left, targetRoot);
    copyPokedex(root->right, targetRoot);
}
// --------------------------------------------------------------
// PART FIVE
// --------------------------------------------------------------
void sortOwners()
{
    if (!ownerHead || ownerHead->next == ownerHead)
    {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }

    int swapped;
    OwnerNode *current;
    OwnerNode *lastSorted = NULL;

    do
    {
        swapped = 0;
        current = ownerHead;

        while (current->next != lastSorted && current->next != ownerHead)
        {
            if (strcmp(current->ownerName, current->next->ownerName) > 0)
            {
                swapOwnerData(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
        lastSorted = current;
    }
    while (swapped);
    printf("Owners sorted by name.\n");
}
//
void swapOwnerData(OwnerNode *a, OwnerNode *b)
{
    if (!a || !b) return;

    char *tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;

    struct PokemonNode *tempPokedex = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempPokedex;
}
// --------------------------------------------------------------
// PART SIX
// --------------------------------------------------------------
void printOwnersCircular()
{
    if (!ownerHead) {
        printf("No owners.\n");
        return;
    }
    printf("Enter direction (F or B):");
    char *direction = getDynamicInput();
    int printsNum = readIntSafe("How many prints? ");
    if (printsNum <= 0) {
        printf("Number of prints must be greater than zero.\n");
        return;
    }
    OwnerNode *current = ownerHead;
    for (int i = 0; i < printsNum; i++) {
        printf("[%d] %s\n", i + 1, current->ownerName);

        if (*direction == 'F' || *direction == 'f') {
            current = current->next;
        } else { // direction == 'B'
            current = current->prev;
        }
    }

}
//
void linkOwnerInCircularList(OwnerNode *newOwner)
{
    if (!newOwner) {
        return;
    }

    if (!ownerHead) {
        // רשימה ריקה - הצומת החדש יהיה הראשון והיחיד
        ownerHead = newOwner;
        newOwner->next = newOwner->prev = newOwner; // מעגל סגור
        return;
    }

    // הוספה לסוף הרשימה המעגלית
    OwnerNode *tail = ownerHead->prev; // הצומת האחרון
    tail->next = newOwner;
    newOwner->prev = tail;
    newOwner->next = ownerHead;
    ownerHead->prev = newOwner;

}
void removeOwnerFromCircularList(OwnerNode *target)
{
    if (!ownerHead || !target) {
        return;
    }

    // מקרה: רשימה עם צומת יחיד
    if (ownerHead == target && ownerHead->next == ownerHead) {
        free(target->ownerName);
        free(target);
        ownerHead = NULL;
        return;
    }

    // עדכון קישורים
    target->prev->next = target->next;
    target->next->prev = target->prev;

    // עדכון ראש הרשימה אם יש צורך
    if (ownerHead == target) {
        ownerHead = target->next;
    }

    // שחרור זיכרון
    free(target->ownerName);
    free(target);
}
// --------------------------------------------------------------
// PART SEVEN
// --------------------------------------------------------------
void freeAllOwners()
{
    while (ownerHead) {
        OwnerNode *current = ownerHead;

        // עדכון ראש הרשימה
        if (ownerHead->next == ownerHead) {
            ownerHead = NULL; // רשימה עם צומת יחיד
        } else {
            ownerHead->prev->next = ownerHead->next;
            ownerHead->next->prev = ownerHead->prev;
            ownerHead = ownerHead->next;
        }

        // שחרור הצומת הנוכחי
        free(current->ownerName);
        freePokemonTree(current->pokedexRoot);
        free(current);
    }
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
        displayAlphabetical(owner->pokedexRoot);
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
    if (ownerHead == NULL)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    int ownerPlace = 0;
    OwnerNode *temp = ownerHead;
        ownerPlace++;
        printf("%d. %s\n",ownerPlace,temp->ownerName);
        temp = temp->next;
        while (temp != ownerHead)
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
            evolvePokemon(currentOwner);
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
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
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
    freeAllOwners();
    return 0;
}
