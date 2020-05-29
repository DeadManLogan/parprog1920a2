// code base for various sorting algorithms
// compile with e.g. gcc -fopenmp -O2 -Wall quicksort.c -o quicksort

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define CUTOFF 10
#define N 1000000
#define CREATE_LIMIT 500000

// mutex protecting common resources
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t msg_in = PTHREAD_COND_INITIALIZER;
pthread_cond_t msg_out = PTHREAD_COND_INITIALIZER;

typedef struct
{
    lista[MESSAGES];
    int lin = -1, lout = 0;
};

struct thread_params{
    double *lista;
	int n;	
};

int send(int msg)
{
    pthread_mutex_lock(&mutex);
    while (global_availmsg>0)
    {     //perimeno na adeiasei to buffer kai an den ginei to epanalamvanei
        pthread_cond_wait(&msg_out,&mutex);   //to mutex mas petaei apo tin oura kai paei to thread gia ipno.ksipnaei mono apo ti msg_out
    }

    global_buffer[qout] = msg;
    lout++;
    pthread_cond_signal(&msg_in);
    pthread_mutex_unlock(&mutex);
}

int receive() {

    // lock mutex
    pthread_mutex_lock(&mutex);
    while (global_availmsg<1)
    {
        pthread_cond_wait(&msg_in,&mutex);
    }

    // receive message
    int terminated = 1;

    pthread_cond_signal(&msg_out);
    pthread_mutex_unlock(&mutex);

    return(terminated);
}

void *thread_func (void *args){

    int n = ((struct thread_params*)args)->n;
    double *a = ((struct thread_params*)args)->*a;
    int i = 0;

    struct thread_params *tparm; 
    // thread input params
    tparm = (struct thread_params *)args;
    int id = tparm->id;

    while(1)
    {
        q = receive();

        if(n > CREATE_LIMIT)
        {
            z = partition(lista, n);

            pthread_create(&threads[i],NULL,thread_func,&tparm[i]);
            pthread_create(&threads[i],NULL,thread_func,&tparm[i]);

            pthread_join(threads[i],NULL);
            pthread_join(threads[i],NULL);
        }
        else
        {
            quicksort(lista,n);
            int terminated = 1;
            send(terminated);
        }
        // exit and let be joined
        pthread_exit(NULL); 
    }


}

void inssort(double *lista,int n) {
    int i,j;
    double t;

    for (i=1;i<n;i++) {
        j = i;
        while ((j>0) && (a[j-1]>a[j])) {
            t = lista[j-1];  lista[j-1] = lista[j];  lista[j] = t;
            j--;
        }
    }

}


int partition(double *lista,int n) {
    int first,last,middle;
    double t,p;
    int i,j;

    // take first, last and middle positions
    first = 0;
    middle = n/2;
    last = n-1;  

    // put median-of-3 in the middle
    if (lista[middle] < lista[first]) { t = lista[middle]; lista[middle] = lista[first]; lista[first] = t; }
    if (lista[last] < lista[middle]) { t = lista[last]; lista[last] = lista[middle]; lista[middle] = t; }
    if (lista[middle] < lista[first]) { t = lista[middle]; lista[middle] = lista[first]; lista[first] = t; }

    // partition (first and last are already in correct half)
    p = lista[middle]; // pivot
    for (i=1,j=n-2;;i++,j--) {
        while (lista[i] < p) i++;
        while (p < lista[j]) j--;
        if (i>=j) break;

        t = lista[i]; lista[i] = lista[j]; lista[j] = t;      
    }

    // return position of pivot
    return i;
}


void quicksort(double *lista,int n) {
    int i;

    // debug only
    //printf("Sorting array of %d elements\n",n);

    // check if below cutoff limit
    if (n<=CUTOFF)
    {
        inssort(lista,n);
        return;
    }

    // partition into two halves
    i = partition(lista,n);

    quicksort(lista,i);
    quicksort(lista+i,n-i);
}


int main() {
    double *lista;
    int i = 0;
    // array of structs to fill and pass to threads on creation
    struct thread_params tparm[THREADS];
    // table of thread IDs (handles) filled on creation, to be used later on join
    pthread_t threads[THREADS];

    lista = (double *)malloc(N*sizeof(double));
    if (lista==NULL) {
        printf("error in malloc\n");
        exit(1);
    }

    // fill array with random numbers
    srand(0);
    for (i=0;i<N;i++) {
        lista[i] = (double)rand()/RAND_MAX;
    }

    for (i=0; i < THREADS; i++)
    {
        // create thread with default attrs (attrs=NULL)
        if (pthread_create(&threads[i],NULL,thread_func,&tparm[i])!=0)
        {
            printf("Error in thread creation!\n");
            exit(1);
        }
    }

    int shutdown = 0;
    send(shutdown);

    // edo prepei na exoume ena while gia na kanei receive kai na auksanei ena counter an to minima einai 1(terminated)

    // check sorting
    for (i=0;i<(N-1);i++)
    {
        if (lista[i] > lista[i+1])
        {
            printf("Sort failed!\n");
            break;
        }
    }



    // block on thread join
    for (i=0;i<THREADS;i++)
    {
        pthread_join(threads[i],NULL);
    }

    free(lista);
    // destroy mutex - should be unlocked
    pthread_mutex_destroy(&mutex);

    pthread_cond_destroy(&msg_in);
    pthread_cond_destroy(&msg_out);

    return 0;
}