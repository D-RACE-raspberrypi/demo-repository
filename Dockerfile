# --- Étape 1 : compilation ---
FROM debian:bookworm-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc libc6-dev libsdl2-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY BT_controller/controller.c .
RUN gcc -O2 -Wall -o controller controller.c -lSDL2

# --- Étape 2 : image d'exécution ---
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
        libsdl2-2.0-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /src/controller .

CMD ["./controller"]