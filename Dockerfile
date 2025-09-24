FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

COPY . .


RUN sed -i 's/#define NUM_PHILOSOPHERS 5/#define NUM_PHILOSOPHERS 7/' dining_philosophers.c && \
    sed -i 's/#define SIMULATION_TIME_SECONDS 10/#define SIMULATION_TIME_SECONDS 30/' dining_philosophers.c

RUN gcc -o dining_philosophers dining_philosophers.c -lpthread -lrt

CMD ["./dining_philosophers"]