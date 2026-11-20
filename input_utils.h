#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#define MAX_SLOTS 50          /* total parking slots in the lot   */
#define DATA_FILE "parking.dat"
#define PAYMENT_FILE "payments.dat"   /* every payment is appended here  */
#define PASSWORD  "admin123"          /* admin login password            */
#define MEMBER_PASSWORD "member123"   /* password to verify membership   */
#define MEMBER_DISCOUNT 20            /* members get 20% off the fee     */

/* Colours: red = errors/delete/exit, green = adding/member/login ok,
            cyan, magenta, yellow = everything else */
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define MAGENTA "\033[1;35m"
#define RESET   "\033[0m"

int readInt(const char prompt[], int min, int max);

void readString(const char prompt[], char buf[], int size);

void toUpperStr(char *s);

#endif
