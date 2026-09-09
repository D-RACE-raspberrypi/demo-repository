# --- Étape 1 : compilation ---
FROM debian:bookworm-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc libc6-dev libsdl2-dev libgpiod-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY src/ .
RUN gcc -O2 -Wall -I. -o main main.c BT_controller/controller.c BT_controller/gpio_rasp.c -lSDL2 -lgpiod

# --- Étape 2 : image d'exécution ---
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        libsdl2-2.0-0 libgpiod2 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /src/main .

CMD ["./main"]