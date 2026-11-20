
#include <stdio.h>
#include "payment_utils.h"
#include "lookup_utils.h"    /* TYPE_NAMES */
#include "input_utils.h"      /* PAYMENT_FILE and the colour macros */

/* Returns the id for the next payment (records already in file + 1). */
int nextPaymentId(void) {
    FILE *fp = fopen(PAYMENT_FILE, "rb");
    if (fp == NULL) return 1;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (int)(size / sizeof(Payment)) + 1;
}

/* Adds one payment to the end of the payment file. */
void savePayment(const Payment *p) {
    FILE *fp = fopen(PAYMENT_FILE, "ab");
    if (fp == NULL) {
        printf(RED "Error: could not record the payment.\n" RESET);
        return;
    }
    fwrite(p, sizeof(Payment), 1, fp);
    fclose(fp);
    printf(MAGENTA "Payment #%d recorded.\n" RESET, p->id);
}

/* Counts all payments and adds up the money collected. */
void getPaymentTotals(int *n, long *total) {
    Payment p;
    *n = 0;
    *total = 0;
    FILE *fp = fopen(PAYMENT_FILE, "rb");
    if (fp == NULL) return;
    while (fread(&p, sizeof(Payment), 1, fp) == 1) {
        (*n)++;
        *total += p.fee;
    }
    fclose(fp);
}

/* PAYMENT HISTORY: shows every payment ever recorded + total collected. */
void showPayments(void) {
    Payment p;
    int n = 0;
    long total = 0;
    FILE *fp = fopen(PAYMENT_FILE, "rb");
    if (fp == NULL) {
        printf(YELLOW "No payments recorded yet.\n" RESET);
        return;
    }
    printf(CYAN "\n%-10s %-10s %-5s %-12s %-22s %-23s %-18s %-13s\n",
           "ID", "Slot", "Plate", "Type", "Entry", "Exit", "Hours", "Paid");
    printf("-----------------------------------------------------------------------------------\n" RESET);
    while (fread(&p, sizeof(Payment), 1, fp) == 1) {
        printf("%-5d %-5d %-10s %-7s %02d/%02d/%04d %02d:%02d %02d/%02d/%04d %02d:%02d %-8d Rs.%d%s\n",
               p.id, p.slotNo, p.plate, TYPE_NAMES[p.type],
               p.inDay, p.inMonth, p.inYear, p.inHour, p.inMin,
               p.outDay, p.outMonth, p.outYear, p.outHour, p.outMin,
               p.hours, p.fee, p.isMember ? " (M)" : "");
        n++;
        total += p.fee;
    }
    fclose(fp);
    if (n == 0) {
        printf(YELLOW "No payments recorded yet.\n" RESET);
        return;
    }
    printf(MAGENTA "\nTotal payments: %d    Total collected: Rs. %ld\n" RESET, n, total);
    printf("(M) = member discount applied\n");
}
