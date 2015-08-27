#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "routing.h"

int new_capacity(){
    int *tmp;
    
	/* Εάν το διάνυσμα δεν υπάρχει δέσμευσε έναν νέο χώρο στη μνήμη
	 * και επέστρεψε τιμή ίση με το μέγεθος του διανύσματος. */
    if (App.Queue==NULL){

        tmp = (int *)malloc(200 * sizeof(int));
        if (tmp==NULL){
            puts("Malloc Error");
            exit (Error);
        }
        App.Queue = tmp;

        tmp = (int *)malloc(200 * sizeof(int));
        if (tmp==NULL){
            puts("Malloc Error");
            exit (Error);
        }
        App.rndLoop = tmp;

        return 200;
    }

  /* Αφού το διάνυσμα υπάρχει δέσμευσε έναν νέο επιπλέον χώρο στη
   * μνήμη και επέστρεψε τιμή ίση με το μέγεθος του νέου διανύσματος.*/
    tmp = (int *)realloc(App.Queue,(App.QueueCapacity + 100) * sizeof(int));
    if (tmp == NULL){
        puts("Realloc Error");
        exit (Error);
    }    
    App.Queue = tmp;

    tmp = (int *)realloc(App.rndLoop,(App.QueueCapacity + 100) * sizeof(int));
    if (tmp == NULL){
        puts("Realloc Error");
        exit (Error);
    }
    App.rndLoop = tmp;

    return 100;
}

int defineCount(){
    short int tmp;

    tmp = rand() % ReadRandomRate;
    return tmp;
    
	//Σε περίπτωση που θέλουμε όσο αυξάνει ο σωρός απο αιτήσεις
	//να μειώνονται οι αιτήσεις που στέλνει το Λ.Σ.
	//Θα βγάλουμε από τα σχόλια τον παρακάτω κώδικα.
/*
    if (App.QueueSize<StackSize) {
        tmp = rand() % ReadRandomRate;
        return tmp;
    } else if(App.QueueSize<StackSize*2){
        tmp = rand() % (int)(ReadRandomRate/2);
        return tmp;
    } else if(App.QueueSize<StackSize*3){
        tmp = rand() % (int)(ReadRandomRate/3);
        return tmp;
    }    
*/
}

int add_app(int tmp,int loop){

    App.Queue[App.QueueSize] = tmp;
    App.rndLoop[App.QueueSize] = loop;
    App.QueueSize++;

	/* Εάν η χωριτικότητα του διανύσματος με τον αριθμό των αιτήσεων 
     είναι ίσα αύξησε την χωρητικότητα του διανύσματος. */
    if (App.QueueCapacity == App.QueueSize){
        App.QueueCapacity += new_capacity();
    }

    return;
}

int remove_app(int POS){
    int *tmp_1,*tmp_2;

    App.Queue[POS] = App.Queue[App.QueueSize-1];
    App.rndLoop[POS] = App.rndLoop[App.QueueSize-1];
    App.QueueSize--;

    /* Δεν υπάρχει λόγος να έχουμε λιγότερο απο 50 στοιχεία και
     * και να έχουμε δεσμέυσει μνήμη περισσότερο από 200 */

    if ((App.QueueSize<50) && (App.QueueCapacity>200)){

        tmp_1 = (int *)malloc(200 * sizeof(int));
        if (tmp_1==NULL){
            puts("Error deleting app");
            exit (Error);
        }
        memmove(tmp_1,App.Queue,App.QueueSize * sizeof(int));

        free(App.Queue);        
        App.Queue = tmp_1;
        
        tmp_2 = (int *)malloc(200 * sizeof(int));
        if (tmp_2==NULL){
            puts("Error deleting app");
            exit (Error);
        }
        memmove(tmp_2,App.rndLoop,App.QueueSize * sizeof(int));

        free(App.rndLoop);
        App.rndLoop = tmp_2;
    }

    return 0;
}

int move_up(){
    int tmp,POS,MIN;
    int x;

    MIN = MaxBlockRange;

    for (x=0; x<App.QueueSize; x++){

        tmp = App.Queue[x]-App.CurrentApp;

        if ((tmp>=0) && (abs(tmp)<abs(MIN))){
            MIN = tmp;
            POS = x;
        }
        if (tmp==0){ break; }
    }

    /* Εάν δεν καταφέρεις να κινηθείς προς τα πάνω επέστρεψε τιμή FALSE */
    if (MIN==MaxBlockRange) { return FALSE; }

    /* Eφόσον έχεις βρει ποια αίτηση είναι η επόμενη
     * διάβασε την και αποθήκευσε και την κίνηση που
     * έκανε η κεφαλή για να την διαβάσει */
    App.sum       += abs(MIN);
    App.CurrentApp = App.Queue[POS];
    App.CurrentRND = App.rndLoop[POS];
    remove_app(POS);

    return TRUE;
}

int move_down(){
    int x;
    int POS,MIN,tmp;

    MIN = MaxBlockRange;

    for (x=0; x<App.QueueSize; x++){
        tmp = App.Queue[x]-App.CurrentApp;

        if ((tmp<=0) && (abs(tmp)<abs(MIN))){
            MIN = tmp;
            POS = x;
        }
        if (tmp==0){ break; }
    }

    /* Εάν δεν καταφέρεις να κινηθείς προς τα πάνω επέστρεψε τιμή FALSE */
    if (MIN==MaxBlockRange){ return FALSE; }

    /* Eφόσον έχεις βρει ποια αίτηση είναι η επόμενη
     * διάβασε την και αποθήκευσε και την κίνηση που
     * έκανε η κεφαλή για να την διαβάσει */
    App.sum       += abs(MIN);
    App.CurrentApp = App.Queue[POS];
    App.CurrentRND = App.rndLoop[POS];
    remove_app(POS);

    return TRUE;
}

int min_app(){
    int x;
    int tmp,POS,MIN;

    //Βρές τη μικρότερή αίτηση και μετακινήσου εκεί.
    MIN = MaxBlockRange;

    for(x=0; x<App.QueueSize; x++){

        tmp = App.Queue[x] - App.CurrentApp;

        if (tmp<MIN){
            MIN = tmp;
            POS = x;
        }
    }

    /* Εάν δεν καταφέρεις να βρεις το μικρότερο επέστρεψε τιμή FALSE */
    if (MIN==MaxBlockRange){ return FALSE; }

    App.sum += abs(MIN);
    App.CurrentApp = App.Queue[POS];
    App.CurrentRND = App.rndLoop[POS];
    remove_app(POS);

    return TRUE;
}

int max_app(){
    int x;
    int tmp,POS,MAX;
    
    //Βρές τη μεγαλύτερη αίτηση και μετακινήσου εκεί.
    MAX = MaxBlockRange;

    for(x=0; x<App.QueueSize; x++){
        tmp =App.CurrentApp - App.Queue[x];
        if (tmp<MAX){
            MAX = tmp;
            POS = x;
        }
    }

    /* Εάν δεν καταφέρεις να βρεις το μεγαλύτερο επέστρεψε τιμή FALSE */
    if (MAX==MaxBlockRange){ return FALSE; }

    App.sum += abs(MAX);
    App.CurrentApp = App.Queue[POS];
    App.CurrentRND = App.rndLoop[POS];
    remove_app(POS);

    return TRUE;
}