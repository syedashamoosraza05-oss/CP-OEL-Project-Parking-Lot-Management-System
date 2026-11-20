#ifndef LOOKUP_UTILS_H
#define LOOKUP_UTILS_H

/* One parking slot. Vehicle for slot N is stored at v[N-1].
   slotNo == 0 means the slot is empty. */
typedef struct {
    int  slotNo;              /* 1..MAX_SLOTS, 0 = empty slot     */
    char plate[20];           /* number plate, unique             */
    char owner[40];           /* owner name                       */
    int  type;                /* 1 = Car, 2 = Bike, 3 = Truck     */
    int  isMember;            /* 1 = Member, 0 = regular          */
    int  entryDay;            /* 1-31                             */
    int  entryMonth;          /* 1-12                             */
    int  entryYear;           /* e.g. 2026                        */
    int  entryHour;           /* 0-23                             */
    int  entryMin;            /* 0-59                             */
} Vehicle;

/* Hourly rates and names for each vehicle type (index 1..3) */
extern const int RATES[4];
extern const char *TYPE_NAMES[4];

int isMemberSlot(int slot);
int findBySlot(Vehicle *v, int slot);
int findByPlate(Vehicle *v, const char *plate);
void printHeader(void);
void printVehicle(const Vehicle *v);
void showEmptySlots(Vehicle *v);
int verifyMembership(void);

#endif
