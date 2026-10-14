# Engine Monitor — exercice C++98

Tu vas écrire, module par module, un petit logiciel de **surveillance de paramètres moteur**. Il lit un fichier de scénario (déclarations de capteurs, mesures, acquittements) et produit un journal horodaté des alarmes.

Le domaine est volontairement simple et ne demande aucun calcul : la difficulté est dans **l'écriture du code**. Il faut respecter des interfaces imposées, gérer la mémoire à la main, utiliser le polymorphisme et produire une sortie exacte au caractère près.

---

## 0. Mise en place

```bash
unzip engine-monitor.zip && cd engine-monitor
git init && git add . && git commit -m "Squelette initial"
```

Arborescence :

```
include/        en-têtes (interfaces imposées — voir règles ci-dessous)
src/            TES fichiers .cpp (vide au départ)
tests/unit/     tests unitaires par étape (ne pas modifier)
tests/scenarios/  fichiers d'entrée de bout en bout
tests/expected/   sorties attendues correspondantes
tests/run_tests.sh
Makefile
```

Commandes utiles :

| Commande | Rôle |
|---|---|
| `make test-logger` | compile et lance les tests de l'étape 1 (idem `test-sensor`, `test-registry`, `test-alarm`, `test-parser`) |
| `make` | compile le programme `bin/monitor` |
| `make scenarios` | tests de bout en bout (étape 6) |
| `make test` | tous les tests |
| `make memcheck` | tous les tests sous **valgrind** (fuites, accès invalides) |

Installe valgrind si besoin : `sudo apt install valgrind`.

> Conseil : **un commit par étape**, avec un message clair. Avant de commiter, `make test-<etape>` doit être vert. À partir de l'étape 3, `make memcheck` doit l'être aussi.

---

## Règles du jeu

**Compilation** : `g++ -std=c++98 -Wall -Wextra -pedantic -Werror`. Un seul warning et ça ne compile pas.

**Interdit (C++11 et plus)** : `auto`, `nullptr` (utilise `0`), `override`, `final`, range-based `for`, lambdas, `std::unique_ptr` / `shared_ptr`, `std::to_string`, `std::stoi`, initialisation par accolades `{}`, `= default`, `= delete`, `enum class`.

**En-têtes fournis (`include/`)** : la **partie publique est un contrat**, ne la modifie pas, car les tests en dépendent. La **partie privée** est une suggestion : tu peux ajouter, renommer ou supprimer des membres et méthodes privés.

**À toi d'écrire** : tous les `.cpp` dans `src/`, plus deux en-têtes manquants (`TemperatureSensor.h`, `PressureSensor.h`).

**Style attendu** (comme en projet industriel) :
- listes d'initialisation dans les constructeurs, dans l'ordre de déclaration des membres ;
- `const` partout où c'est possible (méthodes, paramètres par référence const) ;
- pas de `using namespace std;` ;
- gardes d'inclusion `#ifndef` dans chaque en-tête ;
- itérateurs explicites pour parcourir les conteneurs.

---

## Architecture

```
            main.cpp
               │
        MonitoringSystem ─────────────┐
        │         │          │         │
   CommandParser  │     AlarmManager  Logger
                  │
            SensorRegistry ──possède──> Sensor (abstraite)
                                          ├── TemperatureSensor
                                          └── PressureSensor
```

L'ordre des étapes suit les dépendances : chaque étape se teste seule.

---

## Format du fichier de scénario

Une commande par ligne. Les mots sont séparés par un ou plusieurs blancs (espaces, tabulations). Lignes vides et lignes commençant par `#` ignorées.

```
SENSOR <TYPE> <ID> <LABEL> <MIN> <MAX>    déclare un capteur (TYPE = TEMP ou PRES)
READ   <TIME> <ID> <VALUE>                 nouvelle mesure
ACK    <TIME> <ID>                         acquittement de l'alarme d'un capteur
REPORT <TIME>                              état de tous les capteurs
```

Toutes les valeurs numériques sont des **entiers**. `TIME` est un entier ≥ 0.

Exemple (`tests/scenarios/02_alarms.txt`, extrait) :

```
SENSOR TEMP T1 ENG1_EGT -50 900
READ 2 T1 950
ACK 4 T1
REPORT 5
```

---

## Étape 1 — `Logger` (`src/Logger.cpp`)

En-tête fourni : `include/Logger.h`. Lis les commentaires, ils font partie de la spécification.

Format exact d'une ligne :

```
[T+0042] INFO  message
[T+0007] WARN  message
[T+----] ERROR message          (quand time vaut NO_TIME)
```

- Le temps est écrit sur 4 chiffres, complété par des `0`.
- Le niveau occupe **5 caractères** : `INFO `, `WARN `, `ERROR`, suivi d'un espace puis du message.
- `count(level)` renvoie le nombre de lignes déjà écrites à ce niveau.

Points d'attention :
- le Logger garde une **référence** vers le flux : pourquoi la classe est-elle non copiable ?
- les manipulateurs de `<iomanip>` modifient l'état du flux de façon persistante, et un test le vérifie.

`make test-logger`

---

## Étape 2 — Hiérarchie de capteurs

Fichiers : `src/Sensor.cpp`, puis **à créer entièrement** : `include/TemperatureSensor.h`, `src/TemperatureSensor.cpp`, `include/PressureSensor.h`, `src/PressureSensor.cpp`.

`Sensor` est abstraite (voir `include/Sensor.h`). Chaque classe dérivée redéfinit les 4 méthodes virtuelles pures.

Valeurs dans `[min, max]` (bornes **incluses**) → `SEV_NONE`. En dehors :

| Type | `typeName()` | `unit()` | valeur > max | valeur < min |
|---|---|---|---|---|
| `TemperatureSensor` | `"TEMP"` | `"degC"` | `SEV_CRITICAL` (surchauffe) | `SEV_WARNING` |
| `PressureSensor` | `"PRES"` | `"psi"` | `SEV_WARNING` | `SEV_CRITICAL` (perte de pression) |

- `describe()` renvoie par exemple `T1 (ENG1_EGT) TEMP [-50..900 degC]`.
- `clone()` renvoie (en `Sensor*`) une **copie allouée avec `new`** qui a le bon type dynamique : un `PressureSensor` cloné reste un `PressureSensor`.
- `severityToString()` est à écrire dans `Sensor.cpp`.

Points d'attention :
- pourquoi le destructeur de `Sensor` doit-il être `virtual` ? Que se passe-t-il sinon dans `testPolymorphism` ?
- les membres de `Sensor` sont `protected` : les classes dérivées y accèdent directement.

`make test-sensor`

---

## Étape 3 — `SensorRegistry` (règle des trois)

Fichier : `src/SensorRegistry.cpp`. C'est **l'étape la plus importante** pour le poste : gestion de la mémoire à la main, sans pointeur intelligent.

Le registre **possède** les capteurs (`std::map<std::string, Sensor*>`) :
- le destructeur les détruit tous ;
- `add(sensor)` prend possession du capteur. S'il y a doublon d'identifiant, il le **détruit** et renvoie `false`. `add(0)` renvoie `false` ;
- `ids()` renvoie les identifiants triés (un indice : quelle est la propriété d'une `std::map` ?).

**Copie profonde obligatoire** : une copie (constructeur de copie ou `operator=`) possède ses propres capteurs, indépendants de ceux de l'original. Copier les pointeurs ne suffit pas, il faut copier les objets. Or tu ne connais que `Sensor*`, pas le type réel : c'est à ça que sert `clone()`.

Points d'attention :
- l'auto-affectation `a = a;` ne doit ni planter ni perdre de données ;
- dans `operator=`, que deviennent les capteurs que `*this` possédait avant ?
- évite de dupliquer le code entre destructeur, constructeur de copie et `operator=` ;
- pour aller plus loin, cherche l'idiome **copy-and-swap**.

`make test-registry` puis `make memcheck`. Aucune fuite tolérée.

---

## Étape 4 — `AlarmManager`

Fichier : `src/AlarmManager.cpp`. Au plus **une alarme par capteur**, stockée par valeur dans une `std::map<std::string, Alarm>`.

Règles de `update(sensorId, severity, time)` :

| Gravité reçue | Alarme existante ? | Effet | Retour |
|---|---|---|---|
| `SEV_NONE` | non | rien | `EVT_NONE` |
| `SEV_NONE` | oui | alarme **supprimée** | `EVT_CLEARED` |
| ≠ NONE | non | nouvelle alarme `ALARM_ACTIVE`, `raisedAt = time`, compteur total +1 | `EVT_RAISED` |
| ≠ NONE | oui, même gravité | rien (même si acquittée) | `EVT_NONE` |
| ≠ NONE | oui, gravité différente | nouvelle gravité, état remis à `ALARM_ACTIVE`, `raisedAt = time` | `EVT_UPDATED` |

- `acknowledge(id)` fait passer l'alarme en `ALARM_ACKNOWLEDGED`. Renvoie `false` s'il n'y a pas d'alarme ou si elle est déjà acquittée.
- `count()` compte les alarmes présentes (actives et acquittées), `unacknowledgedCount()` celles à l'état `ALARM_ACTIVE`, et `totalRaised()` le nombre de `EVT_RAISED` depuis le début.

Points d'attention :
- `find()` renvoie un pointeur sur un élément de la map : combien de temps reste-t-il valide ?
- pourquoi `Alarm` a-t-elle besoin d'un constructeur par défaut ? (indice : `std::map::operator[]`)

`make test-alarm`

---

## Étape 5 — `CommandParser`

Fichier : `src/CommandParser.cpp`. Analyse **syntaxique** uniquement : le parser ne connaît ni les capteurs ni les alarmes.

| Cas | Résultat |
|---|---|
| ligne vide, blanche, ou 1er mot commençant par `#` | `CMD_EMPTY` |
| mot-clé inconnu (sensible à la casse) | `CMD_INVALID`, error = `unknown command '<MOT>'` |
| nombre de mots incorrect, entier invalide, ou TIME < 0 | `CMD_INVALID`, error = `malformed <MOT-CLE> command` |
| sinon | `CMD_SENSOR` / `CMD_READ` / `CMD_ACK` / `CMD_REPORT`, champs renseignés |

Un entier est valide seulement si **tout** le mot est un entier : `12`, `-3` sont valides, mais `abc`, `12abc` et `1.5` ne le sont pas. `std::stoi` est interdit : utilise `std::istringstream`, et vérifie qu'il ne reste rien après le nombre.

Un type de capteur inconnu (`VIB`) ou une plage inversée (`min > max`) **ne sont pas** des erreurs de syntaxe. Le parser les accepte, et c'est l'étape suivante qui les refuse.

`make test-parser`

---

## Étape 6 — `MonitoringSystem` et `main`

Fichiers : `src/MonitoringSystem.cpp`, `src/main.cpp`. Il n'y a pas de test unitaire : on valide par les **tests de bout en bout**, en comparant la sortie au caractère près avec `tests/expected/*.out`.

### Comportement par commande

**`SENSOR`**, vérifications dans cet ordre :
1. `min > max` → `ERROR line N: invalid range for '<ID>'`
2. type ni `TEMP` ni `PRES` → `ERROR line N: unknown sensor type '<TYPE>'`
3. identifiant déjà utilisé → `ERROR line N: duplicate sensor '<ID>'`
4. sinon → `INFO  sensor registered: <describe()>`

Ces lignes sont toutes écrites avec `NO_TIME`.

**`READ`**
- capteur inconnu → `ERROR line N: unknown sensor '<ID>'` (avec le TIME de la commande) ;
- sinon : enregistre la valeur dans le capteur, évalue sa gravité, met à jour l'AlarmManager, puis :

| Événement | Ligne écrite |
|---|---|
| `EVT_RAISED` | `WARN  alarm raised: <ID> <GRAVITE> value=<V> <unité>` |
| `EVT_UPDATED` | `WARN  alarm updated: <ID> <GRAVITE> value=<V> <unité>` |
| `EVT_CLEARED` | `INFO  alarm cleared: <ID> value=<V> <unité>` |
| `EVT_NONE` | rien |

**`ACK`**
- capteur inconnu → `ERROR line N: unknown sensor '<ID>'`
- acquittement refusé → `ERROR line N: no unacknowledged alarm for '<ID>'`
- sinon → `INFO  alarm acknowledged: <ID>`

**`REPORT`** écrit une ligne d'en-tête, puis une ligne par capteur, **triée par identifiant**, toutes avec le TIME de la commande :

```
[T+0013] INFO  report: 3 sensors, 2 alarms (1 unacknowledged), 3 raised in total
[T+0013] INFO    P1 ENG1_OIL = 95 psi [WARNING ACKNOWLEDGED]
[T+0013] INFO    T1 ENG1_EGT = 1000 degC [CRITICAL ACTIVE]
[T+0013] INFO    T2 ENG2_EGT = --- degC [OK]
```

(2 espaces avant l'identifiant ; `---` si le capteur n'a encore reçu aucune valeur.)

**Ligne invalide** (`CMD_INVALID`) → `ERROR line N: <message du parser>` avec `NO_TIME`.

**Fin de traitement**, toujours écrite, après la dernière ligne :
```
[T+----] INFO  end of run: <nb lignes lues> lines, <nb WARN> warnings, <nb ERROR> errors
```

Les numéros de ligne commencent à 1 et comptent **toutes** les lignes du fichier, commentaires et lignes vides compris.

### `main`

```
bin/monitor <fichier_scenario>
```

| Situation | stderr | Code de retour |
|---|---|---|
| pas exactement 1 argument | `usage: <argv[0]> <scenario_file>` | 2 |
| fichier illisible | `error: cannot open '<fichier>'` | 2 |
| exécution sans ligne ERROR | — | 0 |
| au moins une ligne ERROR | — | 1 |

Le journal est écrit sur la **sortie standard**.

### Validation

```bash
make scenarios          # les 4 scénarios doivent être [ OK ]
make memcheck
```

En cas d'échec, le script affiche un `diff` (`-` attendu, `+` obtenu). Pour un seul scénario : `./tests/run_tests.sh bin/monitor 03_errors`.

Pense à un piège classique : si `add()` détruit le capteur en cas de doublon, que vaut un pointeur que tu utiliserais **après** l'appel ?

---

## Étape 7 (bonus) — Évolutions sur ton propre code

Tu travailles maintenant comme sur du code existant : avant de coder, rédige dans `EVOLUTIONS.md` une courte **analyse d'impact** qui répond à quatre questions. Quels fichiers vont changer ? Quels comportements existants pourraient casser ? Quels tests existants devront être relancés ? Quels nouveaux tests faut-il écrire ?

Ensuite, pour chaque évolution : une branche Git, un scénario `tests/scenarios/05_xxx.txt` avec sa sortie attendue **écrite à la main avant de coder**, l'implémentation, puis `make test && make memcheck`. Les 4 scénarios d'origine doivent rester verts (non-régression).

**E1 — Nouveau type de capteur `LEVEL`** (niveau carburant, unité `pct`).
Hors plage : en dessous du min → `SEV_CRITICAL`, au-dessus du max → `SEV_WARNING`. Exemple : `SENSOR LEVEL F1 FUEL_L 10 100`.

**E2 — Mesures hors séquence.**
Une commande `READ` dont le TIME est **strictement inférieur** au TIME de la dernière mesure acceptée pour ce capteur est rejetée. Elle produit `WARN  out of sequence reading for '<ID>' ignored` et ne modifie ni la valeur ni les alarmes.

**E3 — Commande `RESET <TIME>`.**
Elle supprime toutes les alarmes **acquittées**, sans toucher aux alarmes actives, et écrit `INFO  reset: <n> acknowledged alarms removed`.

Question de réflexion : pour E1, combien d'endroits du code as-tu dû toucher ? Qu'est-ce que ça dit de la fonction `createSensor` ?

---

## Auto-évaluation finale

- [ ] `make test` et `make memcheck` sont verts
- [ ] aucune fonctionnalité C++11 (le Makefile compile déjà en `-std=c++98 -pedantic -Werror`)
- [ ] aucun `delete` oublié, aucun double `delete`
- [ ] chaque classe qui possède une ressource a soit les 3 (destructeur, copie, affectation), soit est explicitement non copiable
- [ ] chaque méthode qui ne modifie pas l'objet est `const`
- [ ] les messages de sortie ne sont pas dupliqués en dur à plusieurs endroits
- [ ] un commit par étape, avec des messages lisibles

Quand tu as fini (ou si tu bloques), envoie ton code : je t'enverrai la correction commentée et une revue de ton code.
