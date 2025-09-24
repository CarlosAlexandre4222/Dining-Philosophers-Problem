FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN gcc -o dining_philosophers dining_philosophers.c -lpthread -lrt

CMD ["./dining_philosophers"]