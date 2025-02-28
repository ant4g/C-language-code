#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int balance;
    int num_ac;
    pthread_mutex_t lock;
} BankAccount;

void wpłata(BankAccount* account, int amount) {
    pthread_mutex_lock(&account->lock);
    account->balance += amount;
    printf("Wpłata: %d, Stan konta: %d, po wpłacie: %d\n", amount,account->num_ac, account->balance);
    pthread_mutex_unlock(&account->lock);
}

void wypłata(BankAccount* account, int amount) {
    pthread_mutex_lock(&account->lock);
    if (account->balance >= amount) {
        account->balance -= amount;
        printf("Wypłata: %d, Stan konta: %d, po wypłacie: %d\n", amount, account->num_ac,  account->balance);
    } else {
        printf("Wypłata niemożliwa za mało środków na koncie\n");
    }
    pthread_mutex_unlock(&account->lock);
}

void przelew(BankAccount* from, BankAccount* to, int amount) {
    pthread_mutex_lock(&from->lock);
    pthread_mutex_lock(&to->lock);

    if (from->balance >= amount) {
        from->balance -= amount;
        to->balance += amount;
        printf("Przelew: %d z konta1 do konta2\n", amount);
        printf("Stan konta1 po operacji: %d\n", from->balance);
        printf("Stan konta2 po operacji: %d\n", to->balance);
    } else {
        printf("Przelew niemożliwy: Za mało środków na koncie nr. 1\n");
    }

    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
}

void* wpłata_thread(void* arg) {
    BankAccount* account = (BankAccount*)arg;
    wpłata(account, 50);
    return NULL;
}

void* wypłata_thread(void* arg) {
    BankAccount* account = (BankAccount*)arg;
    wypłata(account, 30);
    return NULL;
}

void* przelew_thread(void* arg) {
    BankAccount** accounts = (BankAccount**)arg;
    przelew(accounts[0], accounts[1], 40);
    return NULL;
}

int main() {
    BankAccount account1 = {100, 1, PTHREAD_MUTEX_INITIALIZER};
    BankAccount account2 = {200, 2, PTHREAD_MUTEX_INITIALIZER};


    printf("Początkowy stan konta 1: %d\n", account1.balance );
    printf("Początkowy stan konta 2: %d\n", account2.balance );

    pthread_t threads[3];

    pthread_create(&threads[0], NULL, wpłata_thread, &account1);
    pthread_create(&threads[1], NULL, wypłata_thread, &account2);

    BankAccount* accounts[] = {&account1, &account2};
    pthread_create(&threads[2], NULL, przelew_thread, accounts);

    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Końcowy stan konta 1: %d\n", account1.balance);
    printf("Końcowy stan konta 2: %d\n", account2.balance);

    pthread_mutex_destroy(&account1.lock);
    pthread_mutex_destroy(&account2.lock);

    return 0;
}
