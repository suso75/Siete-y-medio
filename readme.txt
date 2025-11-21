# Siete y Medio

## Descripción
Este proyecto es una implementación en C++ del popular juego de cartas "Siete y medio". 
El jugador compite contra la máquina (la banca) con el objetivo de acercarse lo máximo posible a 7.5 puntos sin pasarse.

## Autores
- Jesus Goyena
- Teresa Serrano

## Archivos del Proyecto
- `Practica_final.cpp`: Código fuente principal del juego.
- `dinero.juego`: Fichero de texto que almacena el registro de jugadores y sus saldos.
- `Ultima.partida`: Fichero binario que guarda los datos de la última partida jugada.

## Compilación y Ejecución
Para compilar el programa, necesitas un compilador de C++ (como g++).

Comando de compilación:
g++ Practica_final.cpp -o SieteYMedio

Para ejecutar el juego:
./SieteYMedio  (en Linux/Mac)
SieteYMedio.exe (en Windows)

## Cómo Jugar
El juego cuenta con un menú principal con las siguientes opciones:
1. **Identificar a un nuevo jugador**: Registra tu nombre o inicia sesión si ya has jugado.
2. **Mostrar el dinero disponible**: Consulta tu saldo actual.
3. **Comenzar una partida nueva**: Inicia una nueva ronda contra la banca.
4. **Mostrar última partida jugada**: Visualiza los detalles de la partida anterior.
5. **Salir del juego**: Guarda el progreso y cierra la aplicación.

### Reglas Básicas
- Las cartas tienen su valor numérico, excepto sota, caballo y rey que valen 0.5 puntos.
- Debes pedir cartas o plantarte para acercarte a 7.5.
- Si te pasas de 7.5, pierdes.
- Ganas si tienes más puntos que la banca sin pasarte, o si la banca se pasa.
