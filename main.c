#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
#define MAX_VEHICLES 100
#define MAX_STR      50
#define MIN_YEAR     1886
#define MAX_YEAR     2026
 
/* ANSI escape codes */
#define COLOR_RESET  "\033[0m"
#define COLOR_BRAND  "\033[36m"   /* Cyan    — vehicle brand  */
#define COLOR_MODEL  "\033[33m"   /* Yellow  — vehicle model  */
#define COLOR_YEAR   "\033[35m"   /* Magenta — vehicle year   */
#define COLOR_PRICE  "\033[32m"   /* Green   — vehicle price  */
#define COLOR_ERROR  "\033[31m"   /* Red     — error messages */
 
typedef struct {
    char  brand[MAX_STR];
    char  model[MAX_STR];
    int   year;
    float price;
} Vehicle;
 
Vehicle inventory[MAX_VEHICLES];
int vehicleCount = 0;
 
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
 
static void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
 
static void readString(const char *prompt, char *dest, int maxLen) {
    do {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(dest, maxLen, stdin)) {
            dest[strcspn(dest, "\n")] = '\0'; /* strip trailing newline */
        } else {
            dest[0] = '\0';
        }
        if (dest[0] == '\0') {
            printf(COLOR_ERROR "  Error: Input cannot be empty. "
                   "Try again.\n" COLOR_RESET);
        }
    } while (dest[0] == '\0');
}
 
/*
 * Read an integer year in [MIN_YEAR, MAX_YEAR].
 * Re-prompts on non-integer input or out-of-range values.
 */
static int readYear(const char *prompt) {
    int year;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &year) != 1) {
            clearInputBuffer();
            printf(COLOR_ERROR "  Error: Enter a valid integer "
                   "(e.g., 2020).\n" COLOR_RESET);
            continue;
        }
        clearInputBuffer();
        if (year < MIN_YEAR || year > MAX_YEAR) {
            printf(COLOR_ERROR "  Error: Year must be between "
                   "%d and %d.\n" COLOR_RESET, MIN_YEAR, MAX_YEAR);
            continue;
        }
        return year;
    }
}
 
static float readPrice(const char *prompt) {
    float price;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &price) != 1) {
            clearInputBuffer();
            printf(COLOR_ERROR "  Error: Enter a valid price "
                   "(e.g., 25000.00).\n" COLOR_RESET);
            continue;
        }
        clearInputBuffer();
        if (price <= 0.0f) {
            printf(COLOR_ERROR "  Error: Price must be greater "
                   "than 0.\n" COLOR_RESET);
            continue;
        }
        return price;
    }
}
 
/* ── File I/O ── */
 
void loadFromFile(void) {
    FILE *file = fopen(
        "/Users/vladistodorescu/Programming Project"
        "/Vehicle System/Vehicle System/collection.csv", "r");
    if (!file) {
        printf("No existing database found. Starting fresh.\n");
        return;
    }
 
    vehicleCount = 0;
    while (fscanf(file, " %49[^,],%49[^,],%d,%f\n",
                  inventory[vehicleCount].brand,
                  inventory[vehicleCount].model,
                  &inventory[vehicleCount].year,
                  &inventory[vehicleCount].price) == 4) {
        vehicleCount++;
        if (vehicleCount >= MAX_VEHICLES) break;
    }
 
    fclose(file);
    printf("Successfully loaded %d vehicles.\n", vehicleCount);
}
 
void saveToFile(void) {
    FILE *file = fopen(
        "/Users/vladistodorescu/Programming Project"
        "/Vehicle System/Vehicle System/collection.csv", "w");
    if (!file) {
        printf(COLOR_ERROR "Error: Could not save data!\n" COLOR_RESET);
        return;
    }
 
    for (int i = 0; i < vehicleCount; i++) {
        fprintf(file, "%s,%s,%d,%.2f\n",
                inventory[i].brand,
                inventory[i].model,
                inventory[i].year,
                inventory[i].price);
    }
 
    fclose(file);
}
 
void addVehicle(void) {
    if (vehicleCount >= MAX_VEHICLES) {
        printf(COLOR_ERROR "Inventory full!\n" COLOR_RESET);
        return;
    }
 
    clearScreen();
    printf("--- ADD NEW VEHICLE ---\n");
 
    readString("Brand (e.g., Toyota): ",
               inventory[vehicleCount].brand, MAX_STR);
    readString("Model (e.g., Corolla): ",
               inventory[vehicleCount].model, MAX_STR);
    inventory[vehicleCount].year =
        readYear("Year of manufacture (yyyy, e.g., 2020): ");
    inventory[vehicleCount].price =
        readPrice("Price in $ (e.g., 25000.00): ");
 
    vehicleCount++;
    saveToFile();
    printf("\nVehicle added and saved to collection.csv!\n");
    printf("Press Enter to return...");
    getchar();
}
 
void sellVehicle(void) {
    clearScreen();
    loadFromFile();
 
    if (vehicleCount == 0) {
        printf("The inventory is currently empty.\n");
        printf("\nPress Enter to return...");
        getchar();
        return;
    }
 
    /* Loop so the user can retry after an invalid ID without recursion. */
    while (1) {
        printf("--- SELL VEHICLE ---\n");
        printf("%-4s %-15s %-15s %-6s %-10s\n",
               "ID", "Brand", "Model", "Year", "Price");
        printf("--------------------------------------------"
               "----------------\n");
 
        for (int i = 0; i < vehicleCount; i++) {
            printf("%-4d ", i + 1);
            printf(COLOR_BRAND  "%-15s" COLOR_RESET " ",
                   inventory[i].brand);
            printf(COLOR_MODEL  "%-15s" COLOR_RESET " ",
                   inventory[i].model);
            printf(COLOR_YEAR   "%-6d"  COLOR_RESET " ",
                   inventory[i].year);
            printf(COLOR_PRICE  "$%-10.2f" COLOR_RESET "\n",
                   inventory[i].price);
        }
 
        printf("\nEnter record ID to sell (1-%d), or 0 to cancel: ",
               vehicleCount);
 
        int idToDelete;
        if (scanf("%d", &idToDelete) != 1) {
            clearInputBuffer();
            printf(COLOR_ERROR "Invalid input. Returning to menu.\n"
                   COLOR_RESET);
            return;
        }
        clearInputBuffer();
 
        if (idToDelete == 0) return;
 
        if (idToDelete < 1 || idToDelete > vehicleCount) {
            printf(COLOR_ERROR "\nError: ID %d does not exist. "
                   "Try again.\n\n" COLOR_RESET, idToDelete);
            continue; /* re-display the list and prompt again */
        }
 
        /* Shift remaining records left to fill the gap. */
        for (int i = idToDelete - 1; i < vehicleCount - 1; i++) {
            inventory[i] = inventory[i + 1];
        }
        vehicleCount--;
        saveToFile();
 
        printf("\nVehicle sold successfully and removed from database!\n");
        printf("Press Enter to return...");
        getchar();
        return;
    }
}
 
/* ── Helpers for filter / search ── */
 
static void toLowerStr(const char *src, char *dst, int maxLen) {
    int i;
    for (i = 0; src[i] && i < maxLen - 1; i++)
        dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = '\0';
}
 
static int containsIgnoreCase(const char *haystack, const char *needle) {
    char h[MAX_STR], n[MAX_STR];
    toLowerStr(haystack, h, MAX_STR);
    toLowerStr(needle,   n, MAX_STR);
    return strstr(h, n) != NULL;
}
 
static void printTableHeader(void) {
    printf("%-15s %-15s %-6s %-10s\n",
           "Brand", "Model", "Year", "Price");
    printf("--------------------------------------------------\n");
}
 
static void printVehicle(const Vehicle *v) {
    printf(COLOR_BRAND  "%-15s" COLOR_RESET " ", v->brand);
    printf(COLOR_MODEL  "%-15s" COLOR_RESET " ", v->model);
    printf(COLOR_YEAR   "%-6d"  COLOR_RESET " ", v->year);
    printf(COLOR_PRICE  "$%-10.2f" COLOR_RESET "\n", v->price);
}
 
/* ── New feature: Filter by attribute ── */
 
void filterByAttribute(void) {
    clearScreen();
    loadFromFile();
 
    if (vehicleCount == 0) {
        printf("The inventory is currently empty.\n");
        printf("\nPress Enter to return...");
        getchar();
        return;
    }
 
    printf("--- FILTER BY ATTRIBUTE ---\n");
    printf("[1] Brand\n");
    printf("[2] Model\n");
    printf("[3] Year\n");
    printf("[4] Price (exact)\n");
    printf("[0] Cancel\n");
    printf("--------------------------\n");
    printf("Select attribute: ");
 
    int attr;
    if (scanf("%d", &attr) != 1 || attr < 0 || attr > 4) {
        clearInputBuffer();
        printf(COLOR_ERROR "Invalid selection.\n" COLOR_RESET);
        printf("Press Enter to return...");
        getchar();
        return;
    }
    clearInputBuffer();
    if (attr == 0) return;
 
    char   strQuery[MAX_STR];
    int    intQuery  = 0;
    float  fltQuery  = 0.0f;
    int    found     = 0;
 
    if (attr == 1) readString("Enter brand to filter by: ",    strQuery, MAX_STR);
    if (attr == 2) readString("Enter model to filter by: ",    strQuery, MAX_STR);
    if (attr == 3) intQuery = readYear("Enter year to filter by: ");
    if (attr == 4) fltQuery = readPrice("Enter exact price to filter by: ");
 
    clearScreen();
    printf("--- FILTER RESULTS ---\n");
    printTableHeader();
 
    for (int i = 0; i < vehicleCount; i++) {
        int match = 0;
        if (attr == 1) match = containsIgnoreCase(inventory[i].brand, strQuery);
        if (attr == 2) match = containsIgnoreCase(inventory[i].model, strQuery);
        if (attr == 3) match = (inventory[i].year == intQuery);
        if (attr == 4) match = (inventory[i].price >= fltQuery - 0.01f &&
                                inventory[i].price <= fltQuery + 0.01f);
        if (match) {
            printVehicle(&inventory[i]);
            found++;
        }
    }
 
    if (found == 0)
        printf(COLOR_ERROR "No vehicles matched your filter.\n" COLOR_RESET);
    else
        printf("\n%d vehicle(s) found.\n", found);
 
    printf("\nPress Enter to return...");
    getchar();
}
 
/* ── New feature: View cars below a desired price ── */
 
void viewCarsBelow(void) {
    clearScreen();
    loadFromFile();
 
    if (vehicleCount == 0) {
        printf("The inventory is currently empty.\n");
        printf("\nPress Enter to return...");
        getchar();
        return;
    }
 
    printf("--- VIEW CARS BELOW PRICE ---\n");
    float maxPrice = readPrice("Show cars below $ (e.g., 30000.00): ");
 
    clearScreen();
    printf("--- VEHICLES UNDER $%.2f ---\n", maxPrice);
    printTableHeader();
 
    int found = 0;
    for (int i = 0; i < vehicleCount; i++) {
        if (inventory[i].price < maxPrice) {
            printVehicle(&inventory[i]);
            found++;
        }
    }
 
    if (found == 0)
        printf(COLOR_ERROR "No vehicles found below $%.2f.\n" COLOR_RESET, maxPrice);
    else
        printf("\n%d vehicle(s) found.\n", found);
 
    printf("\nPress Enter to return...");
    getchar();
}
 
/* ── Submenus ───────────────────────────────────────────────────────── */
 
void vehicleInventorySubmenu(void) {
    int option = -1;
    while (option != 0) {
        clearScreen();
        printf("===============================\n");
        printf("  SUBMENU: VEHICLE INVENTORY   \n");
        printf("===============================\n");
        printf("[1] Show All Vehicles\n");
        printf("[2] Search Vehicle by Brand\n");
        printf("[3] Filter by Attribute\n");
        printf("[4] View Cars Below Price\n");
        printf("[0] Back to main menu\n");
        printf("------------------------------\n");
        printf("Select an option: ");
 
        if (scanf("%d", &option) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
 
        if (option == 1) {
            clearScreen();
            loadFromFile();
            printf("%-15s %-15s %-6s %-10s\n",
                   "Brand", "Model", "Year", "Price");
            printf("--------------------------------------------------\n");
            for (int i = 0; i < vehicleCount; i++) {
                printf(COLOR_BRAND  "%-15s" COLOR_RESET " ",
                       inventory[i].brand);
                printf(COLOR_MODEL  "%-15s" COLOR_RESET " ",
                       inventory[i].model);
                printf(COLOR_YEAR   "%-6d"  COLOR_RESET " ",
                       inventory[i].year);
                printf(COLOR_PRICE  "$%-10.2f" COLOR_RESET "\n",
                       inventory[i].price);
            }
            printf("\nPress Enter to return...");
            getchar();
        } else if (option == 2) {
            clearScreen();
            loadFromFile();
 
            if (vehicleCount == 0) {
                printf("The inventory is currently empty.\n");
                printf("\nPress Enter to return...");
                getchar();
                continue;
            }
 
            char query[MAX_STR];
            readString("Enter brand to search for: ", query, MAX_STR);
 
            clearScreen();
            printf("--- SEARCH RESULTS FOR \"%s\" ---\n", query);
            printTableHeader();
 
            int found = 0;
            for (int i = 0; i < vehicleCount; i++) {
                if (containsIgnoreCase(inventory[i].brand, query)) {
                    printVehicle(&inventory[i]);
                    found++;
                }
            }
 
            if (found == 0)
                printf(COLOR_ERROR "No vehicles matched \"%s\".\n" COLOR_RESET, query);
            else
                printf("\n%d vehicle(s) found.\n", found);
 
            printf("\nPress Enter to return...");
            getchar();
        } else if (option == 3) {
            filterByAttribute();
        } else if (option == 4) {
            viewCarsBelow();
        }
    }
}
 
/* ── Main menu ── */
 
void displayMainMenu(void) {
    int choice = -1;
    loadFromFile();
 
    while (choice != 0) {
        clearScreen();
        printf("*********************************\n");
        printf("  VEHICLE BUYING/SELLING SYSTEM  \n");
        printf("*********************************\n");
        printf("[1] Sell Vehicle\n");
        printf("[2] Add Vehicle\n");
        printf("[3] Vehicle Inventory\n");
        printf("[0] Exit\n");
        printf("*********************************\n");
        printf("Choose an option: ");
 
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();
 
        switch (choice) {
            case 1: sellVehicle();              break;
            case 2: addVehicle();               break;
            case 3: vehicleInventorySubmenu();  break;
            case 0: printf("\nClosing App...\n"); break;
            default:
                printf(COLOR_ERROR "\nInvalid option!\n" COLOR_RESET);
                printf("Press Enter to continue...");
                getchar();
        }
    }
}
 
int main(void) {
    displayMainMenu();
    return 0;
}
