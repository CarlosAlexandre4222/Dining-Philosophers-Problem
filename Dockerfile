FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN gcc -o jantar jantar.c -lpthread -lrt

CMD ["./jantar"]