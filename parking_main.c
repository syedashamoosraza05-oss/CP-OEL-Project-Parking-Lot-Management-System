#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "input_utils.h"
#include "date_time_utils.h"
#include "lookup_utils.h"
#include "payment_utils.h"

/* ADD: park a new vehicle with validation. */
void addVehicle(Vehicle *v, int *count) {
    if (*count >= MAX_SLOTS) {
        printf(RED "Parking lot is full!\n" RESET);
        return;
    }
    Vehicle nv;

    nv.isMember = readInt(GREEN "Is the owner a member? (1=Yes, 0=No): " RESET, 0, 1);

    /* members must enter the membership password */
    if (nv.isMember == 1 && !verifyMembership()) {
        printf(YELLOW "Membership could not be verified. Continuing as a regular customer.\n" RESET);
        nv.isMember = 0;
    }

    showEmptySlots(v);

    nv.slotNo = readInt("Slot number: ", 1, MAX_SLOTS);
    if (findBySlot(v, nv.slotNo) != -1) {
        printf(RED "Slot %d is already occupied.\n" RESET, nv.slotNo);
        return;
    }
    if (isMemberSlot(nv.slotNo) && nv.isMember == 0) {
        printf(RED "Slot %d is reserved for members.\n" RESET, nv.slotNo);
        return;
    }
    /* plate: 6-7 chars, first 3 letters, rest digits */
    int plateOk;
    do {
        readString("Number plate (e.g. ABC123 or ABC1234): ", nv.plate, sizeof(nv.plate));
        toUpperStr(nv.plate);
        int len = strlen(nv.plate);
        plateOk = (len >= 6 && len <= 7);
        for (int i = 0; plateOk && i < len; i++) {
            if (i < 3 && !isalpha((unsigned char)nv.plate[i])) plateOk = 0;
            if (i >= 3 && !isdigit((unsigned char)nv.plate[i])) plateOk = 0;
        }
        if (!plateOk)
            printf(RED "  Invalid plate. Use 6-7 characters: 3 letters followed by 3-4 digits.\n" RESET);
    } while (!plateOk);
    if (findByPlate(v, nv.plate) != -1) {
        printf(RED "This vehicle is already parked.\n" RESET);
        return;
    }
    readString("Owner name: ", nv.owner, sizeof(nv.owner));
    nv.type = readInt("Type (1=Car, 2=Bike, 3=Truck): ", 1, 3);
    readDate("Date of entry:", &nv.entryDay, &nv.entryMonth, &nv.entryYear);
    readTime("Entry time:", &nv.entryHour, &nv.entryMin);

    v[nv.slotNo - 1] = nv;          /* vehicle goes in its own slot */
    (*count)++;
    printf(GREEN "Vehicle parked successfully in slot %d.\n" RESET, nv.slotNo);
}

/* DISPLAY: show all parked vehicles in a table. */
void displayAll(Vehicle *v, int count) {
    if (count == 0) {
        printf(YELLOW "No vehicles parked.\n" RESET);
        return;
    }
    printHeader();
    for (int i = 0; i < MAX_SLOTS; i++)
        if (v[i].slotNo != 0) printVehicle(&v[i]);
}

/* SEARCH: by slot number or number plate. */
void searchVehicle(Vehicle *v, int count) {
    int choice = readInt("Search by 1=Slot, 2=Plate: ", 1, 2);
    int idx;
    if (choice == 1) {
        idx = findBySlot(v, readInt("Slot number: ", 1, MAX_SLOTS));
    } else {
        char plate[20];
        readString("Number plate: ", plate, sizeof(plate));
        toUpperStr(plate);
        idx = findByPlate(v, plate);
    }
    if (idx == -1) {
        printf(RED "Record not found.\n" RESET);
    } else {
        printHeader();
        printVehicle(&v[idx]);
    }
}

/* UPDATE: edit owner name, vehicle type or entry date/time of an existing record. */
void updateVehicle(Vehicle *v, int count) {
    int idx = findBySlot(v, readInt("Slot number to update: ", 1, MAX_SLOTS));
    if (idx == -1) {
        printf(RED "Record not found.\n" RESET);
        return;
    }
    printHeader();
    printVehicle(&v[idx]);
    int choice = readInt("Update 1=Owner, 2=Type, 3=Entry date/time: ", 1, 3);
    if (choice == 1) {
        readString("New owner name: ", v[idx].owner, sizeof(v[idx].owner));
    } else if (choice == 2) {
        v[idx].type = readInt("New type (1=Car, 2=Bike, 3=Truck): ", 1, 3);
    } else {
        readDate("New date of entry:", &v[idx].entryDay, &v[idx].entryMonth, &v[idx].entryYear);
        readTime("New entry time:", &v[idx].entryHour, &v[idx].entryMin);
    }
    printf(CYAN "Record updated.\n" RESET);
}

/* DELETE: check a vehicle out, calculate fee, record the payment,
   free only that slot. Works for stays of any length (even many days).
   Members get MEMBER_DISCOUNT percent off the original fee. */
void deleteVehicle(Vehicle *v, int *count) {
    int idx = findBySlot(v, readInt("Slot number to check out: ", 1, MAX_SLOTS));
    if (idx == -1) {
        printf(RED "Record not found.\n" RESET);
        return;
    }
    int exitD, exitMo, exitY, exitH, exitM;
    printf("Date of entry: %02d/%02d/%04d\n", v[idx].entryDay, v[idx].entryMonth, v[idx].entryYear);
    readDate("Date of exit:", &exitD, &exitMo, &exitY);
    readTime("Exit time:", &exitH, &exitM);
    printf("Entry time: %02d:%02d\n", v[idx].entryHour, v[idx].entryMin);

    long mins = toMinutes(exitD, exitMo, exitY, exitH, exitM)
              - toMinutes(v[idx].entryDay, v[idx].entryMonth, v[idx].entryYear,
                          v[idx].entryHour, v[idx].entryMin);
    if (mins < 0) {
        printf(RED "Exit time is before the entry time. Check-out cancelled.\n" RESET);
        return;
    }

    int hours = (int)((mins + 59) / 60);     /* round up to full hour */
    if (hours < 1) hours = 1;                /* minimum 1 hour charge */
    int fee = hours * RATES[v[idx].type];
    int original = fee;
    int discount = 0;

    long totalDays = mins / 1440;
    printf("\nVehicle %s parked for %ld week(s) %ld day(s) %ld hour(s) %ld min.\n",
           v[idx].plate, totalDays / 7, totalDays % 7, (mins % 1440) / 60, mins % 60);
    printf("Billed hours: %d  x  Rs. %d\n", hours, RATES[v[idx].type]);

    if (v[idx].isMember) {
        discount = fee * MEMBER_DISCOUNT / 100;
        printf("Original fee    : Rs. %d\n", fee);
        printf(GREEN "Member discount (%d%%): - Rs. %d\n" RESET, MEMBER_DISCOUNT, discount);
        fee -= discount;
    }
    printf(RED "Total fee: Rs. %d\n" RESET, fee);

    /* record this payment */
    Payment p;
    p.id = nextPaymentId();
    p.slotNo = v[idx].slotNo;
    strcpy(p.plate, v[idx].plate);
    strcpy(p.owner, v[idx].owner);
    p.type = v[idx].type;
    p.isMember = v[idx].isMember;
    p.inDay = v[idx].entryDay;   p.inMonth = v[idx].entryMonth;
    p.inYear = v[idx].entryYear; p.inHour = v[idx].entryHour;
    p.inMin = v[idx].entryMin;
    p.outDay = exitD;  p.outMonth = exitMo; p.outYear = exitY;
    p.outHour = exitH; p.outMin = exitM;
    p.mins = mins;
    p.hours = hours;
    p.originalFee = original;
    p.discount = discount;
    p.fee = fee;
    savePayment(&p);

    /* just mark the slot empty - other vehicles do not move */
    v[idx].slotNo = 0;
    (*count)--;
    printf(RED "Slot freed.\n" RESET);
}

/* ---------- File handling ---------- */

void saveToFile(Vehicle *v, int count) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf(RED "Error: could not save data.\n" RESET);
        return;
    }
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(v, sizeof(Vehicle), MAX_SLOTS, fp);
    fclose(fp);
    printf(MAGENTA "Data saved (%d records).\n" RESET, count);
}

void loadFromFile(Vehicle *v, int *count) {
    memset(v, 0, sizeof(Vehicle) * MAX_SLOTS);   /* start with all slots empty */
    *count = 0;

    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {                        /* first run: no file yet */
        return;
    }
    if (fread(count, sizeof(int), 1, fp) != 1 || *count < 0 || *count > MAX_SLOTS) {
        *count = 0;
    } else if (fread(v, sizeof(Vehicle), MAX_SLOTS, fp) != MAX_SLOTS) {
        memset(v, 0, sizeof(Vehicle) * MAX_SLOTS);
        *count = 0;
    }
    fclose(fp);
    printf(MAGENTA "Loaded %d record(s) from file.\n" RESET, *count);
}

/* ---------- Extra features ---------- */

/* SORT: bubble sort by slot number (ascending).
   Sorts a copy only, so the real vehicles stay in their slots. */
void sortBySlot(Vehicle *v, int count) {
    Vehicle temp[MAX_SLOTS];
    int n = 0;

    for (int i = 0; i < MAX_SLOTS; i++)
        if (v[i].slotNo != 0) temp[n++] = v[i];

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (temp[j].slotNo > temp[j + 1].slotNo) {
                Vehicle tmp = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = tmp;
            }

    if (n == 0) {
        printf(YELLOW "No vehicles parked.\n" RESET);
        return;
    }
    printf(YELLOW "Vehicles sorted by slot number:\n" RESET);
    printHeader();
    for (int i = 0; i < n; i++) printVehicle(&temp[i]);
}

/* SUMMARY REPORT: counts per type, slot availability and money collected. */
void summaryReport(Vehicle *v, int count) {
    int types[4] = {0};
    int members = 0;
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (v[i].slotNo != 0) {
            types[v[i].type]++;
            if (v[i].isMember) members++;
        }
    }
    int payCount;
    long payTotal;
    getPaymentTotals(&payCount, &payTotal);

    printf(MAGENTA "\n===== PARKING SUMMARY =====\n" RESET);
    printf("Total slots     : %d\n", MAX_SLOTS);
    printf("Occupied        : %d\n", count);
    printf("Available       : %d\n", MAX_SLOTS - count);
    printf("Cars %d  Bikes %d  Trucks %d \n", types[1], types[2], types[3]);
    printf(GREEN "Member vehicles : %d\n" RESET, members);
    printf("Payments made   : %d\n", payCount);
    printf("Total collected : Rs. %ld\n", payTotal);
}

/* LOGIN: after 3 wrong attempts wait 30 seconds, after 5 exit.
   Returns 1 on success. */
int login(void) {
    char pass[50];
    for (int tries = 1; tries <= 5; tries++) {
        readString("Enter password: ", pass, sizeof(pass));
        if (strcmp(pass, PASSWORD) == 0) return 1;
        printf(RED "Wrong password.\n" RESET);

        if (tries == 3) {
            printf(YELLOW "3 wrong attempts. Please wait 30 seconds...\n" RESET);
            time_t start = time(NULL);
            while (time(NULL) - start < 30);     /* wait 30 seconds */
            printf(CYAN "You can try again now.\n" RESET);
        }
    }
    return 0;
}

/* ---------- Main menu ---------- */

int main(void) {
    Vehicle lot[MAX_SLOTS] = {0};
    int count = 0;
    int choice;

    system("");     /* makes colours work in Windows terminal */

    printf(CYAN "=== PARKING LOT MANAGEMENT SYSTEM ===\n" RESET);
    if (login()==0) {
        printf(RED "Too many failed attempts. Exiting.\n" RESET);
        return 1;
    }
    printf(GREEN "Login successful.\n" RESET);
    loadFromFile(lot, &count);

    do {
        printf(CYAN "\n------ MENU ------\n" RESET);
        printf("1. Park vehicle (Add)\n");
        printf("2. Display all\n");
        printf("3. Search\n");
        printf("4. Update record\n");
        printf("5. Check out (Delete)\n");
        printf("6. Sort by slot\n");
        printf("7. Summary report\n");
        printf("8. Save\n");
        printf("9. Load from file\n");
        printf("10. Payment history\n");
        printf("0. Save & Exit\n");
        choice = readInt("Choice: ", 0, 10);

        switch (choice) {
            case 1: addVehicle(lot, &count);    break;
            case 2: displayAll(lot, count);     break;
            case 3: searchVehicle(lot, count);  break;
            case 4: updateVehicle(lot, count);  break;
            case 5: deleteVehicle(lot, &count); break;
            case 6: sortBySlot(lot, count);     break;
            case 7: summaryReport(lot, count);  break;
            case 8: saveToFile(lot, count);     break;
            case 9: loadFromFile(lot, &count);  break;
            case 10: showPayments();            break;
            case 0: saveToFile(lot, count);
                    printf(RED "Goodbye!\n" RESET); break;
        }
    } while (choice != 0);

    return 0;
}