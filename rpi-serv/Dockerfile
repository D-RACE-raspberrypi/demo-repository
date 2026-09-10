# bookworm : meme base Debian que le Raspberry Pi OS du Pi 5 (Debian 12, kernel 6.6)
FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y gcc make
WORKDIR /app
COPY include/ include/
COPY src/ src/
RUN gcc -Wall -Wextra -O2 -Iinclude src/server.c src/car.c src/moteur.c src/servo.c -o serveur_pi

FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends hostapd dnsmasq iproute2 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/serveur_pi .
CMD ["./serveur_pi", "5000"]