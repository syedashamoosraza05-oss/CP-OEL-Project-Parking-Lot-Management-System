#include <stdio.h>
#include <string.h>
#include "lookup_utils.h"
#include "input_utils.h"      /* readString, MAX_SLOTS, MEMBER_PASSWORD, colours */

const int RATES[4] = {0, 50, 20, 100};
const char *TYPE_NAMES[4] = {"", "Car", "Bike", "Truck"};

/* Every 5th slot (5, 10, 15...) is reserved for members. */
int isMemberSlot(int slot) {
    return slot % 5 == 0;
}

/* Returns index of vehicle in given slot, or -1 if the slot is empty. */
int findBySlot(Vehicle *v, int slot) {
    for (int i = 0; i < MAX_SLOTS; i++)
        if (v[i].slotNo == slot) return i;
    return -1;
}

/* Returns index of vehicle with given plate, or -1. */
int findByPlate(Vehicle *v, const char *plate) {
    for (int i = 0; i < MAX_SLOTS; i++)
        if (v[i].slotNo != 0 && strcmp(v[i].plate, plate) == 0) return i;
    return -1;
}
void printHeader(void) {
    printf(CYAN "\n%-6s %-14s %-20s %-7s %-8s %-17s\n",
           "Slot", "Plate", "Owner", "Type", "Class", "Entry");
    printf("-----------------------------------------------------------------------------\n" RESET);
}

void printVehicle(const Vehicle *v) {
    if (v->isMember) printf(GREEN);
    printf("%-6d %-14s %-20s %-7s %-8s %02d/%02d/%04d %02d:%02d\n",
           v->slotNo, v->plate, v->owner, TYPE_NAMES[v->type],
           v->isMember ? "Member" : "Regular",
           v->entryDay, v->entryMonth, v->entryYear, v->entryHour, v->entryMin);
    if (v->isMember) printf(RESET);
}

/* Shows all empty slots (regular = green, member = yellow). */
void showEmptySlots(Vehicle *v) {
    printf("\nEmpty regular slots: ");
    for (int s = 1; s <= MAX_SLOTS; s++)
        if (!isMemberSlot(s) && findBySlot(v, s) == -1)
            printf(GREEN "%d " RESET, s);

    printf("\nEmpty member slots : ");
    for (int s = 1; s <= MAX_SLOTS; s++)
        if (isMemberSlot(s) && findBySlot(v, s) == -1)
            printf(YELLOW "%d " RESET, s);
    printf("\n\n");
}

/* Asks for the membership password (3 attempts).
   Returns 1 if verified, 0 otherwise. */
int verifyMembership(void) {
    char pass[50];
    for (int tries = 1; tries <= 3; tries++) {
        readString("Enter membership password: ", pass, sizeof(pass));
        if (strcmp(pass, MEMBER_PASSWORD) == 0) {
            printf(GREEN "Membership verified.\n" RESET);
            return 1;
        }
        printf(RED "Wrong membership password. (%d/3)\n" RESET, tries);
    }
    return 0;
}
