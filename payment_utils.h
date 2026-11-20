#ifndef PAYMENT_UTILS_H
#define PAYMENT_UTILS_H
typedef struct {
    int  id;                  /* payment number 1, 2, 3...        */
    int  slotNo;
    char plate[20];
    char owner[40];
    int  type;
    int  isMember;
    int  inDay, inMonth, inYear, inHour, inMin;      /* entry date + time */
    int  outDay, outMonth, outYear, outHour, outMin; /* exit date + time  */
    long mins;                /* total minutes parked             */
    int  hours;               /* billed hours (rounded up)        */
    int  originalFee;         /* fee before discount              */
    int  discount;            /* discount amount (0 for regular)  */
    int  fee;                 /* final amount paid                */
} Payment;

int nextPaymentId(void);
void savePayment(const Payment *p);
void getPaymentTotals(int *n, long *total);
void showPayments(void);
#endif
