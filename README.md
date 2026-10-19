# Simulador de una Máquina de Turing Determinista Multicinta

**Autor:** Marco Pérez Padilla  
**Correo:** alu0101469348@ull.edu.es  
**Asignatura:** Complejidad Computacional — Curso 2026/27  
**Práctica 2:** Simulador de una Máquina de Turing determinista

Simulador orientado a objetos, escrito en C++, de una Máquina de Turing
(MT) determinista con una o varias cintas. Dado un fichero de texto con la
definición formal de la máquina y una serie de cadenas de entrada, indica
para cada una si la MT se detiene en un estado de aceptación y muestra el
contenido final de la cinta 1, con la cabeza de L/E entre corchetes
(`ab[c]d`), con la opción de mostrar paso a paso la traza de la ejecución.

## Índice

- [Variaciones elegidas](#variaciones-elegidas)
- [Formato del fichero de definición](#formato-del-fichero-de-definición)
- [Máquinas de Turing diseñadas](#máquinas-de-turing-diseñadas)
- [Estructura del proyecto](#estructura-del-proyecto)
- [Diseño y decisiones técnicas](#diseño-y-decisiones-técnicas)
- [Manejo de errores](#manejo-de-errores)
- [Compilación y ejecución](#compilación-y-ejecución)
- [Tests](#tests)
- [Contenedor Docker](#contenedor-docker)
- [Repositorio](#repositorio)

## Variaciones elegidas

Según pide el enunciado, estas son las opciones seleccionadas en cada fila:

| Variación | Opción elegida |
|---|---|
| Escritura y movimiento | **Simultáneos**: cada transición lee, escribe y mueve a la vez |
| Movimientos | **L, R y S** (izquierda, derecha y no movimiento) |
| Cinta | **Infinita en ambas direcciones** |

Además, la máquina es **multicinta** y el número de cintas `k` se indica en
el propio fichero de configuración.

## Formato del fichero de definición

Sigue el formato del enunciado: comentarios con `#`, un elemento por línea y
las transiciones al final.

```txt
# Comentarios
q0 q1 q2        # conjunto Q
0 1             # alfabeto de entrada Σ
0 1 .           # alfabeto de cinta Γ
q0              # estado inicial
.               # símbolo blanco
q2              # conjunto F de estados finales
1               # número de cintas k
q0 0 q1 0 R     # δ(q0, 0) = (q1, 0, R), para 1 cinta
```

Cada transición tiene `2 + 3k` campos separados por espacios, en este orden:

```
origen  lectura_1..lectura_k  destino  escritura_1..escritura_k  movimiento_1..movimiento_k
```

Para una máquina de 2 cintas: `q0 0 . q0 0 0 R R` significa que en `q0`,
leyendo `0` en la cinta 1 y blanco en la 2, escribe `0` en ambas y mueve las
dos cabezas a la derecha. Los movimientos válidos son `L`, `R` y `S`.

Al ejecutar, la entrada se coloca únicamente en la cinta 1 con la cabeza en
su primer símbolo; el resto de cintas empiezan en blanco. En el teclado o
en el fichero de cadenas, `.` representa la cadena vacía.

La ejecución termina **únicamente por ausencia de transición aplicable**. La
cadena se acepta si la máquina se detiene en un estado de `F`.

## Máquinas de Turing diseñadas

Ficheros de configuración:

| Problema | Fichero de configuración | Cadenas de prueba |
|---|---|---|---|
| 1. `L = { aⁿbᵐ \| m ≥ n, n > 0 }` | `data/definitions/TM-anbm.txt` | `data/strings/TM-anbm_strings.txt` |
| 2. Sustituir la cadena por nº de `b` y nº de `a` en unario | `data/definitions/TM-count_symbols.txt` | `data/strings/TM-count_symbols_strings.txt` |

Además se incluyen los dos ejemplos dados con la práctica
(`example-odd_zeros.txt`, `example-double_unary.txt`) y un ejemplo de
2 cintas (`example-copy_to_tape2.txt`) para demostrar el soporte multicinta.

**Máquina 1.** Usa 2 cintas: la cinta 1 lleva la entrada y la cinta 2 actúa como contador. Por cada `a` leída en la cinta 1 se escribe una `X` en la cinta 2; al encontrar la primera `b`, cada `b` consumida borra una `X`. Las `b` que sobran (cuando ya no quedan `X`) se saltan, porque `m` puede ser mayor que `n`. Al terminar la entrada se acepta si no queda ninguna `X`, es decir, si `m ≥ n`. Como `q0` solo tiene transición con `a`, una cadena que empiece por `b` (o vacía) se rechaza, lo que impone `n > 0`. Una `a` después de las `b` (como en `aba`) no tiene transición y la máquina se detiene en un estado no final.

**Máquina 2.** Usa 3 cintas: la cinta 1 lleva la entrada (y acaba conteniendo el resultado), la cinta 2 cuenta las `b` y la cinta 3 cuenta las `a`, cada cuenta como un bloque de `1`s. En una sola pasada sobre la entrada, cada símbolo se borra (se marca como `X`) y se añade un `1` a la cinta de conteo correspondiente; la cabeza de esa cinta se mueve hacia la izquierda, así que al terminar queda sobre el blanco justo a la izquierda del bloque y un paso a la derecha basta para empezar a leerlo, sin tener que rebobinar. Luego se rebobina la cinta 1 hasta el inicio (usando el blanco a la izquierda de la entrada como centinela), se escribe el conteo de `b` de la cinta 2 (o un `0` si está vacío), un blanco separador, y el conteo de `a` de la cinta 3 (o un 0 si está vacío). Finalmente se retrocede por la cinta 1 hasta el primer carácter del resultado: como el separador también es un blanco, solo se trata como centinela izquierdo si la celda anterior también es blanca.

```
abbabaabb  ->  [1]1111·1111      (5 b's, 4 a's; · es el símbolo blanco)
aa         ->  [0]·11
bb         ->  [1]1·0
```

## Estructura del proyecto

```
.
├── Makefile
├── README.md
├── include/            # cabeceras (.h), organizadas por responsabilidad
│   ├── exceptions/      # jerarquía de excepciones (exceptions.h)
│   ├── model/           # Alphabet, Movement, Tape, TMTransition, TMTransitionTable, TMConfiguration, TuringMachine
│   ├── simulation/      # Simulator (la ejecución paso a paso)
│   ├── io/              # TuringMachineParser, WordChecker, Menu, Application, TraceWriter, Options
│   └── help/            # Help, Usage, PrintWarning/PrintError, ValidateArguments
├── src/                # implementación (.cc), misma organización que include/
├── tests/              # tests de GoogleTest, un fichero por cada .h/.cc de arriba
├── libs/googletest/     # código fuente de GoogleTest
└── data/
    ├── definitions/      # ficheros de definición (para -config): válidos, de error y de aviso
    └── strings/          # ficheros de cadenas de entrada (para -in)
```

## Diseño y decisiones técnicas

- **`Tape`**: una cinta infinita en ambas direcciones sobre un `std::deque`
  con un desplazamiento entre posición lógica (puede ser negativa) e índice,
  que solo crece una celda cada vez, en la dirección en que la cabeza se
  sale. Las celdas sin escribir leen como el blanco. `ToString` muestra desde
  la celda más a la izquierda hasta la más a la derecha visitadas, con la
  cabeza entre corchetes.
- **`TMTransition` multicinta**: `read`, `write` y `move` son vectores con
  una entrada por cinta, así que una máquina de 1 cinta no es un caso
  especial del código.
- **`TMTransitionTable::Find`** devuelve como mucho una transición
  (`std::optional`), porque la máquina es determinista.
- **`TuringMachine`** valida la definición completa en el constructor
  (ver [Manejo de errores](#manejo-de-errores)). No hay patrón Strategy/
  Factory para el criterio de aceptación, a diferencia de la práctica
  anterior: aquí solo hay uno (detenerse en un estado de `F`), y una jerarquía
  sin nada que elegir sería ceremonia.
- **`Simulator`**: al ser determinista, la ejecución es una línea recta, no
  una búsqueda en profundidad. El único freno frente a una MT que no termina
  es un número máximo de pasos (`ExecutionLimitExceededException`).
- **`TraceWriter` separado del simulador**: el simulador nunca escribe
directamente por pantalla; cambiar el formato de la traza no obliga a
tocar la lógica de la simulación. La traza no la exige el enunciado:
se añadió como herramienta de estudio, para poder seguir paso a paso qué
hace la máquina y depurar las definiciones propias (estados, transiciones
y movimientos de cada cabeza).
- **`Application` y `WordChecker`**: en lugar de funciones sueltas, agrupan
  el estado que antes se pasaba una y otra vez (opciones, autómata, flags y
  streams), de modo que solo varía la cadena entre llamadas. `Menu`,
  `WordChecker` y `Application` reciben los streams por inyección, así que
  se prueban con `istringstream`/`ostringstream`.

## Manejo de errores

Se distinguen dos niveles de gravedad:

- **Errores críticos** (`Error:` por `stderr`, código de salida `1`):
  fichero de definición inexistente o vacío; definición que incumple la
  definición formal de una MT (estado inicial fuera de `Q`, blanco fuera de
  `Γ` o dentro de `Σ`, `Σ ⊄ Γ`, estado final fuera de `Q`, símbolos de más de
  un carácter, número de cintas no positivo o no numérico, transición con un
  número de campos incorrecto, con estados o símbolos no declarados o con un
  movimiento que no sea `L`, `R` o `S`, definición sin transiciones, dos
  transiciones contradictorias para el mismo `(estado, lectura)`, es decir,
  una MT no determinista); argumentos de línea de comandos incorrectos o un
  fichero de `-in`/`-out` que no se puede abrir.
- **Errores no críticos** (`Warning:` por `stderr`, se continúa): una cadena
  con un símbolo fuera de `Σ`, una cadena cuya ejecución supera el límite de
  pasos (posible bucle infinito), una transición duplicada idéntica y una
  opción inválida del menú.

`data/definitions/` contiene un fichero por cada fallo (`error-*.txt`) y uno
para el aviso de transición duplicada (`warning-duplicate_transition.txt`);
`data/strings/invalid_symbols_strings.txt` dispara el aviso de símbolo
fuera de `Σ`:

```bash
./bin/mt -config data/definitions/error-invalid_movement.txt
./bin/mt -config data/definitions/TM-anbm.txt -in data/strings/invalid_symbols_strings.txt
```

## Compilación y ejecución

```bash
make            # compila el simulador -> bin/mt
make test       # compila y ejecuta la batería de tests -> bin/run_tests
make clean      # borra build/ y bin/
```

```bash
./bin/mt -config <fichero> [-trace] [-in <fichero>] [-out <fichero>]
```

| Opción          | Obligatoria | Descripción                                                         |
|-----------------|:-----------:|---------------------------------------------------------------------|
| `-config <f>`   | Sí          | Fichero con la definición de la MT                                  |
| `-trace`        | No          | Muestra la traza de la ejecución (todas las cintas, cada transición); no exigido por el enunciado |
| `-in <f>`       | No          | Cadenas a comprobar, una por línea; sin ella se piden por teclado    |
| `-out <f>`      | No          | Fichero donde escribir la traza (necesita `-trace`)                   |

Ejemplo, con el problema 2 y el ejemplo exacto del enunciado:

```bash
$ ./bin/mt -config data/definitions/TM-count_symbols.txt -in data/strings/TM-count_symbols_strings.txt
Machine loaded: 12 states, 3 tape(s), 1 accepting state(s)
'abbabaabb': ACCEPTED  tape: ..[1]1111.1111.
'aa': ACCEPTED  tape: ..[0].11.
```

Sin `-in`, el programa entra en un menú interactivo para comprobar cadenas
por teclado (`.` para la cadena vacía, `..` para volver) o desde un fichero.

## Tests

Se usa [GoogleTest](https://github.com/google/googletest), vendorizado en
`libs/googletest/`. `make test` compila y lanza toda la batería (166 tests).
Cada clase tiene su fichero de test en `tests/`, con la misma organización
por carpetas que `include/`, y cubre el comportamiento esperado y todos los
casos de error y excepciones. `tests/simulation/delivered_machines_test.cc`
carga las máquinas de `data/definitions/` y comprueba su comportamiento.

```bash
./bin/run_tests
```

## Contenedor Docker

El proyecto incluye `Dockerfile`, `docker-compose.yml` y una configuración
de devcontainer.

```bash
docker compose build
docker compose run --rm cpp-dev bash
make test
```

## Repositorio

Código fuente completo en GitHub:
`https://github.com/Marco-Perez-Padilla/Complejidad-Computacional.-P02-TM`
