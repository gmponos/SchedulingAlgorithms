#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "routing.h"

pthread_t random_t, child_t;
pthread_mutex_t crit_m = PTHREAD_MUTEX_INITIALIZER;

int read_random();
int initialize();

int start_alarm();
int finish_alarm();

/* Μεταβλητές που αφορούν τα παιδιά.
 * stat_c   - Επιστρεφόμενη τιμή απο τα παιδιά
 * pid_c[x] - Χώρος αποθήκευσης του pid των παιδιών
 * flag_c   - flag που δηλώνει αν πρόκειται για παιδί */
int stat_c, pid_c[10], flag_c;

int EndTime;

/* Το power είναι μία σημαντική μεταβλητή στο πρόγραμμα μας.
 * Συμβολίζει το ότι το λειτουργικό είναι ανοιχτό και στέλνει
 * αιτήσεις. Έτσι ο αλγόριθμος καταλαβαίνει ότι όσο είναι
 * σε κατάσταση ON θα εξυπηρετεί αιτήσεις
 * if (Power) { sleep(1); } */
int Power;

int main(void) {

    short int i;

    srand(time(NULL));

    initialize();

    puts("To πρόγραμμα εξυπηρετεί Χ αιτήσεις όπου Χ ένας τυχαίος αριθμός");
    puts("αιτήσεων ο οποίος δημιουργήθηκε σε n δευτερόλεπτα.");
    puts("\nΣημείωση:");
    puts("Ο χρόνος που θα δημιουργεί αιτήσεις δεν εχει σχέση με τον χρόνο");
    puts("που θα εκτελείται το πρόγραμμα");
    puts("\nΕισήγαγε τα n δευτερόλεπτα που το πρόγραμμα θα δημιουργεί");
    puts("αιτήσεις:");

    /* Εισήγαγε μη μηδενική τιμή */
    do {
	scanf("%d", &EndTime);
    } while (EndTime < 0);

    for (i = 0; i < 10; i++) {
	pid_c[i] = fork();
	switch (pid_c[i]) {
	    case 0:
		childs(i);
		flag_c = TRUE;
		break; //case break
	    case -1:
		perror("Fork SSTF");
		return Error;
	}
	//Αν είσαι παιδί κάνε break απ'το for
	if (flag_c == TRUE) {
	    break;
	}
    }

    if (flag_c == FALSE) {
	father();
    }

    return (EXIT_SUCCESS);
}

/*********************************************************************************
/*******************  Δημιουργία των τυχαίων αριθμών  ****************************
/*********************************************************************************/

int father() {
    int x, i, tmp;
    char FileName[40] = "random.txt";
    FILE *fp = fopen(FileName, "w");

    for (i = 0; i < 10000; i++) {
	x = rand() % MaxBlockRange;
	fprintf(fp, "%d ", x);
    }
    fclose(fp);

    //Στείλε σήμα στα παιδιά ότι δημιούργησα τυχαίους αριθμούς
    for (i = 0; i < 10; i++) {
	kill(pid_c[i], SIGALRM);
    }

    for (i = 0; i < 10; i++) {

	waitpid(pid_c[i], &stat_c, 0);
	tmp = stat_c >> 8;

	if (tmp == TRUE) {
	    printf("Success");
	} else {
	    printf("Error");
	}

	switch (i) {
	    case 0:
		printf(" at SSTF \n");
		break;
	    case 1:
		printf(" at FCFS \n");
		break;
	    case 2:
		printf(" at SCAN down \n");
		break;
	    case 3:
		printf(" at SCAN up \n");
		break;
	    case 4:
		printf(" at C-SCAN down \n");
		break;
	    case 5:
		printf(" at C-SCAN up \n");
		break;
	    case 6:
		printf(" at LOOK down \n");
		break;
	    case 7:
		printf(" at LOOK up \n");
		break;
	    case 8:
		printf(" at C-LOOK down \n");
		break;
	    case 9:
		printf(" at C-LOOK up \n");
		break;
	}

    }
    return;
}

/*********************************************************************************
/*********************  Δημιουργία των αλγόριθμων  *******************************
/*********************************************************************************/

int childs(int i) {

    /* Αυτή τη περιοχή θα την διαχειριστούμε ως μία δεύτερη submain για τα παιδιά.
     * ότι θέλουμε να ισχύει για ολα τα παιδιά θα το δηλώνουμε πριν από τα νήματα.
     * Με το i θα ξεχωρίζουμε για ποιο παιδί μιλάμε. */

    //Περίμενε τον πατέρα να στείλει σήμα ότι δημιούργησε τους τυχαίους αριθμούς.
    signal(SIGALRM, start_alarm);
    pause();

    //Το πρόγραμμα μας θα εκτελείται όσο χρόνο εμείς του εισάγουμε.
    signal(SIGALRM, finish_alarm);
    alarm(EndTime);

    //Συγκεκριμένος κώδικας για κάθε παιδί ξεχωριστά.
    switch (i) {
	case 0:
	    printf("SSTF \n");
	    if (pthread_create(&child_t, NULL, (void *) &SSTF, NULL) != 0) {
		puts("Error creating SSTF thread");
		exit(Error);
	    }
	    break;
	case 1:
	    printf("FCFS \n");
	    if (pthread_create(&child_t, NULL, (void *) &FCFS, NULL) != 0) {
		puts("Error creating FCFS thread");
		exit(Error);
	    }
	    break;
	case 2:
	    printf("SCAN down \n");
	    if (pthread_create(&child_t, NULL, (void *) &SCAN, NULL) != 0) {
		puts("Error creating SCAN thread");
		exit(Error);
	    }
	    break;
	case 3:
	    printf("SCAN up \n");
	    App.direction = UP;

	    if (pthread_create(&child_t, NULL, (void *) &SCAN, NULL) != 0) {
		puts("Error creating SCAN thread");
		exit(Error);
	    }
	    break;
	case 4:
	    printf("C-SCAN down \n");
	    if (pthread_create(&child_t, NULL, (void *) &C_SCAN, NULL) != 0) {
		puts("Error creating C-SCAN thread");
		exit(Error);
	    }
	    break;
	case 5:
	    printf("C-SCAN up \n");
	    App.direction = UP;
	    if (pthread_create(&child_t, NULL, (void *) &C_SCAN, NULL) != 0) {
		puts("Error creating C-SCAN thread");
		exit(Error);
	    }
	    break;

	case 6:
	    printf("LOOK down \n");
	    if (pthread_create(&child_t, NULL, (void *) &LOOK, NULL) != 0) {
		puts("Error creating LOOK thread");
		exit(Error);
	    }
	    break;

	case 7:
	    printf("LOOK up \n");
	    App.direction = UP;
	    if (pthread_create(&child_t, NULL, (void *) &LOOK, NULL) != 0) {
		puts("Error creating LOOK thread");
		exit(Error);
	    }
	    break;
	case 8:
	    printf("C-LOOK down \n");
	    if (pthread_create(&child_t, NULL, (void *) &C_LOOK, NULL) != 0) {
		puts("Error creating LOOK thread");
		exit(Error);
	    }
	    break;
	case 9:
	    printf("C-LOOK up \n");
	    App.direction = UP;
	    if (pthread_create(&child_t, NULL, (void *) &C_LOOK, NULL) != 0) {
		puts("Error creating LOOK thread");
		exit(Error);
	    }
	    break;
    }

    if (pthread_create(&random_t, NULL, (void *) &read_random, NULL) != 0) {
	puts("Error creating Random thread");
	exit(Error);
    }

    pthread_join(random_t, NULL);
    pthread_join(child_t, NULL);

    exit(TRUE);
}

/* Λίγα λόγια για τους αλγόριθμους.
 * 1. Όλοι δημιουργούν ένα αρχείο με το όνομα τους. Τα αρχείο που το όνομα τους
 * ξεκινάει από 0 σημαίνει ότι ο αλγόριθμος λειτουργεί με κατεύθυνση UP ενώ τα 
 * αρχεία που το όνομα τους ξεκινάει από 1 σημαίνει ότι ο αλγόριθμος λειτουργεί με
 * κατεύθυνση down.
 * 
 * 2. if(Power) { sleep(1); }
 * Η συγκεκριμένη εντολή είναι κοινή σε όλους τους αλγόριθμους.
 * Όσο το νήμα που συμβολίζει το Λ.Σ. είναι ανοιχτό και στέλνει αιτήσεις 
 * στο σκληρό τότε κοιμήσου για 1sec αλλιώς εξυπηρέτησε όσες αιτήσεις σου έμειναν.
 * Την sleep την εισαγάγαμε προκειμένου να έχουμε πιο εμφανή αποτελέσματα. */

/*********************************************************************************
/********************************  FCFS  *****************************************
/*********************************************************************************/

int FCFS() {
    short int i;

    char FileName[40] = "FCFS.txt";
    FILE *fp = fopen(FileName, "w");

    while (Power) {
	while (App.QueueSize) {
	    pthread_mutex_lock(&crit_m);

	    // Εξυπηρέτησε σειριακά όσες αιτήσεις δημιουργήθηκαν έως τώρα.
	    for (i = 0; i < App.QueueSize; i++) {
		App.sum += abs(App.CurrentApp - App.Queue[i]);
		fprintf(fp, "%d    %d     %d     %d\n", App.CurrentApp, App.Queue[i], App.rndLoop[i], App.sum);
		App.CurrentApp = App.Queue[i];
	    }

	    App.QueueSize = 0;
	    pthread_mutex_unlock(&crit_m);
	    if (Power) {
		sleep(1);
	    }
	}
    }
    return TRUE;
}

/**
 * SSTF algorithm
 * 
 * @return 
 */
int SSTF() {
    unsigned int x;
    int POS, MIN, tmp;

    char FileName[40] = "SSTF.txt";
    FILE *fp = fopen(FileName, "w");

    fprintf(fp, "Start POS: %d\n", App.CurrentApp);
    fprintf(fp, "\nCurrent rndLoop  sum\n\n");

    while (Power) {
	while (App.QueueSize) {

	    MIN = MaxBlockRange;
	    pthread_mutex_lock(&crit_m);

	    for (x = 0; x < App.QueueSize; x++) {
		tmp = abs(App.Queue[x] - App.CurrentApp);

		if (tmp < MIN) {
		    MIN = tmp;
		    POS = x;
		}
	    }

	    App.sum += MIN;
	    App.CurrentApp = App.Queue[POS];
	    App.CurrentRND = App.rndLoop[POS];
	    remove_app(POS);

	    fprintf(fp, "%d    %d     %d \n", App.CurrentApp, App.CurrentRND, App.sum);

	    pthread_mutex_unlock(&crit_m);
	    if (Power) {
		sleep(1);
	    }
	}
    }

    fclose(fp);
    return TRUE;
}

/**
 * SCAN algorithm
 * @return 
 */
int SCAN() {

    char FileName[40];
    sprintf(FileName, "%d SCAN.txt", App.direction);
    FILE *fp = fopen(FileName, "w");

    fprintf(fp, "Start POS: %d\n", App.CurrentApp);
    fprintf(fp, "\nCurrent rndLoop  sum\n\n");

    while (Power) {
	while (App.QueueSize) {
	    switch (App.direction) {
		case UP:
		    while (App.QueueSize) {
			pthread_mutex_lock(&crit_m);

			if (move_up() == FALSE) {
			    App.sum += abs(App.CurrentApp - MaxBlockRange);
			    App.CurrentApp = MaxBlockRange - 1;
			    App.direction = DOWN;

			    fprintf(fp, "direction changed \n%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
			    // Κάνουμε Break από το while γιατί θέλουμε να αλλάξουμε κατεύθυνση.
			    break;
			}
			fprintf(fp, "%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
			pthread_mutex_unlock(&crit_m);
			if (Power) {
			    sleep(1);
			}
		    }
		    pthread_mutex_unlock(&crit_m);
		    break; //case break

		default:
		    while (App.QueueSize) {
			pthread_mutex_lock(&crit_m);

			if (move_down() == FALSE) {
			    App.sum += abs(App.CurrentApp);
			    App.CurrentApp = 0;
			    App.direction = UP;

			    fprintf(fp, "direction changed \n%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);

			    // Κάνουμε Break από το for γιατί θέλουμε να αλλάξουμε κατεύθυνση.
			    break;
			}
			fprintf(fp, "%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
			pthread_mutex_unlock(&crit_m);
			if (Power) {
			    sleep(1);
			}
		    }
		    pthread_mutex_unlock(&crit_m);
		    break; //case break
	    }
	}
    }

    fclose(fp);
    return TRUE;
}

/*********************************************************************************
/******************************  C-SCAN  *****************************************
/*********************************************************************************/

int C_SCAN() {

    char FileName[40];
    sprintf(FileName, "%d C-SCAN.txt", App.direction);
    FILE *fp = fopen(FileName, "w");

    fprintf(fp, "Start POS: %d\n", App.CurrentApp);
    fprintf(fp, "\nCurrent rndLoop  sum\n\n");

    switch (App.direction) {
	case UP:
	    while (Power) {
		while (App.QueueSize) {

		    pthread_mutex_lock(&crit_m);

		    if (move_up() == FALSE) {
			App.sum += abs(App.CurrentApp - MaxBlockRange);
			App.CurrentApp = 0;
			fprintf(fp, "Cycle\n");
		    }
		    fprintf(fp, "%d     %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
		    pthread_mutex_unlock(&crit_m);
		    if (Power) {
			sleep(1);
		    }
		}
	    }
	    break; //case break

	default:
	    while (Power) {
		while (App.QueueSize) {

		    pthread_mutex_lock(&crit_m);

		    if (move_down() == FALSE) {
			App.sum += abs(App.CurrentApp + 1);
			App.CurrentApp = MaxBlockRange - 1;
			fprintf(fp, "Cycle\n");
		    }
		    fprintf(fp, "%d     %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
		    pthread_mutex_unlock(&crit_m);
		    if (Power) {
			sleep(1);
		    }
		}
	    }
	    break; //case break
    }
    fclose(fp);
    return TRUE;
}

/*********************************************************************************
/********************************  LOOK  *****************************************
/*********************************************************************************/

int LOOK() {

    char FileName[40];
    sprintf(FileName, "%d LOOK.txt", App.direction);
    FILE *fp = fopen(FileName, "w");

    fprintf(fp, "Start POS: %d\n", App.CurrentApp);
    fprintf(fp, "\nCurrent rndLoop  sum\n\n");

    while (Power) {
	while (App.QueueSize) {

	    switch (App.direction) {
		case UP:
		    while (App.QueueSize) {

			pthread_mutex_lock(&crit_m);

			if (move_up() == FALSE) {
			    App.direction = DOWN;
			    fprintf(fp, "direction changed \n");
			    break;
			    // Κάνουμε Break από το for γιατί θέλουμε να αλλάξουμε κατεύθυνση.
			}
			fprintf(fp, "%d     %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
			pthread_mutex_unlock(&crit_m);
			if (Power) {
			    sleep(1);
			}
		    }
		    pthread_mutex_unlock(&crit_m);
		    break; //case break

		default:
		    while (App.QueueSize) {

			pthread_mutex_lock(&crit_m);
			if (move_down() == FALSE) {
			    App.direction = UP;
			    fprintf(fp, "direction changed \n");
			    break;
			    // Κάνουμε Break από το for γιατί θέλουμε να αλλάξουμε κατεύθυνση.
			}
			fprintf(fp, "%d     %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
			pthread_mutex_unlock(&crit_m);
			if (Power) {
			    sleep(1);
			}
		    }
		    pthread_mutex_unlock(&crit_m);
		    break; //case break
	    }
	}
    }

    fclose(fp);
    return TRUE;
}

/*********************************************************************************
/********************************  C-LOOK  ***************************************
/*********************************************************************************/
int C_LOOK() {

    char FileName[40];
    sprintf(FileName, "%d C-LOOK.txt", App.direction);
    FILE *fp = fopen(FileName, "w");

    fprintf(fp, "Start POS: %d\n", App.CurrentApp);
    fprintf(fp, "\nCurrent rndLoop  sum\n\n");

    switch (App.direction) {
	case UP:
	    while (Power) {
		while (App.QueueSize) {
		    pthread_mutex_lock(&crit_m);
		    if (move_up() == FALSE) {
			/* Μετακινήσου στην μικρότερη */
			min_app();
			fprintf(fp, "cycle\n");
		    }
		    fprintf(fp, "%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
		    pthread_mutex_unlock(&crit_m);
		    if (Power) {
			sleep(1);
		    }
		}
	    }
	    break; //case break

	default:
	    while (Power) {
		while (App.QueueSize) {

		    pthread_mutex_lock(&crit_m);
		    if (move_down() == FALSE) {
			/* Μετακινήσου στην μεγαλύτερη */
			max_app();
			fprintf(fp, "cycle\n");
		    }
		    fprintf(fp, "%d    %d     %d\n", App.CurrentApp, App.CurrentRND, App.sum);
		    pthread_mutex_unlock(&crit_m);
		    if (Power) {
			sleep(1);
		    }
		}
	    }
	    break; //case break
    }
    return TRUE;
}

int read_random() {
    int count, tmp, loop = 0;
    FILE *fp;
    char read[40] = "random.txt";

    if ((fp = fopen(read, "r")) == NULL) {
	printf("Error opening file at pid %d \n", getpid());
	return Error;
    }

    while (Power) {

	count = defineCount();

	pthread_mutex_lock(&crit_m); //Κρίσιμη περιοχή.
	loop++;
	while (count--) {
	    fscanf(fp, "%d", &tmp);
	    add_app(tmp, loop);
	}
	pthread_mutex_unlock(&crit_m); //Τέλος Κρίσιμης περιοχή.

	//Αφού διάβασες μερικούς αριθμούς πέσε για ύπνο.
	sleep(1);
    }

    //Κλείνουμε τα αρχεία.
    fclose(fp);
    return TRUE;
}

int initialize() {
    //Αρχικοποιούμε το πρόγραμμα μας.
    flag_c = FALSE;
    Power = ON;

    App.QueueSize = 0;
    App.sum = 0;
    App.QueueCapacity = new_capacity();
    App.direction = DOWN;
    App.CurrentApp = rand() % MaxBlockRange;
    return TRUE;
}

int start_alarm(int sig) {
    printf("Είμαι το παιδί με pid %d και ξεκίνησα ", getpid());
    return TRUE;
}

int finish_alarm(int sig) {
    Power = OFF;

    //Σταμάτα το νήμα που διαβάζει αριθμούς.
    pthread_cancel(random_t);

    //Περίμενε να εξυπηρετήσει και τις υπόλοιπες αιτήσεις.
    pthread_join(child_t, NULL);
    return TRUE;
}