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
// --------------------------------------------------------------
// GENERIC FUNCTIONS
// --------------------------------------------------------------
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit){}
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
            temp = temp->next;
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
    data->id = pokedex[choice].id;
    data->name = pokedex[choice].name;
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
    PokemonNode *root = createPokemonNode(data,rootID-1);
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
    }
}
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode)
{
    if (root == NULL)
    {
        return NULL;
    }
    if (root->data->id > newNode->data->id)
    {
        if (root->left != NULL)
            return insertPokemonNode(root->left, newNode);
        root->left = newNode;
        if(root->left == NULL)
            return NULL;
        return root;
    }
    if (root->data->id < newNode->data->id)
    {
        if (root->right != NULL)
            return insertPokemonNode(root->right, newNode);
        root->right = newNode;
        if(root->right == NULL)
            return NULL;
        return root;
    }
}
PokemonNode *searchPokemonBFS(PokemonNode *root, int id)//FIX IT TO BFS//////////////////////////////////
{
    if (root == NULL)
        return NULL;
    if (root->data->id == id)
        return root;
    if (root->data->id > id)
        return searchPokemonBFS(root->left, id);
    if (root->data->id < id)
        return searchPokemonBFS(root->right, id);
}
// --------------------------------------------------------------
// 2) DISPLAY POKEDEX
// --------------------------------------------------------------
void displayBFS(PokemonNode *root)
{
    char *type = (char*)malloc(sizeof(char) * 20);
    if(!type)
    {
        printf("Error: malloc failed\n");
        return;
    }
    *type = pokedex[root->data->id-1].TYPE;
    char *canEvolve = (char*)malloc(sizeof(char) * 20);
    if(!canEvolve)
    {
        printf("Error: malloc failed\n");
        free(type);
        return;
    }
    *canEvolve = pokedex[root->data->id-1].TYPE;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",root->data->id,root->data->name,
        type,root->data->hp,root->data->attack,canEvolve);
    free(type);
    free(canEvolve);
}
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
void displayAlphabetical(PokemonNode *root){}
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
        //displayBFS(owner->pokedexRoot);
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
            //displayMenu(cur);
            break;
        case 3:
            //freePokemon(cur);
            break;
        case 4:
            //pokemonFight(cur);
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
