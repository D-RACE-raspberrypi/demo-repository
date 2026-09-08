# Tutos Docker du projet

Petits guides pour les actions courantes sur le conteneur `astrid` (image `d_race`).

## 1. Lancer le projet

```bash
# Build (si nécessaire) + démarrage, logs affichés dans le terminal
docker compose up --build
```

- Arrêter : `Ctrl+C` puis `docker compose down`
- Lancer en arrière-plan : `docker compose up -d --build`
- Voir les logs quand c'est en arrière-plan : `docker compose logs -f astrid`

> ⚠️ La manette doit être appairée/connectée **sur l'hôte** (le Raspberry Pi).
> Le conteneur y accède via `/dev/input` (monté dans le `docker-compose.yml`).

## 2. Mettre à jour l'image après une modif du code

Après avoir modifié `BT_controller/controller.c` (ou le `Dockerfile`) :

```bash
docker compose up --build
```

`--build` force la reconstruction de l'image avant le démarrage. Docker ne
recompile que ce qui a changé (cache de layers), c'est rapide.

Pour seulement reconstruire sans lancer :

```bash
docker compose build
```

## 3. Ajouter une dépendance (bibliothèque C)

Le `Dockerfile` a **2 étapes** : `build` (compilation) et l'image finale (exécution).
Une bibliothèque se rajoute donc à 2 endroits :

1. **Étape `build`** : le paquet de développement (`-dev`), pour les headers `.h` :

   ```dockerfile
   RUN apt-get update && apt-get install -y --no-install-recommends \
           gcc libc6-dev libsdl2-dev libcurl4-openssl-dev \
       && rm -rf /var/lib/apt/lists/*
   ```

2. **Étape finale** : la bibliothèque runtime (sans `-dev`), pour le `.so` :

   ```dockerfile
   RUN apt-get update && apt-get install -y --no-install-recommends \
           libsdl2-2.0-0 libcurl4 \
       && rm -rf /var/lib/apt/lists/*
   ```

3. Ajouter le flag de link à la ligne `gcc` si besoin (ex. `-lcurl`) :

   ```dockerfile
   RUN gcc -O2 -Wall -o controller controller.c -lSDL2 -lcurl
   ```

4. Reconstruire : `docker compose build`

Astuce pour trouver le nom des paquets Debian : `apt search <nom>` ou
chercher "debian package libXXX" — le paquet header finit presque toujours en `-dev`.

## 4. Ouvrir un shell dans le conteneur (debug)

```bash
# Conteneur déjà lancé :
docker compose exec astrid bash

# Ou lancer un conteneur jetable juste pour inspecter :
docker compose run --rm astrid bash
```

Utile pour vérifier que la manette est visible :

```bash
ls -l /dev/input/
```

## 5. Lancer le programme manuellement

```bash
docker compose run --rm astrid ./controller
```

`--rm` supprime le conteneur à la sortie (pas d'accumulation de conteneurs morts).

## 6. Arrêter et nettoyer

```bash
# Arrêter et supprimer conteneur + réseau
docker compose down

# Supprimer aussi l'image du projet
docker compose down --rmi local

# Grand ménage général (images/caches non utilisés — global à la machine !)
docker system prune
```

## 7. Voir l'état

```bash
docker compose ps        # conteneurs du projet
docker images            # images présentes (d_race doit apparaître)
docker compose logs -f   # logs en continu
```

## 8. Problèmes courants

| Symptôme | Cause probable | Solution |
|---|---|---|
| `Aucune manette détectée.` | Manette non connectée à l'hôte | Appairer/brancher la manette sur le Pi, vérifier `ls /dev/input/` sur l'hôte |
| La manette est sur l'hôte mais pas dans le conteneur | Volumes/devices manquants | Vérifier les sections `volumes:` et `device_cgroup_rules:` du `docker-compose.yml` |
| Modif du code sans effet | Image pas reconstruite | Relancer avec `docker compose up --build` |
| Erreur de compilation au build | Voir le message `gcc` dans la sortie du build | Corriger `controller.c`, rebuilder |
