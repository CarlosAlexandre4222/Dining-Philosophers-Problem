# 🍽️ Dining Philosophers Problem

Uma implementação completa do clássico problema dos "Filósofos Jantando" usando **POSIX threads** em C, com sistema de monitoramento em tempo real e análise de métricas de performance.

## 📋 Índice
- [Sobre o Projeto](#sobre-o-projeto)
- [Interface](#interface)
- [Pré-requisitos](#pré-requisitos)
- [Como Compilar e Executar](#como-compilar-e-executar)
- [Como Usar](#como-usar)
- [Divisão de Responsabilidades](#divisão-de-responsabilidades)
- [Autores](#autores)

## 🎯 Sobre o Projeto

O **Dining Philosophers Problem** é um dos problemas clássicos de sincronização em sistemas operacionais. Este projeto implementa uma solução completa que:

- **Evita deadlock**: Implementação segura que nunca trava o sistema
- **Monitora performance**: Coleta métricas detalhadas de tempo de espera e throughput
- **Interface rica**: Sistema de logs em tempo real com emojis e formatação profissional
- **Análise estatística**: Relatórios parciais e finais com médias e totais

### 🧠 O Problema
Cinco filósofos sentam ao redor de uma mesa circular. Entre cada par de filósofos há um garfo. Para comer, um filósofo precisa pegar ambos os garfos adjacentes (esquerdo e direito). O desafio é sincronizar o acesso aos garfos sem causar **deadlock** ou **starvation**.

### ✅ Nossa Solução
Implementamos o algoritmo baseado em **monitores** usando:
- **Mutex locks** para exclusão mútua
- **Condition variables** para sincronização eficiente
- **Estados (states)** (THINKING, HUNGRY, EATING)
- **Sistema de métricas** para análise de performance

## 🖥️ Interface

### Saída do Programa
```
Iniciando o Jantar dos Filósofos por 10 segundos...
---------------------------------------------------
🍽️  Filósofo 0 quer comer
🍜 Filósofo 0 começou a COMER! (Esperou 0.00 s)
🍽️  Filósofo 2 quer comer
🍜 Filósofo 2 começou a COMER! (Esperou 0.00 s)
🤔 Filósofo 0 terminou de comer.
🍽️  Filósofo 1 quer comer
🍜 Filósofo 1 começou a COMER! (Esperou 2.31 s)

==================== MÉTRICAS PARCIAIS ====================
| Filósofo | Refeições | Tempo Total Espera | Espera Média |
|----------|-----------|--------------------|--------------| 
|    0     |     2     |      0.45 s        |   0.23 s     |
|    1     |     1     |      2.31 s        |   2.31 s     |
|    2     |     2     |      0.12 s        |   0.06 s     |
|    3     |     1     |      1.78 s        |   1.78 s     |
|    4     |     2     |      0.89 s        |   0.45 s     |
=========================================================

!!! TEMPO ESGOTADO. FINALIZANDO A SIMULAÇÃO !!!

====================== RELATÓRIO FINAL ======================
| Filósofo | Refeições | Tempo Total Espera | Espera Média |
|----------|-----------|--------------------|--------------| 
|    0     |     3     |      0.67 s        |   0.22 s     |
|    1     |     2     |      3.45 s        |   1.73 s     |
|    2     |     3     |      0.23 s        |   0.08 s     |
|    3     |     2     |      2.89 s        |   1.45 s     |
|    4     |     3     |      1.12 s        |   0.37 s     |
-----------------------------------------------------------
Total de refeições servidas: 13
===========================================================
```

## 🛠️ Pré-requisitos

### Sistema Operacional
- **Linux** (Ubuntu, Debian, CentOS, etc.)

### Dependências
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential gcc
```

### Bibliotecas Necessárias
- `pthread` (POSIX Threads)
- `time.h` (para timestamps precisos)
- `signal.h` (para controle de execução)

## ⚙️ Como Compilar e Executar

### Compilação
```bash
# Clona o repositório
git clone https://github.com/carlosalexandre422/Dining-Philosophers-Problem.git
cd Dining-Philosophers-Problem

# Compila o programa
gcc -o dining_philosophers dining_philosophers.c -lpthread -lrt

# Ou use o Makefile (se disponível)
make
```

### Execução
```bash
# Execução padrão
./dining_philosophers

# Execução com redirecionamento de logs
./dining_philosophers > output.log 2>&1

# Execução em background
./dining_philosophers &
```

### Limpeza
```bash
# Remove executável
rm dining_philosophers

# Ou use o Makefile
make clean
```

## 📖 Como Usar

### Configuração
Você pode ajustar os parâmetros no início do arquivo `dining_philosophers.c`:

```c
#define NUM_PHILOSOPHERS 5           // Número de filósofos (padrão: 5)
#define SIMULATION_TIME_SECONDS 10   // Duração da simulação (padrão: 10s)
#define MAX_SEC 3                    // Tempo máximo para pensar/comer (padrão: 3s)
```

### Interpretando a Saída

#### Estados dos Filósofos
- 🤔 **THINKING**: Filósofo está pensando (não precisa de garfos)
- 🍽️ **HUNGRY**: Filósofo quer comer (esperando garfos)
- 🍜 **EATING**: Filósofo está comendo (possui ambos os garfos)

#### Métricas
- **Refeições**: Número total de vezes que o filósofo comeu
- **Tempo Total Espera**: Soma de todo tempo esperando para comer
- **Espera Média**: Tempo médio de espera por refeição

### Casos de Teste
```bash
# Teste rápido (5 segundos)
sed -i 's/#define SIMULATION_TIME_SECONDS 5/' dining_philosophers.c
gcc -o dining_philosophers dining_philosophers.c -lpthread -lrt
./dining_philosophers

# Teste com mais filósofos
sed -i 's/#define NUM_PHILOSOPHERS 7/' dining_philosophers.c
gcc -o dining_philosophers dining_philosophers.c -lpthread -lrt
./dining_philosophers
```

## 👥 Divisão de Responsabilidades

### 🧠 Túlio Neiva

#### Funções Implementadas:
- `dining_table_init()` - Inicialização do sistema de sincronização
- `test()` - Algoritmo central de decisão (coração do sistema)
- `pickup_forks()` - Lógica de aquisição de recursos
- `return_forks()` - Lógica de liberação de recursos  
- Sistema de métricas e timestamps

### 🎮 Carlos Alexandre  

#### Funções Implementadas:
- `philosopher_lifecycle()` - Ciclo de vida de cada filósofo
- `monitor_lifecycle()` - Sistema de monitoramento em tempo real
- `main()` - Orquestração geral e controle de threads
- Interface e sistema de logs

---

## 👨‍💻 Autores

| Desenvolvedor | GitHub | Email | Responsabilidade |
|---------------|--------|-------|------------------|
| **Túlio Neiva** | [@tuliorneiva](https://github.com/tuliorneiva) | tuliorneiva@gmail.com | Sincronização & Métricas |
| **Carlos Alexandre** | [@carlosalexandre422](https://github.com/carlosalexandre422) | carlos.alexandre@email.com | Threads & Interface |

---

<div align="center">
  <strong>🍽️ Dining Philosophers Problem - Implementação Completa</strong><br>
  <em>Sistemas Operacionais • POSIX Threads • Sincronização Avançada</em>
</div>

