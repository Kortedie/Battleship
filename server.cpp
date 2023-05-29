#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

const int boardSize = 15;
const int numShips = 8;
const int maxAttempts = 50;

const int aircraftCarrierSize = 5;
const int battleshipSize = 4;
const int submarineSize = 3;
const int boatSize = 1;

void printBoard(const int board[boardSize][boardSize]) {
    // Obtener la cantidad de dígitos en el número más grande del tablero
    int maxNum = boardSize - 1;
    int maxNumDigits = 1;
    while (maxNum >= 10) {
        maxNum /= 10;
        maxNumDigits++;
    }

    // Mostrar las coordenadas del eje x por encima del tablero
    cout << setw(maxNumDigits + 2) << " ";
    for (int j = 0; j < boardSize; j++) {
        cout << setw(maxNumDigits + 1) << j << " ";  // Ajustar el ancho del campo según la cantidad de dígitos
    }
    cout << endl;

    for (int i = 0; i < boardSize; i++) {
        // Mostrar las coordenadas del eje y a la izquierda del tablero
        cout << setw(maxNumDigits + 1) << i << " ";  // Ajustar el ancho del campo según la cantidad de dígitos

        for (int j = 0; j < boardSize; j++) {
            if (board[i][j] == 1) {
                cout << "  P ";  // Portaaviones
            } else if (board[i][j] == 2) {
                cout << "  B ";  // Buque
            } else if (board[i][j] == 3) {
                cout << "  S ";  // Submarino
            } else if (board[i][j] == 4){
                cout << "  L ";  // Lancha
            } else if (board[i][j] == 'A') {
                cout << "  A ";
            } else if (board[i][j] == 'X') {
                cout << "  X ";
            } else {
                cout << setw(maxNumDigits + 1) << board[i][j] << " ";  // Ajustar el ancho del campo según la cantidad de dígitos
            }
        }
        cout << endl;
    }
}

void playerTurn(int board[boardSize][boardSize]) {
    cout << "Jugador, es tu turno." << endl;
    cout << "Ingresa las coordenadas (fila y columna) para adivinar (rango 0-" << boardSize - 1 << "): ";
    int row, col;
    cin >> row >> col;

    // Verificar si las coordenadas están dentro del rango válido
    if (row < 0 || row >= boardSize || col < 0 || col >= boardSize) {
        cout << "Coordenadas inválidas. Por favor, ingresa valores dentro del rango 0-" << boardSize - 1 << "." << endl;
        return;  // Salir de la función sin hacer cambios en el tablero
    }

    if (board[row][col] == 1) {
        cout << "¡Acertaste! Hundiste un portaaviones." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 2) {
        cout << "¡Acertaste! Hundiste un buque." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 3) {
        cout << "¡Acertaste! Hundiste un submarino." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 4) {
        cout << "¡Acertaste! Hundiste una lancha." << endl;
        board[row][col] = 'A';
    } else {
        cout << "¡Fallaste! No hay ningún barco en esa posición." << endl;
        board[row][col] = 'X';  // Cambiar el carácter en la coordenada a "X"
    }
}

void botTurn(int board[boardSize][boardSize]) {
    cout << "Turno del bot." << endl;
    int row = rand() % boardSize;
    int col = rand() % boardSize;

    if (board[row][col] == 1) {
        cout << "El bot ha acertado y ha hundido un portaaviones." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 2) {
        cout << "El bot ha acertado y ha hundido un buque." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 3) {
        cout << "El bot ha acertado y ha hundido un submarino." << endl;
        board[row][col] = 'A';
    } else if (board[row][col] == 4) {
        cout << "El bot ha acertado y ha hundido una lancha." << endl;
        board[row][col] = 'A';
    } else {
        cout << "El bot ha fallado su intento." << endl;
        board[row][col] = 'X';
    }
}

bool checkGameOver(const int board[boardSize][boardSize]) {
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (board[i][j] == 1 || board[i][j] == 2) {
                return false;  // Hay al menos un barco que no ha sido hundido
            }
        }
    }
    return true;  // Todos los barcos han sido hundidos
}

// Estructura para almacenar los datos del cliente
struct ClientData {
    int clientSocket;
    string playerName;
    bool isTurn;
};

vector<ClientData> clients;  // Vector para almacenar los datos de los clientes

// Función para manejar a un cliente
void* HandleClient(void* arg) {
    int clientSocket = *((int*)arg);
    char buffer[1024];
    string message;

    // Crear una nueva estructura ClientData para el cliente
    ClientData clientData;
    clientData.clientSocket = clientSocket;
    clientData.isTurn = false;

    // Leer el nombre del jugador desde el cliente
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        cerr << "Error al leer el nombre del jugador" << endl;
        close(clientSocket);
        return NULL;
    }
    clientData.playerName = buffer;

    // Añadir al cliente al vector de clientes
    clients.push_back(clientData);

    cout << "Se ha unido un nuevo jugador: " << clientData.playerName << endl;

    // Enviar un mensaje de bienvenida al cliente
    string welcomeMessage = "Bienvenido al juego de batalla naval, " + clientData.playerName + "!";
    write(clientSocket, welcomeMessage.c_str(), welcomeMessage.length());

    // Si solo hay un cliente conectado, se establece su turno como verdadero
    if (clients.size() == 1) {
        clients[0].isTurn = true;
        int playerBoard[boardSize][boardSize];
        int botBoard[boardSize][boardSize];

        srand(time(NULL));

            // Inicializar los tableros
            for (int i = 0; i < boardSize; i++) {
                for (int j = 0; j < boardSize; j++) {
                    playerBoard[i][j] = 0;
                    botBoard[i][j] = 0;
                }
            }

            // Posicionar el portaaviones del jugador
            cout << "Posiciona tu portaaviones." << endl;
            cout << "Ingresa las coordenadas (fila y columna) para posicionar el portaaviones (rango 0-" << boardSize - 1 << "): ";
            int row, col;
            cin >> row >> col;

            cout << "Ingresa la orientación del portaaviones (0 para horizontal, 1 para vertical): ";
            int orientation;
            cin >> orientation;

            // Verificar si las coordenadas y la orientación son válidas
            if (row < 0 || row >= boardSize || col < 0 || col >= boardSize ||
                (orientation != 0 && orientation != 1)) {
                cout << "Coordenadas u orientación inválidas. Por favor, ingresa valores dentro del rango y la orientación correcta." << endl;
                return 0;
            }

            // Verificar si hay suficiente espacio para colocar el portaaviones en la orientación seleccionada
            if (orientation == 0 && col + aircraftCarrierSize - 1 >= boardSize) {
                cout << "No hay suficiente espacio horizontal para colocar el portaaviones en esa posición." << endl;
                return 0;
            } else if (orientation == 1 && row + aircraftCarrierSize - 1 >= boardSize) {
                cout << "No hay suficiente espacio vertical para colocar el portaaviones en esa posición." << endl;
                return 0;
            }

            // Posicionar el portaaviones del jugador según la orientación seleccionada
            if (orientation == 0) {
                for (int j = col; j < col + aircraftCarrierSize; j++) {
                    playerBoard[row][j] = 1;  // Usar el número 1 para representar el portaaviones en el tablero del jugador
                }
            } else {
                for (int i = row; i < row + aircraftCarrierSize; i++) {
                    playerBoard[i][col] = 1;  // Usar el número 1 para representar el portaaviones en el tablero del jugador
                }
            }

            // Posicionar los buques del jugador
            for (int ship = 0; ship < 2; ship++) {
                cout << "Posiciona tu buque " << ship + 1 << "." << endl;
                cout << "Ingresa las coordenadas (fila y columna) para posicionar el buque (rango 0-" << boardSize - 1 << "): ";
                cin >> row >> col;

                cout << "Ingresa la orientación del buque (0 para horizontal, 1 para vertical): ";
                cin >> orientation;

                // Verificar si las coordenadas y la orientación son válidas
                if (row < 0 || row >= boardSize || col < 0 || col >= boardSize ||
                    (orientation != 0 && orientation != 1)) {
                    cout << "Coordenadas u orientación inválidas. Por favor, ingresa valores dentro del rango y la orientación correcta." << endl;
                    return 0;
                }

                // Verificar si hay suficiente espacio para colocar el buque en la orientación seleccionada
                if (orientation == 0 && col + battleshipSize - 1 >= boardSize) {
                    cout << "No hay suficiente espacio horizontal para colocar el buque en esa posición." << endl;
                    return 0;
                } else if (orientation == 1 && row + battleshipSize - 1 >= boardSize) {
                    cout << "No hay suficiente espacio vertical para colocar el buque en esa posición." << endl;
                    return 0;
                }

                // Verificar si la posición seleccionada se superpone con el portaaviones
                if (orientation == 0) {
                    for (int j = col; j < col + battleshipSize; j++) {
                        if (playerBoard[row][j] == 1) {
                            cout << "La posición seleccionada se superpone con el portaaviones." << endl;
                            return 0;
                        }
                    }
                } else {
                    for (int i = row; i < row + battleshipSize; i++) {
                        if (playerBoard[i][col] == 1) {
                            cout << "La posición seleccionada se superpone con el portaaviones." << endl;
                            return 0;
                        }
                    }
                }

                // Posicionar el buque según la orientación seleccionada
                if (orientation == 0) {
                    for (int j = col; j < col + battleshipSize; j++) {
                        playerBoard[row][j] = 2;  // Usar el número 2 para representar el buque en el tablero del jugador
                    }
                } else {
                    for (int i = row; i < row + battleshipSize; i++) {
                        playerBoard[i][col] = 2;  // Usar el número 2 para representar el buque en el tablero del jugador
                    }
                }
            }
        //Submarino
        for (int sub = 0; sub < 2; sub++) {
            cout << "Posiciona tu submarino " << sub + 1 << "." << endl;
            cout << "Ingresa las coordenadas (fila y columna) para posicionar el submarino (rango 0-" << boardSize - 1 << "): ";
            cin >> row >> col;

            cout << "Ingresa la orientación del submarino (0 para horizontal, 1 para vertical): ";
            cin >> orientation;

            // Verificar si las coordenadas y la orientación son válidas
            if (row < 0 || row >= boardSize || col < 0 || col >= boardSize ||
                (orientation != 0 && orientation != 1)) {
                cout << "Coordenadas u orientación inválidas. Por favor, ingresa valores dentro del rango y la orientación correcta." << endl;
                return 0;
            }

            // Verificar si hay suficiente espacio para colocar el submarino en la orientación seleccionada
            if (orientation == 0 && col + submarineSize - 1 >= boardSize) {
                cout << "No hay suficiente espacio horizontal para colocar el submarino en esa posición." << endl;
                return 0;
            } else if (orientation == 1 && row + submarineSize - 1 >= boardSize) {
                cout << "No hay suficiente espacio vertical para colocar el submarino en esa posición." << endl;
                return 0;
            }

            // Verificar si la posición seleccionada se superpone con el portaaviones o los buques
            if (orientation == 0) {
                for (int j = col; j < col + submarineSize; j++) {
                    if (playerBoard[row][j] == 1 || playerBoard[row][j] == 2) {
                        cout << "La posición seleccionada se superpone con un barco existente." << endl;
                        return 0;
                    }
                }
            } else {
                for (int i = row; i < row + submarineSize; i++) {
                    if (playerBoard[i][col] == 1 || playerBoard[i][col] == 2) {
                        cout << "La posición seleccionada se superpone con un barco existente." << endl;
                        return 0;
                    }
                }
            }

            // Colocar el submarino en el tablero del jugador
            if (orientation == 0) {
                for (int j = col; j < col + submarineSize; j++) {
                    playerBoard[row][j] = 3;  // Submarino
                }
            } else {
                for (int i = row; i < row + submarineSize; i++) {
                    playerBoard[i][col] = 3;  // Submarino
                }
            }
        }
        // Lancha
        for (int lan = 0; lan < 3; lan++) {
            cout << "Posiciona tu lancha " << lan + 1 << "." << endl;
            cout << "Ingresa las coordenadas (fila y columna) para posicionar la lancha (rango 0-" << boardSize - 1 << "): ";
            cin >> row >> col;

            cout << "Ingresa la orientación de la lancha (0 para horizontal, 1 para vertical): ";
            cin >> orientation;

            // Verificar si las coordenadas y la orientación son válidas
            if (row < 0 || row >= boardSize || col < 0 || col >= boardSize ||
                (orientation != 0 && orientation != 1)) {
                cout << "Coordenadas u orientación inválidas. Por favor, ingresa valores dentro del rango y la orientación correcta." << endl;
                return 0;
            }

            // Verificar si la posición seleccionada se superpone con el portaaviones o los buques o submarinos
            if (orientation == 0) {
                for (int j = col; j < col + boatSize; j++) {
                    if (playerBoard[row][j] == 1 || playerBoard[row][j] == 2) {
                        cout << "La posición seleccionada se superpone con un barco existente." << endl;
                        return 0;
                    }
                }
            } else {
                for (int i = row; i < row + boatSize; i++) {
                    if (playerBoard[i][col] == 1 || playerBoard[i][col] == 2) {
                        cout << "La posición seleccionada se superpone con un barco existente." << endl;
                        return 0;
                    }
                }
            }

            // Colocar el submarino en el tablero del jugador
            if (orientation == 0) {
                for (int j = col; j < col + boatSize; j++) {
                    playerBoard[row][j] = 4;  // Submarino
                }
            } else {
                for (int i = row; i < row + boatSize; i++) {
                    playerBoard[i][col] = 4;  // Submarino
                }
            }
        }


        // Posicionar los barcos del bot de forma aleatoria
        for (int ship = 0; ship < numShips; ship++) {
            bool positionSet = false;
            int attempts = 0;

            while (!positionSet && attempts < maxAttempts) {
                row = rand() % boardSize;
                col = rand() % boardSize;

                orientation = rand() % 2;

                // Verificar si hay suficiente espacio para colocar el barco en la orientación seleccionada
                if (orientation == 0 && col + battleshipSize - 1 < boardSize) {
                    bool validPosition = true;
                    for (int j = col; j < col + battleshipSize; j++) {
                        if (botBoard[row][j] != 0) {
                            validPosition = false;
                            break;
                        }
                    }
                    if (validPosition) {
                        for (int j = col; j < col + battleshipSize; j++) {
                            botBoard[row][j] = ship + 1;  // Usar el número ship + 1 para representar el barco en el tablero del bot
                        }
                        positionSet = true;
                    }
                } else if (orientation == 1 && row + battleshipSize - 1 < boardSize) {
                    bool validPosition = true;
                    for (int i = row; i < row + battleshipSize; i++) {
                        if (botBoard[i][col] != 0) {
                            validPosition = false;
                            break;
                        }
                    }
                    if (validPosition) {
                        for (int i = row; i < row + battleshipSize; i++) {
                            botBoard[i][col] = ship + 1;  // Usar el número ship + 1 para representar el barco en el tablero del bot
                        }
                        positionSet = true;
                    }
                }

                attempts++;
            }
        }


            // Juego principal
            bool gameOver = false;
            while (!gameOver) {
                cout << endl << "Tablero del jugador:" << endl;
                printBoard(playerBoard);

                cout << endl << "Tablero del bot:" << endl;
                printBoard(botBoard);

                playerTurn(botBoard);
                gameOver = checkGameOver(botBoard);

                if (!gameOver) {
                    botTurn(playerBoard);
                    gameOver = checkGameOver(playerBoard);
                }
            }

            // Mostrar los tableros finales
            cout << endl << "Tablero final del jugador:" << endl;
            printBoard(playerBoard);

            cout << endl << "Tablero final del bot:" << endl;
            printBoard(botBoard);

            // Determinar el ganador
            if (checkGameOver(playerBoard)) {
                cout << "¡Felicidades! ¡Has ganado el juego!" << endl;
            } else {
                cout << "El bot ha ganado el juego. ¡Mejor suerte la próxima vez!" << endl;
            }

            return 0;
    
    }

    while (true) {
        // Limpiar el buffer
        memset(buffer, 0, sizeof(buffer));

        // Leer el disparo del cliente
        bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            cerr << "Error al leer el disparo del cliente: " << clientData.playerName << endl;
            break;
        }
        message = buffer;

        // Verificar si es el turno del cliente
        if (clientData.isTurn) {
            cout << "El jugador " << clientData.playerName << " realizó un disparo: " << message << endl;

            // Realizar la lógica del juego y enviar una respuesta al cliente
            // Aquí debes implementar la lógica específica del juego de batalla naval

            // Cambiar el turno del cliente al siguiente jugador
            int currentPlayerIndex = -1;
            for (size_t i = 0; i < clients.size(); i++) {
                if (clients[i].clientSocket == clientSocket) {
                    currentPlayerIndex = i;
                    break;
                }
            }

            if (currentPlayerIndex != -1) {
                clients[currentPlayerIndex].isTurn = false;

                size_t nextPlayerIndex = (currentPlayerIndex + 1) % clients.size();
                clients[nextPlayerIndex].isTurn = true;
            }
        } else {
            // El cliente intentó realizar un disparo cuando no era su turno
            string errorMessage = "No es tu turno, espera a que sea tu turno.";
            write(clientSocket, errorMessage.c_str(), errorMessage.length());
        }
    }

    // Eliminar al cliente del vector de clientes
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->clientSocket == clientSocket) {
            cout << "El jugador " << it->playerName << " se ha desconectado." << endl;
            clients.erase(it);
            break;
        }
    }

    // Cerrar el socket del cliente
    close(clientSocket);

    return NULL;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    pthread_t threadId;

    // Crear el socket del servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Vincular el socket a la dirección del servidor
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error al vincular el socket" << endl;
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(serverSocket, 5) < 0) {
        cerr << "Error al escuchar las conexiones entrantes" << endl;
        return 1;
    }

    cout << "Servidor en espera de conexiones..." << endl;

    while (true) {
        // Aceptar una conexión entrante
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            cerr << "Error al aceptar la conexión entrante" << endl;
            return 1;
        }

        // Crear un nuevo hilo para manejar al cliente
        if (pthread_create(&threadId, NULL, HandleClient, (void*)&clientSocket) < 0) {
            cerr << "Error al crear un nuevo hilo" << endl;
            return 1;
        }
    }

    // Cerrar el socket del servidor
    close(serverSocket);

    return 0;
}
