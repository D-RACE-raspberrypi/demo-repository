FROM debian:trixie-slim AS build
RUN apt-get update && apt-get install -y gcc make
WORKDIR /app
COPY src/server.c .
RUN gcc -Wall -Wextra -O2 server.c -o serveur_pi

FROM debian:trixie-slim
RUN apt-get update && apt-get install -y --no-install-recommends hostapd dnsmasq iproute2 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/serveur_pi .
CMD ["./serveur_pi", "5000"]