/**********************Practica_final.cpp*********************************/ /**
 *
 * @file Practica_final.cpp
 *
 * Proposito: El programa es una version del juego de cartas del siete y medio.
 *              En esta version el jugador juega contra la maquina y su objetivo
 *              es lograr acercarse lo maximo posible a los 7.5 puntos sin 
 *              pasarse. El programa permite ademas el registro de jugadores y 
 *              por medio de almacenar la informcion en ficheros permite a los 
 *              jugadores mediante su nombre recuperar el saldo de apuestas 
 *              anteriores.  Ademas existe la opcion de ver partidas anteriores 
 *              y consultar el saldo.
 *
 * @author Jesus Goyena
 * @author Teresa Serrano 
 * @version 1.0
 * @date 
 */ /**************************************************************************/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <ctime>
#include <fstream>
using namespace std;

/*
* Declaracion de constantes
*/
const int MAX = 40; ///< Numero maximo de cartas
const float PT_MAX = 7.5; ///< Numero al que aproximarse
const int MAX_C = 30; ///< Numero maximo de cartas que se van a repartir
const int MAX1 = 5; ///< Total de palos (contando que el 0 no es un palo)
const int MAX2 = 13; ///< Total de numeros por palo (el 0 no es considerado un numero)
const int MAX3 = 9; ///< Total de lineas del dibujo de la carta
const string N_IN = "dinero.juego"; ///< Nombre del fichero con todos los registros
const string N_OUT = "Ultima.partida"; ///< Nombre del fichero binario con los datos de la ultima partida registrada

/*
* Vector con los puntos de cada jugada
*/
typedef float Puntos [MAX];

/*
* Vector con la representacion de las cartas
*/
typedef string D_CARTA[MAX1][MAX2][MAX3];

/*
* Informacion de una carta
*/
struct Carta
{
    int palo;
    int numero;
}; 

/*
* Informcion relativa al jugador
*/
struct Jugador
{
    int saldo;
    string nombre;
}; 

/*
* Vector informacion de todos los jugadores
*/
typedef Jugador Jugadores[MAX];

/*
* Vector con la informcion cartas jugadas.
*/
typedef Carta Cartas [MAX];

/*
* Vector con los numeros jugados
*/
typedef int Numeros_Jugados[MAX];

/*
* CABECERA DE FUNCIONES
*/
float AtribucionPuntos(Carta); // Atribuye los puntos que vale cada carta segun su numero 
int GenerarNumero(void); //Genera un numero aleatorio, paso como parametro un int para que sepa si generar el numero del palo o de la carta
bool CartaRepetida(Numeros_Jugados, int, int); //Comprueba si la nueva carta generada esta repetida o no
float CalcularPuntos(Puntos, int); //Calcula los puntos totales de un jugador a traves de un vector
bool IdentificarGanador(float, float); //Dice a partir de un booleano si el usuario ha ganado (true) o ha perdido (false) --> He contado el empate como victoria para el usuario
bool DeclararGanador(Jugador &, float, float, int, Jugadores, int);
void MostrarCartas(D_CARTA, Cartas, int); //Muestra las cartas
void IniciarDibujoCarta (D_CARTA); //Inicia el dibujo de las cartas
void LeerDatosJugadores(Jugadores, int &); //Lee los datos de los jugadores
void IdentificarJugador(Jugador &, Jugadores, int &, int &); //Identifica al jugador dentro de los jugadores registrados y pasa su posicion por referencia, si no esta registrado, lo añade al vector en otra casilla nueva
void EliminarJugador(Jugadores, int, int &); //Elimina el jugador eliminado del vector
void GuardarUltimaJugada(Cartas, Cartas, int, int, float, float, bool, int); //Guarda los datos necesarios para escribir la ultima jugada en un archivo binario
bool ApuestaEnRango(int, int); //Comprueba si la apuesta del usuario esta dentro de las posibilidades
void EscribirDatosJugadores(Jugadores, int); //Escribe el vector con todos los jugadores registrados hasta el momento
void MostrarUltimaJugada(void); //Muestra la ultima jugada la cual es leida de un fichero binario
void IniciarBaraja(Cartas); //Genera la baraja sin barajar en un vector
void Barajar (Cartas, Cartas); //Baraja la baraja generada en la funcion "IniciarBaraja"
void PedirApuesta(int &, Jugador); //Pide la apuesta al usuario
void ComprobarEliminado(Jugadores, Jugador &, int, int); //Comprueba si el jugador ha sido eliminado 
string aMayus(string s);
/*
* Funcion principal
*/
int main (void)
{
    ///Declaracion de variables
    /// Variables que sirven para el usuario
    bool seguir = true; ///- Si seguimos pidiendo carta 
    char decision; ///- Si quiere pedir otra carta o plantarse
    int jugadas = 0; ///- Numero de jugadas del usuario
    int apuesta; ///- Dinero apostado
    int posicion_jugador; ///- Posicion del fichero en la que esta el jugador
    char opcion; ///- Opcion elejida
    /// Variables que sirven para el turno de la maquina
    int turnos = 0; ///- Numero de turnos de la maquina (la banca)
    int rango_juego = 6; ///- Rango de puntuacion que la banca debe sobrepasar para plantarse
    bool plantado; ///- Si la banca se ha plantado
    /// Variables que sirven para ambos turnos
    Carta carta_usuario, carta_banca; ///- Carta generada para el usuario y para la banca respectivamente
    Cartas Baraja_Espanyola; ///- Vector con los datos de todas las cartas de la baraja española excepto el 8 y 9 (en orden)
    Cartas Registro_Usuario; ///- Vector con todas las cartas repartidas para el usuario
    Cartas Registro_Banca; ///- Vector con todas las cartas repartidas para la banca
    Cartas cartas_jugadas; ///- Registro con las cartas aleatoriamente barajadas que se repartiran
    D_CARTA d_carta; ///- Matriz con todas las cartas guardadas
    Puntos j1, j2; ///- Vectores con los puntos que suman en cada ronda (siendo j1 el usuario y j2 la banca)
    Jugadores jugadores_registrados; ///- Vector con todos los jugadores con sus respectivos saldos leidos
    Jugador usuario; ///- Variable con los datos del usuario
    int num_registros = 0; ///- Numero de jugadores leidos
    int ronda = 0; ///- Recuento de si se reparte una carta para cada uno o solo una carta para uno (la banca o el usuario)
    bool ganador; ///- Si el usuario ha ganado o no
    bool final; ///- Si se ha terminado de repartir
    
    /// Leer la informcion registro de los jugadores en los ficheros.
    LeerDatosJugadores(jugadores_registrados, num_registros);
    
    /// Iniciar el dibujo de las cartas.
    IniciarDibujoCarta(d_carta);
    
    /// Descripcion del juego
    cout << endl;
    cout << "___________________SIETE Y MEDIO__________________________";
    cout << endl;
      
    ///Mostrar Menú
    do
    { 
    
        ///  Mostrar Menu de opciones
        cout << endl << endl;
        cout << "___________________OPCIONES DEL MENU______________________";
        cout << endl;
        cout << "        (1)  Identificar a un nuevo jugador." << endl;
        cout << "        (2)  Mostrar el dinero disponible." << endl;
        cout << "        (3)  Comenzar una partida nueva." << endl;
        cout << "        (4)  Mostar ultima partida jugada." << endl;
        cout << "        (5)  Salir del juego." << endl;
        cout << "__________________________________________________________";
        cout  << endl;
        
        /// Pedimos al usuario que nos diga una opcion
        cout << " Introduce la opcion seleccionada: ";
        cin >> opcion; 
        
        /// SI la opcion seleccionada es la 1 4 o 5 o el usuario ya se ha identificado
        if (opcion >= '4' || opcion == '1' || usuario.nombre != "")
        {
            /// SEGUN la opcion elegida:
            switch (opcion)
            {
                /// Opcion 1 : Nuevo registro jugador
                case '1':
                    ///- Pedimos el nombre al usuario
                    cout << "\nDime tu nombre: ";
                    cin >> usuario.nombre; 
                    ///-Identificamos al jugador en el fichero
                    IdentificarJugador(usuario, jugadores_registrados,
                    num_registros, posicion_jugador);
                
                    ///- Guardamos la informacion leida del fichero en el usuario
                    usuario = jugadores_registrados[posicion_jugador];
                        
                    break;
                /// Opcion 2: Dinero disponible para apostar
                case '2':
                        ///- Mostramos el dinero disponible para el usuario
                        cout << "\nEl jugador " << usuario.nombre << 
                        " tiene a su\ndisposicion ";
                        cout << usuario.saldo << " euros para\napostar\n\n";
                        
                    break;
                /// Opcion 3: Nueva partida     
                case '3': 
                    ///- Reiniciar valores del juego
                    turnos = 0;
                    jugadas = 0; 
                    seguir = true;
                    final = plantado = false;
                    ronda = 0;
                    
                    /// PARA i = 0 HASTA QUE i <= MAX
                    for (int i = 0; i <= MAX; i++)
                        ///- Reiniciamos los valores de los vectores puntos de los jugadores a 0 
                        j1[i] = j2[i] = 0;
                    /// FIN_PARA
                    
                    ///- Iniciamos la baraja sin barajar
                    IniciarBaraja(Baraja_Espanyola);
                    
                    ///- Barajamos la baraja
                    Barajar(cartas_jugadas, Baraja_Espanyola);
                    
                    /**
                     * Mostramos que la baraja ha sido barajada
                     * dando pie al inicio de la partida
                     */
                    cout << "\nBaraja barajada\n\n";
                    
                    ///- Turno del Usuario
                    cout << "Carta para el usuario:\n\n";
                    
                    /// HACER
                    do
                    {    
                        ///- SI el usuario no se ha plantado:   
                        if (seguir)
                        {
                            ///- 1. Se le raparte una carta al usuario
                            carta_usuario = cartas_jugadas[ronda];
                            Registro_Usuario[jugadas] = carta_usuario;
                            
                            ///- 2.Se le atribuyen los puntos correspondientes
                            j1[jugadas] = AtribucionPuntos(carta_usuario);
                            
                            ///- 3.Se le muetran las cartas jugadas y los puntos
                            MostrarCartas(d_carta, Registro_Usuario, jugadas);
                            cout << "Puntos: " << CalcularPuntos(j1, jugadas);
                            
                            ///- 4. Comprobar que sean inferiores a 7.5 puntos
                            if (CalcularPuntos(j1, jugadas) > 7.5)
                                ///- Si lo supera el usuario se planta directo
                                seguir = false;
                        }
                        ///SINO
                        else 
                            ///- La carta no es repartida al usuario
                            ronda --;
                        
                        ///- SI la banca no se ha plantado:
                        if (!plantado)
                        {
                            ///- 1. Repartir carta (la siguiente de la baraja)
                            carta_banca = cartas_jugadas[ronda + 1];
                            ///- 2.Registrar su jugada
                            Registro_Banca[turnos] = carta_banca;
                            ///- 3. Otorgarle los puntos correspondientes.
                            j2[turnos] = AtribucionPuntos(carta_banca);    
                        }
                        /// SINO
                        else
                            ///- La carta no es repartida para la banca
                            ronda --;
                        /// FIN_SI
                        cout << endl;
                        
                        ///- Si es la primera ronda pedirle apuesta al jugador
                        if (jugadas == 0 && seguir)
                            PedirApuesta(apuesta, usuario);
                        
                        /// SI el usuario no se ha plantado   
                        if(seguir)
                        {
                            ///- Pedirle si quiere mas cartas o se planta
                            cout << "\nMas cartas o te plantas (+/o): ";
                            cin >> decision;
                        }
                        /// SI decision == 'o'
                        if (decision == 'o')
                            ///- El usuario se ha plantado
                            seguir = false;
                        /**
                         *  SINO SI el jugador no se ha plantado 
                         *  (ya que puede haberlo hecho automaticamente si ha
                         *  superado los 7.5 puntos)
                         */
                        else if(seguir)
                        {
                            ///- Se sigue repartiendo carta al usuario
                            cout << "Nueva carta para el usuario:\n";
                            jugadas++;
                        } 
                        /// FIN_SI
                        
                        /** 
                         * SI la banca ha sobrepasado el rango de puntuacion
                         * que debe superar
                         */
                        if (CalcularPuntos(j2, turnos + 1) > rango_juego)
                            ///- La banca se planta
                            plantado = true;
                        /// SINO
                        else
                            /// Se sigue repartiendo carta a la banca
                            turnos++;
                        /// FIN_SI
                        
                        /// SI tanto la banca como el jugador se plantan
                        if (plantado && !seguir)
                            ///- Se termina de repartir
                            final = true;
                        ///FIN_SI
        
                        ///- Aumentamos el valor de la posicion de la carta a repartir en 2
                        ronda += 2;   
                    }
                    /// MIENTRAS jugadas <= MAX y no se haya terminado de repartir
                    while (jugadas <= MAX && !final);/// FIN_HACER_MIENTRAS
                    
                    /**
                     *  Ahora tan solo mostramos las cartas que se han repartido
                     *  a la banca, los puntos que ha ido obteniendo y si se ha
                     * plantado o no
                     */
                    cout << "Juega la banca\n\n";
                    /// PARA i = 0 MIENTRAS i <= turnos
                    for (int i = 0; i <= turnos + 1; i++)
                    {   
                        /// SI no es el turno en el que se planta
                        if (i != turnos + 1)
                        {
                            ///- Mostrar las cartas y los puntos acumulados
                            MostrarCartas(d_carta, Registro_Banca, i);
                            cout << "Puntos: " << CalcularPuntos(j2, i) << endl;
                            /// SI no es la ultima reparticion 
                            if (i != turnos)
                            {
                                ///- Mostramos el mensaje de siguiente jugada
                                cout << "\nSiguiente jugada\n";
                                system("pause");
                                cout << endl;
                            }/// FIN_SI
                        }
                        /// SINO
                        else 
                            ///- Mostramos que la banca se planta
                            cout << "\nMe planto\n"; /// FIN_SI        
                    }/// FIN_PARA
                    
                    /// Declarar un ganador cuando se ha terminado la partida
                    ganador = DeclararGanador(usuario, CalcularPuntos(j1, 
                    jugadas), CalcularPuntos (j2, turnos), apuesta, 
                    jugadores_registrados, posicion_jugador);
                    
                    /// Guardar los datos de la ultima partida en un fichero.
                    GuardarUltimaJugada(Registro_Usuario, Registro_Banca,
                    turnos, jugadas, CalcularPuntos(j2, turnos),
                    CalcularPuntos (j1, jugadas), ganador, apuesta);
                    
                    /* 
                    * Comprobar si se ha eliminado al juagador en el caso 
                    * de que haya perdido todo el dinero para apostar
                    * En caso afirmativo se eliminara al jugador
                    */
                    ComprobarEliminado (jugadores_registrados, usuario, 
                    posicion_jugador, num_registros);
                    
                    break;
                /// Opcion 4: Ver la ultima partida 
                case'4':
                    
                    cout << endl;
                    ///- Mostramos la ultima jugada leida del fichero binario
                    MostrarUltimaJugada();
                    
                    cout << endl;
                    
                    break; 
                /// Opcion 5 : Salir del juego.
                case '5':
                    /*
                    *   Guardamos los datos del vector con todos los 
                    *   jugadores registrados
                    */
                    EscribirDatosJugadores(jugadores_registrados, 
                    num_registros);
                        
                    break;
                /// Opcion por defecto: Indicar error y volver al menu principal
                default:
                    cout << "\nOpcion no valida\n\n";
                    break;
            } /// FIN_SEGUN
        }
        /// SINO 
        else
            ///- Indicar al usuario que antes debe identificarse
            cout << "Identificate antes de hacer nada\n\n";
        /// FIN_SI
    }
    while (opcion != '5');/// Fin_MIENTRAS
    
    return 0;
}
/***************************AtribucionPuntos*********************+*********/ /**
 *
 * @brief Atribucion de Puntos                                                                                                                                                                                                                              
 * 
 * Proposito: Atribuye los puntos que vale una carta segun su numero
 *
 * @param [in] carta (Carta)  Carta a valorar
 *
 * @return puntos (float) Valor de la carta correspondiente
 */ /**************************************************************************/
float AtribucionPuntos (Carta carta)
{
    float puntos;

    if (carta.numero < 10)
        puntos = carta.numero;
    else
        puntos = 0.5;
    
    return puntos;      
}
/***************************GenerarNumero**********************************/ /**
 *
 * @brief GenerarNumero
 * 
 * Proposito: Genera un numero aleatorio del 0 a 40 siendo este el numero maximo
 *            de cartas 
 *
 * @return num (int) Numero generado aleatoriamente
 */ /**************************************************************************/
int GenerarNumero(void)
{
    int num = 0;
    int tope;
    tope = rand()%(20);
    for (int i = 0; i < tope; i++)
        num = rand()%(MAX);
        
    return num;
}

/******************************CartaRepetida*******************************/ /**
 *
 * @brief Carta Repetida
 * 
 * Proposito: Comprueba si un numero generado aleatoriamente el cual esta ligado
 *            a una carta en concreto ha salido anterioriormente o no.
 * 
 * @param [in] nj (Numeros_Jugados) Vector con todos los numeros generados
 * @param [in] n (int) Nuevo numero generado
 * @param [in] j (int) Cantidad de numeros generados con anterioridad
 *
 * @retval true Carta Repetida
 * @retval false Carta no repetida
 */ /**************************************************************************/
bool CartaRepetida (Numeros_Jugados nj, int n, int j)
{
    
    bool ok = false;
    
    for (int i = 0; i < j; i++)
    {
        if (nj[i] == n)
            ok = true;
    }
        
    return ok;
}
/*******************************CalcularPuntos*****************************/ /**
 *
 * @brief CalcularPuntos
 * 
 * Proposito: La funcion calcula los puntos que tiene cada jugador en funcion
 *            de las cartas que le han ido saliendo. Para ello utiliza el 
 *            historico de puntos y el numero de jugadas.
 *
 * @param [in] p (Puntos) Vector con los puntos obtenidos en cada ronda
 * @param [in] n (int) Total de jugadas
 * 
 * @return points (float) Total de puntos
 */ /**************************************************************************/
float CalcularPuntos(Puntos p, int n)
{
    
    float points = 0;
    
    for (int i = 0; i <= n; i++)
        points += p[i];
    
    return points;
}

/***************************IdentificarGanador*****************************/ /**
 *
 * @brief Identificar Ganador
 * Proposito:   La funcion compara los puntos de la maquina y del jugador 
 *              y en funcion de eso identifaca el que mas puntos tiene y lo 
 *              declara como ganador 
 *
 * @param [in] p_usuario (float) Puntos totales del usuario.
 * @param [in] p_maquina (float) Puntos totales de la maquina.
 *
 * @retval true Si el jugador es el ganador
 * @retval false Si ha ganado la maquina.
 */ /**************************************************************************/
bool IdentificarGanador(float p_usuario, float p_maquina)
{
    bool ok = true;
    
    if (p_maquina > PT_MAX && p_usuario > PT_MAX)
    {
        if (p_usuario > p_maquina)
            ok = false;
    }
    else if (p_usuario > PT_MAX)
        ok = false;
    else if (p_usuario < p_maquina && p_maquina <= PT_MAX)
        ok = false;
    else if (p_usuario == p_maquina)
        ok = false;
        
    return ok;
}
/******************************DeclararGanador*****************************/ /**
 *
 * @brief Declarar Ganador
 * Proposito: La funcion identifica el ganador de la partida y en funcion de 
 *            si ha ganado o no le suma o resta el dinero de la apuesta. 
 *            A continuacion, muestra el resultado por pantalla.
 *
 * @param [in] p_usuario (float) Los puntos conseguidos por el usuario.
 * @param [in] p_maquina (float) Los puntos conseguidos por la maquina.
 * @param [in] apuesta (int) El dinero que ha apostado el jugador.
 * @param [in] posicion_jugador (int) Posicion del jugador en el vector.
 * @param [out] jugadores_registrados (Jugadores) Vector con datos de jugadores.
 * @param [out] usuario (Jugador) Informacion del jugador. 
 *
 * @retval true Ha ganado el jugador
 * @retval false Ha ganado el usuario
 */ /**************************************************************************/
bool DeclararGanador(Jugador & usuario, float p_usuario, float p_maquina, 
int apuesta, Jugadores jugadores_registrados, int posicion_jugador)
{
    bool ganador;
    
    ganador = IdentificarGanador(p_usuario, p_maquina);
    
    if (ganador)
    {
        usuario.saldo += apuesta;
        cout << "\n" << usuario.nombre << ", has ganado la partida.\n";
    } 
    else
    {
        usuario.saldo -= apuesta;
        cout << "\n" << usuario.nombre << ", has perdido la partida.\n";
    }
    
    jugadores_registrados[posicion_jugador] = usuario;
    
    cout << "Te quedan " << usuario.saldo << " euros para apostar.\n\n";
    
    return ganador;
}
/**********************IniciarDibujoCarta**********************************/ /**
 *
 * @brief IniciarDibujoCarta
 * 
 * Proposito: La funcion guarda los dibujos de las cartas de la baraja española  
 *            en el vector de D_CARTA para utilizarlos despues para el juego.
 *
 * @param [out] d_carta (D_CARTA) Las cartas con sus respectivos didujos.
 * @return void
 *
 */ /**************************************************************************/
void IniciarDibujoCarta (D_CARTA d_carta)
{
    
    for (int i = 1; i < MAX2; i++)
    {
        
        for (int j = 1; j < MAX1; j++)
        {
           
                d_carta[j][i][0] = "+-------+";
            
                switch (i)
                {
                    case 10:
                        d_carta[j][i][1] = "| sota  |";
                        d_carta[j][i][7] = "| sota  |";
                        break;
                    case 11:
                        d_carta[j][i][1] = "|caballo|";   
                        d_carta[j][i][7] = "|caballo|";
                        break;
                    case 12:
                        d_carta[j][i][1] = "|  rey  |";
                        d_carta[j][i][7] = "|  rey  |";
                        break;
                    default:
                        d_carta[j][i][1] = "|" + to_string(i) + 
                        "     " + to_string(i)+ "|";
                        d_carta[j][i][7] = "|" + to_string(i) + 
                        "     " + to_string(i) + "|";
                    break;
                }
            
                switch (j)
                {
                    case 1:
                    
                        d_carta[j][i][2] = "|       |";
                        d_carta[j][i][3] = "|  ***  |";
                        d_carta[j][i][4] = "| *   * |";
                        d_carta[j][i][5] = "| *   * |";
                        d_carta[j][i][6] = "|  ***  |";
                        d_carta[j][i][8] = "+-------+";
                        break;
                    case 2:
                        d_carta[j][i][2] = "|       |";
                        d_carta[j][i][3] = "| *   * |";
                        d_carta[j][i][4] = "|  ***  |";
                        d_carta[j][i][5] = "|   *   |";
                        d_carta[j][i][6] = "| ***** |";
                        d_carta[j][i][8] = "+-------+";
                        break;
                    case 3:
                        d_carta[3][i][2] = "|   *   |";
                        d_carta[3][i][3] = "|  ***  |";
                        d_carta[3][i][4] = "|  ***  |";
                        d_carta[3][i][5] = "|   *   |";
                        d_carta[3][i][6] = "|   *   |";
                        d_carta[3][i][8] = "+-------+";
                        break;
                    case 4:
                        d_carta[4][i][2] = "|   *   |";
                        d_carta[4][i][3] = "|   *   |";
                        d_carta[4][i][4] = "|   *   |";
                        d_carta[4][i][5] = "|  ***  |";
                        d_carta[4][i][6] = "|   *   |";
                        d_carta[4][i][8] = "+-------+";
                        break;
                }
            
        }     
    }
    
    return;
} 

/****************************MostrarCartas*********************************/ /**
 *
 * @brief MostrarCartas
 * 
 * Proposito: La funcion muestra el dibujo de todas las cartas de manera que 
 *              salgan unas al lado de otras. 
 *
 * @param [in] d_carta (D_CARTA) El dibujo que le correpsonde a cada carta.
 * @param [in] jug (Cartas) El valor de la jugada actual.
 * @param [in] t_elem (int) El numero total de cartas a mostrar.
 *
 * @return (void)
 */ /**************************************************************************/
void MostrarCartas( D_CARTA d_carta, Cartas jug, int t_elem)
{
    
    for (int linea = 0; linea < MAX3; linea ++)
    {
        
        for (int orden = 0; orden <= t_elem; orden ++)
        {
            cout <<  d_carta[jug[orden].palo][jug[orden].numero][linea] << " ";
        }
        
        cout << endl;
    } 
    
    return;
}
/***************************LeerDatosJugadores*****************************/ /**
 *
 * @brief  Leer datos jugadores
 * 
 * Proposito: Lee los datos de los jugadores deel fichero de texto con nombre
 *            relacionado con la constante N_IN
 *
 * @param [out] j (Jugadores) Vector con la informacion de los jugadores
 * @param [out] n (int) El total de jugadores.
 *
 * @return (void)
 */ /**************************************************************************/
void LeerDatosJugadores(Jugadores j, int & n)
{
    ifstream f;
    string nombre;
    int dinero;
    
    n = 0;
    f.open(N_IN);
    
    while (f >> nombre)
    {
        
        f >> dinero;
        j[n].nombre = nombre;
        j[n].saldo = dinero;
        n++;
    }
    
    n--;
    
    f.close();
    
    return;
}

/***************************IdentificarJugador*****************************/ /**
 *
 * @brief Identificar a un nuevo Jugador
 * 
 * Proposito: Identifica al jugador dentro de los jugadores registrados y pasa 
 *              su posicion por referencia, si no esta registrado, lo añade al 
 *              vector en otra casilla nueva.
 *
 * @param [in] nk (Jugadores) Vector con los datos de todos los jugadores leidos
 * @param [out] j (Jugador) Informacion del jugador a identificar
 * @param [out] n (int) Numero de jugadores registrados
 * @param [out] pos (int) Posicion del jugador en el vector
 *
 * @return (void)
 */ /**************************************************************************/
void IdentificarJugador(Jugador & j, Jugadores nk, int & n, int & pos)
{
    bool registrado = false;
    
    pos = 0;
    
    for (int i = 0; i <= n; i++)
    {
        if (aMayus(j.nombre) == aMayus(nk[i].nombre))
        {
            pos = i;
            j.saldo = nk[i].saldo;
            if(j.saldo == 0)
                j.saldo = 100;
            registrado = true;
        } 
    }
    
    if (!registrado)
    {
        n++;
        j.saldo = 100;
        nk[n].saldo = j.saldo;
        nk[n].nombre = j.nombre;
        
        pos = n;
    }
    
    return;
}

/***************************GuardarUltimaJugada****************************/ /**
 *
 * @brief Guardar Ultima Jugada
 * 
 * Proposito: Guarda los datos necesarios guardar la ultima jugada en 
 *              un archivo binario.
 *
 * @param [in] jug1 (Cartas) Vector con todas las cartas repartidas al usuario
 * @param [in] jug2 (Cartas) Vector con todas las cartas repartidas a la banca
 * @param [in] t_elem (int)  Total elementos del vector jug1
 * @param [in] dif (int)  Total elementos del vector jug2
 * @param [in] p_m (float) Total puntos de la maquina
 * @param [in] p_u (float) Total puntos del usuario
 * @param [in] ganador (bool) Quien ha ganado la partida
 * @param [in] apuesta (int) El valor de la apuesta hecha
 *
 * @return (void)
 */ /**************************************************************************/
void GuardarUltimaJugada(Cartas jug1, Cartas jug2,
 int t_elem, int dif, float p_m, float p_u, bool ganador, int apuesta)
{
    ofstream f;
    
    f.open(N_OUT, ios::binary);
    
    
    f.write((char *) (&t_elem), sizeof (t_elem));
    f.write((char *) (&dif), sizeof (dif));
    f.write((char *) (&p_m), sizeof (p_m));
    f.write((char *) (&p_u), sizeof (p_u));
    f.write((char *) (&apuesta), sizeof (apuesta));
    f.write((char *) (&ganador), sizeof (ganador));
    
    
    for (int i = 0; i <= dif; i++)
        f.write((char *) (&jug1[i]), sizeof (jug1[i]));
        
    for (int i = 0; i <= t_elem; i++)
        f.write((char *) (&jug2[i]), sizeof (jug2[i]));
        
        
    f.close();
    
    return;
}

/***************************MostrarUltimaJugada****************************/ /**
 *
 * @brief MostrarUltimaJugada
 * 
 * Proposito: Muestra la ultima jugada la cual es leida de un fichero binario.
 *
 * @param [in] (void)
 * 
 * @return (void)
 */ /**************************************************************************/
void MostrarUltimaJugada(void)
{
    ifstream f;
    int t_elem;
    int dif;
    int apuesta;
    float p_u, p_m;
    bool ganador;
    Cartas jug1, jug2;
    D_CARTA c;
    
    IniciarDibujoCarta(c);
    
    f.open(N_OUT, ios::binary);
    
    f.read((char *) (&t_elem), sizeof (t_elem));
    f.read((char *) (&dif), sizeof (dif));
    f.read((char *) (&p_m), sizeof (p_m));
    f.read((char *) (&p_u), sizeof (p_u));
    f.read((char *) (&apuesta), sizeof (apuesta));
    f.read((char *) (&ganador), sizeof (ganador));
    
    for (int i = 0; i <= dif; i++)
        f.read((char *) (&jug1[i]), sizeof (jug1[i]));
        
    for (int i = 0; i <= t_elem; i++)
        f.read((char *) (&jug2[i]), sizeof (jug2[i]));
    
    cout << "Cartas del usuario:\n";
    MostrarCartas(c, jug1, dif);
    
    cout << "Puntos: " << p_u;
    
    cout << "\n\nCartas del ordenador:\n";
    MostrarCartas(c, jug2, t_elem);
    
    cout << "Puntos: " << p_m << endl;
    cout << endl << "Apuesta: " << apuesta << " euros "<< endl << endl;
    
    
    if (ganador)
        cout << "Gana el jugador. Pierde la banca\n";
    else
        cout << "Pierde el jugador. Gana la banca\n";
    
    f.close();
    
    return;
}
/****************************ApuestaEnRango********************************/ /**
 *
 * @brief Comprobar Apuesta En Rango
 * 
 * Proposito: Comprobar si el valor introducido para la apuesta es valido.
 *
 * @param [in] apuesta (int) Apuesta introducida por el usuario
 * @param [in] saldo (int) Saldo del usuario
 *
 * @retval true Si la apuesta introducida es valida.
 * @retval false Si no es valida la apuesta introducida.
 */ /**************************************************************************/
bool ApuestaEnRango(int apuesta, int saldo)
{
    bool ok = true;
    
    if (apuesta <= 0)
        ok = false;
        
    if (apuesta > saldo)
        ok = false;
    
    return ok;
}
/***************************EscribirDatosJugadores*************************/ /**
 *
 * @brief EscribirDatosJugadores
 * 
 * Proposito: La funcion escribe los datos de los jugadores en el fichero 
 *            con nombre relacionado con la constante n_in
 *
 * @param [in] j (Jugadores) La informacion de los jugadores.
 * @param [in] n (int) El total de jugadores
 *
 * @return (void)
 */ /**************************************************************************/
void EscribirDatosJugadores(Jugadores j, int n)
{
    ofstream f;   
    
    f.open(N_IN);
   
    for (int i = 0; i <= n; i++)
    {
        if (j[i].saldo != 0)
            f << aMayus(j[i].nombre) << " " << j[i].saldo << endl;
    }
    
    f.close();
    
    return;
}
/***************************EliminarJugador*****************************/ /**
 *
 * @brief Comprobar matricula
 * 
 * Proposito: Elimina el jugador eliminado del vector, una vez que se ha quedado
 *            sin credito jugando.
 *
 *
 * @param [in] n_reg (int) Numero de jugadores registrados 
 * @param [in] j (Jugadores) La informacion del registro de jugadores
 * @param [in] pos (int) Posicion del jugador
 * @param [out] n_reg (int) Numero de jugadores registrados
 *
 * @return (void)
 */ /**************************************************************************/
void EliminarJugador(Jugadores j, int pos, int & n_reg)
{
    Jugadores aux;
    
    for (int i = 0; i <= (n_reg - pos); i++)
        aux[i] = j[pos + i];
    for (int i = 0; i < (n_reg - pos); i++)
        j[pos + i] = aux[i + 1];
    n_reg--;
    
    return;
}

/*******************************IniciarBaraja*****************************/ /**
 *
 * @brief Iniciar Baraja
 * 
 * Proposito: La funcion inicia la baraja espanyola antes de empezar el juego.
 *
 * @param [out] c (Cartas) Vector donde se guardan las cartas de la baraja esp
 *
 * @return (void)
 */ /**************************************************************************/
void IniciarBaraja (Cartas c)
{
    int ronda = 0;
    
    for (int i = 1; i < MAX1; i++)
    {
        for (int j = 1; j < MAX2; j++)
        {
            if (j != 8 && j != 9)
            {
                c[ronda].numero = j;
                c[ronda].palo = i;
                ronda++;
            }  
        }
    }
    
    return;
}

/*******************************Barajar************************************/ /**
 *
 * @brief Barajar las cartas
 * 
 * Proposito: La funcion baraja las cartas antes de repartirlas a los jugadores.
 *
 * @param [in] carta (Cartas) Cartas aleatorias de la baraja sin barajar.
 * @param [in] baraja (Cartas) Todas las cartas de la baraja sin barajar.
 *
 * @return (void)
 */ /**************************************************************************/
void Barajar (Cartas carta, Cartas baraja)
{
    Numeros_Jugados nj;
    
    
    srand(time(NULL));
    
    for (int i = 0; i <= MAX_C; i++)
    {
        nj[i] = GenerarNumero();
        carta[i] = baraja[nj[i]];
        if (CartaRepetida(nj, nj[i], i) && i != 0)
            i--;
    }
    
    return;    
}

/*****************************PedirApuesta*********************************/ /**
 *
 * @brief Pedir Apuesta
 * 
 * Proposito: La funcion pide al usuario el valor de la apuesta que quiere ha   
 *
 * @param [in] usuario (Jugador) La informacion del jugador.
 * @param [out] apuesta (int) El valor de la apuesta hecha por el jugador.
 *
 * @return (void)
 */ /**************************************************************************/
void PedirApuesta(int & apuesta, Jugador usuario)
{
    bool ok;
    
    do 
    {
        cout << "Dame apuesta (maximo " << usuario.saldo << " euros): ";
        cin >> apuesta; 
        ok = ApuestaEnRango(apuesta, usuario.saldo);
        
        if (apuesta <= 0)
            cout << "El valor debe ser mayor que cero\n";
        else if (apuesta > usuario.saldo)
            cout << "No tienes suficiente dinero\n";
            
    }
    while (!ok);
    
    return;
}

/***************************ComprobarEliminado*****************************/ /**
 *
 * @brief Comprobar Eliminado
 * 
 * Proposito: La funcion comprueba si el jugador a sido eliminado.Y lo elimina 
 *              del registro. En el caso de ser asi le indica que se 
 *              identifique antes de nada.
 *
 * @param [in] jugadores_registrados (Jugadores) Registro de los jugadores
 * @param [in] usuario (Jugador) Informacion del jugador
 * @param [in] posicion_jugador (int) La posicion del jugador en el registro.
 * @param [in] num_registros (int) El total de juagdores registrados.
 *
 * @return (void)
 */ /**************************************************************************/
void ComprobarEliminado(Jugadores jugadores_registrados, 
Jugador & usuario, int posicion_jugador, int num_registros)
{
    if (usuario.saldo == 0)
    {
        cout << usuario.nombre << " has sido eliminado, ";
        cout << "vuelve a identificarte para volver a empezar con 100 euros.\n";
        EliminarJugador(jugadores_registrados, posicion_jugador, num_registros);
        usuario.nombre = "";
        usuario.saldo = 100;
    }
    
    return;
}
string aMayus(string s)
{
    string res;
    res = s;
    for (unsigned i = 0; i < s.length(); i++)
    {
        res[i] = std::toupper(s[i]);
    }
    return res;
}
