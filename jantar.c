#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define NUM_PHILOSOPHERS 5
#define SIMULATION_TIME_SECONDS 10
#define MAX_SEC 3 // tempo máximo (em segundos) que um filósofo pode pensar/comer

// variavel global que controla execucao das threads
volatile sig_atomic_t simulation_running = 1;

enum State
{
    THINKING,
    HUNGRY,
    EATING
};

typedef struct
{
    enum State state[NUM_PHILOSOPHERS];
    pthread_mutex_t mutex;
    pthread_cond_t self[NUM_PHILOSOPHERS];
    // metricas
    int meals_eaten[NUM_PHILOSOPHERS];
    double total_wait_time[NUM_PHILOSOPHERS];
    struct timespec time_entered_hungry[NUM_PHILOSOPHERS];

} DiningTable;

double get_time_in_seconds()
{
   // função de timestamp a partir do clock do hardware
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 1e-9; // converte nanosegundos para segundos
}

// inicializa a mesa de jantar (mutexes, estados, etc)
void dining_table_init(DiningTable *table)
{
    pthread_mutex_init(&table->mutex, NULL); // inicializa o mutex
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        table->state[i] = THINKING; 
        pthread_cond_init(&table->self[i], NULL); // conditional variables
        // counters de métricas
        table->meals_eaten[i] = 0;
        table->total_wait_time[i] = 0.0;
    }
}

void dining_table_destroy(DiningTable *table)
{
    pthread_mutex_destroy(&table->mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        pthread_cond_destroy(&table->self[i]);
    }
}

void test(DiningTable *table, int philosopher)
{
    // vizinhos (fila circular)
    int left = (philosopher + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (philosopher + 1) % NUM_PHILOSOPHERS;

    // verifica se o filósofo pode comer (vizinhos não estão comendo = garfos disponíveis)
    if (table->state[philosopher] == HUNGRY &&
        table->state[left] != EATING &&
        table->state[right] != EATING)
    {

        table->state[philosopher] = EATING;

        // calculo do tempo de espera
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double wait_time = (now.tv_sec - table->time_entered_hungry[philosopher].tv_sec) +
                           (now.tv_nsec - table->time_entered_hungry[philosopher].tv_nsec) * 1e-9;

        table->total_wait_time[philosopher] += wait_time;
        table->meals_eaten[philosopher]++;

        printf("🍜 Filósofo %d começou a COMER! (Esperou %.2f s)\n", philosopher, wait_time);

        // sinaliza o filósofo que ele pode comer
        pthread_cond_signal(&table->self[philosopher]);
    }
}

void pickup_forks(DiningTable *table, int philosopher)
{
    // tenta pegar os garfos (entrar na seção crítica)
    pthread_mutex_lock(&table->mutex);
    
    table->state[philosopher] = HUNGRY;
    clock_gettime(CLOCK_MONOTONIC, &table->time_entered_hungry[philosopher]);

    printf("🍽️  Filósofo %d quer comer\n", philosopher);

    // chama a função para ver se pode comer
    test(table, philosopher);

    // se não conseguiu comer, espera o sinal
    if (table->state[philosopher] != EATING)
    {
        pthread_cond_wait(&table->self[philosopher], &table->mutex);
    }

    // sai da seção crítica
    pthread_mutex_unlock(&table->mutex);
}

void return_forks(DiningTable *table, int philosopher)
{
    // tenta devolver os garfos (entrar na seção crítica)
    pthread_mutex_lock(&table->mutex);

    table->state[philosopher] = THINKING;
    printf("🤔 Filósofo %d terminou de comer.\n", philosopher);

    // confere vizinhos
    int left = (philosopher + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (philosopher + 1) % NUM_PHILOSOPHERS;

    // vê se algum vizinho pode comer agora
    test(table, left);
    test(table, right);

    // sai da seção crítica
    pthread_mutex_unlock(&table->mutex);
}

typedef struct
{
    int id;
    DiningTable *table;
} philosopher_args;

void *philosopher_lifecycle(void *args)
{
    // extrai argumentos e os atribui
    philosopher_args *p_args = (philosopher_args *)args;
    int id = p_args->id;
    DiningTable *table = p_args->table;

    //  verifica flag global
    while (simulation_running)
    {
        // pensa por um tempo aleatório entre 1 e MAX_SEC segundos
        int think_time = (rand() % MAX_SEC) + 1;
        sleep(think_time);

        if (!simulation_running)
            break;

        pickup_forks(table, id);

        int eat_time = (rand() % MAX_SEC) + 1;
        sleep(eat_time);

        return_forks(table, id);
    }
    printf("Filósofo %d está saindo.\n", id);
    return NULL;
}

void *monitor_lifecycle(void *arg)
{
    DiningTable *table = (DiningTable *)arg;

    while (simulation_running)
    {
        sleep(5); // imprime tabela a cada 5 segundos
        if (!simulation_running)
            break;

        pthread_mutex_lock(&table->mutex);
        printf("\n==================== MÉTRICAS PARCIAIS ====================\n");
        printf("| Filósofo | Refeições | Tempo Total Espera | Espera Média |\n");
        printf("|----------|-----------|--------------------|--------------|\n");
        for (int i = 0; i < NUM_PHILOSOPHERS; i++)
        {
            // calcula espera média
            double avg_wait = (table->meals_eaten[i] == 0) ? 0 : table->total_wait_time[i] / table->meals_eaten[i];
            printf("|    %d     |    %3d    |     %8.2f s     |   %6.2f s   |\n",
                   i, table->meals_eaten[i], table->total_wait_time[i], avg_wait);
        }
        printf("=========================================================\n\n");
        pthread_mutex_unlock(&table->mutex);
    }
    return NULL;
}

int main()
{
    srand(time(NULL));

    // criacao e inicializacao da mesa de jantar
    DiningTable table;
    dining_table_init(&table);

    // cria as threads dos filósofos
    pthread_t philosophers[NUM_PHILOSOPHERS];
    philosopher_args args[NUM_PHILOSOPHERS];
    pthread_t monitor_thread;

    printf("Iniciando o Jantar dos Filósofos por %d segundos...\n", SIMULATION_TIME_SECONDS);
    printf("---------------------------------------------------\n");

    // inicia as threads dos filósofos atribuindo IDs e a mesa
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        args[i].id = i;
        args[i].table = &table;
        pthread_create(&philosophers[i], NULL, philosopher_lifecycle, &args[i]);
    }

    // cria a thread monitora
    pthread_create(&monitor_thread, NULL, monitor_lifecycle, &table);

    // simulação roda pelo tempo definido
    sleep(SIMULATION_TIME_SECONDS);

    // sinaliza p tds as threads terminarem
    printf("\n!!! TEMPO ESGOTADO. FINALIZANDO A SIMULAÇÃO !!!\n\n");
    simulation_running = 0;

    // espera tds as threads terminarem
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        pthread_join(philosophers[i], NULL);
    }
    pthread_join(monitor_thread, NULL);

    printf("\n====================== RELATÓRIO FINAL ======================\n");
    printf("| Filósofo | Refeições | Tempo Total Espera | Espera Média |\n");
    printf("|----------|-----------|--------------------|--------------|\n");
    int total_meals = 0;
    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        total_meals += table.meals_eaten[i];
        double avg_wait = (table.meals_eaten[i] == 0) ? 0 : table.total_wait_time[i] / table.meals_eaten[i];
        printf("|    %d     |    %3d    |     %8.2f s     |   %6.2f s   |\n",
               i, table.meals_eaten[i], table.total_wait_time[i], avg_wait);
    }
    printf("-----------------------------------------------------------\n");
    printf("Total de refeições servidas: %d\n", total_meals);
    printf("===========================================================\n\n");

    dining_table_destroy(&table);

    return 0;
}
