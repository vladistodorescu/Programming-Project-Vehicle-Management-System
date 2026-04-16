#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VEHICLES 100
#define MAX_STR 50

typedef struct {
    char brand[MAX_STR];
    char model[MAX_STR];
    int year;
    float price;
} Vehicle;

Vehicle inventory[MAX_VEHICLES];
int vehicleCount = 0;

// Helper to clear screen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Read from CSV files
void loadFromFile() {
    FILE *file = fopen("/Users/vladistodorescu/Programming Project/Vehicle System/Vehicle System/collection.csv", "r");
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

// Write to CSV files
void saveToFile() {
    FILE *file = fopen("/Users/vladistodorescu/Programming Project/Vehicle System/Vehicle System/collection.csv", "w");
    if (!file) {
        printf("Error: Could not save data!\n");
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

// Add Vehicle to Collection
void addVehicle() {
    if (vehicleCount >= MAX_VEHICLES) {
        printf("Inventory full!\n");
        return;
    }

    clearScreen();
    printf("--- ADD NEW VEHICLE ---\n");
    printf("Enter Brand: ");
    scanf(" %49[^\n]", inventory[vehicleCount].brand);
    printf("Enter Model: ");
    scanf(" %49[^\n]", inventory[vehicleCount].model);
    printf("Enter Year: ");
    scanf("%d", &inventory[vehicleCount].year);
    printf("Enter Price: ");
    scanf("%f", &inventory[vehicleCount].price);

    vehicleCount++;
    saveToFile(); // Auto-save after adding
    printf("\nVehicle added and saved to collection.csv!\n");
    getchar(); getchar();
    
}

// Remove Vehicle from Collection
void sellVehicle() {
    int IDtoDelete = -1;
    clearScreen();
    loadFromFile();
    
    if (vehicleCount == 0) {
        printf("The inventory is currently empty.\n");
        printf("\nPress Enter to return...");
        getchar(); getchar();
        return;
    }

    printf("--- SELL VEHICLE ---\n");
    printf("%-4s %-15s %-15s %-6s %-10s\n", "ID", "Brand", "Model", "Year", "Price");
    printf("------------------------------------------------------------\n");
    
    // Show all cars with their ID
    for(int i = 0; i < vehicleCount; i++) {
        printf("%-4d %-15s %-15s %-6d $%-10.2f\n",
               i + 1, inventory[i].brand, inventory[i].model,
               inventory[i].year, inventory[i].price);
    }
    
    printf("\nPlease select the record ID to delete (or 0 to cancel): ");
    if (scanf("%d", &IDtoDelete) != 1) {
        printf("Invalid input. Returning to menu.\n");
        while (getchar() != '\n');
        return;
    }
    
    // If User cancels
    if (IDtoDelete == 0) {
        return;
    }

    // Check if the ID exists (ID is 1 to vehicleCount)
    if (IDtoDelete <= -1 || IDtoDelete > vehicleCount) {
        printf("\nError: ID %d does not exist. Press Enter to try again.", IDtoDelete);
        getchar(); getchar();
        sellVehicle(); // Recursive call to try again
    }
    else {
        // Deletion
        for (int i = IDtoDelete - 1; i < vehicleCount - 1; i++) {
            inventory[i] = inventory[i + 1];
        }

        vehicleCount--;

        saveToFile();

        printf("\nVehicle sold successfully and removed from database!\n");
        printf("Press Enter to return...");
        getchar(); getchar();
    }
}

void vehicleInventorySubmenu() {
    int option = -1;
    while (option != 0) {
        clearScreen();
        printf("===============================\n");
        printf("  SUBMENU: VEHICLE INVENTORY   \n");
        printf("===============================\n");
        printf("[1] Show All Vehicles\n");
        printf("[2] Search Vehicle by Brand\n");
        printf("[0] Back to main menu\n");
        printf("------------------------------\n");
        printf("Select an option: ");

        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n'); continue;
        }

        if (option == 1) {
            clearScreen();
            loadFromFile();
            printf("%-15s %-15s %-6s %-10s\n", "Brand", "Model", "Year", "Price");
            printf("--------------------------------------------------\n");
            for(int i=0; i < vehicleCount; i++) {
                printf("%-15s %-15s %-6d $%-10.2f\n",
                       inventory[i].brand, inventory[i].model,
                       inventory[i].year, inventory[i].price);
            }
            printf("\nPress Enter to return...");
            getchar(); getchar();
        }
    }
}

void displayMainMenu() {
    int choice = -1;
    loadFromFile(); // Initial load when app starts

    while (choice != 0) {
        clearScreen();
        printf("*********************************\n");
        printf("  VEHICLE BUYING/SELLING SYSTEM   \n");
        printf("*********************************\n");
        printf("[1] Sell Vehicle\n");
        printf("[2] Add Vehicle\n");
        printf("[3] Vehicle Inventory\n");
        printf("[0] Exit\n");
        printf("******************************\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); continue;
        }

        switch (choice) {
            case 1:
                sellVehicle();
                break;
            case 2:
                addVehicle();
                break;
            case 3:
                vehicleInventorySubmenu();
                break;
            case 0:
                printf("\nClosing App...\n");
                break;
            default:
                printf("\nInvalid Option!");
                getchar();
                getchar();
        }
    }
}

int main() {
    displayMainMenu();
    return 0;
}
