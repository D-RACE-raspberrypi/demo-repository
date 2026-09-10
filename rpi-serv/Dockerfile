# bookworm : libgpiod v1 (API gpiod_chip_get_line) ; trixie ne fournit que la v2
FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y gcc make libgpiod-dev
WORKDIR /app
COPY include/ include/
COPY src/ src/
RUN gcc -Wall -Wextra -O2 -Iinclude src/server.c src/car.c -o serveur_pi -lgpiod

FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends hostapd dnsmasq iproute2 \
    libgpiod2 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/serveur_pi .
CMD ["./serveur_pi", "5000"]