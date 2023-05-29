#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <thread>

#define BUFFER_SIZE 1024

using namespace std;

void initializeBoard(char *boardData) {
    memset(boardData, '.', 15 * 15);
}

void placeShip(char *boardData, int size, char shipSymbol) {
    int randomRow, randomCol;
    bool isValid = false;
    while (!isValid) {
        randomRow = rand() % 15;
        randomCol = rand() % 15;

        bool isHorizontal = rand() % 2 == 0;
        if (isHorizontal) {
            if (randomCol + size <= 15) {
                isValid = true;
                for (int i = 0; i < size; ++i) {
                    if (boardData[randomRow * 15 + randomCol + i] != '.') {
                        isValid = false;
                        break;
                    }
                }
                if (isValid) {
                    for (int i = 0; i < size; ++i) {
                        boardData[randomRow * 15 + randomCol + i] = shipSymbol;
                    }
                }
            }
        } else {
            if (randomRow + size <= 15) {
                isValid = true;
                for (int i = 0; i < size; ++i) {
                    if (boardData[(randomRow + i) * 15 + randomCol] != '.') {
                        isValid = false;
                        break;
                    }
                }
                if (isValid) {
                    for (int i = 0; i < size; ++i) {
                        boardData[(randomRow + i) * 15 + randomCol] = shipSymbol;
                    }
                }
            }
        }
    }
}

bool isGameOver(const char *boardData) {
    int shipCount = 0;
    for (int i = 0; i < 15 * 15; ++i) {
        if (boardData[i] != '.' && boardData[i] != 'X' && boardData[i] != 'O') {
            ++shipCount;
        }
    }
    return shipCount == 0;
}

void sendBoard(int clientSocket, const char *boardData) {
    send(clientSocket, boardData, 15 * 15, 0);
}

void sendStartingPlayer(int clientSocket, char startingPlayer) {
    send(clientSocket, &startingPlayer, 1, 0);
}

void sendShotResult(int clientSocket, char result) {
    send(clientSocket, &result, 1, 0);
}

void handleClient(int clientSocket, char *boardData, const std::vector<int> &clientSockets, const std::vector<char> &playerSymbols) {
    // Definir el símbolo del jugador
    char playerSymbol = 'A' + clientSockets.size() - 1;

    std::cout << "¡Nuevo jugador conectado! Símbolo del jugador: " << playerSymbol << std::endl;

    // Enviar el tablero inicial al nuevo jugador
    sendBoard(clientSocket, boardData);

    // Seleccionar al azar quién comienza
    char startingPlayer = playerSymbols[rand() % playerSymbols.size()];
    sendStartingPlayer(clientSocket, startingPlayer);

    std::cout << "¡Comienza el jugador " << startingPlayer << "!" << std::endl;

    // Bucle para jugar el juego
    bool gameRunning = true;
    while (gameRunning) {
        // Recibir disparo del cliente
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            std::cout << "El jugador " << playerSymbol << " ha desconectado." << std::endl;
            gameRunning = false;
            break;
        }

        int row = buffer[0] - '0';
        int col = buffer[2] - '0';

        // Verificar el resultado del disparo
        char result;
        if (boardData[row * 15 + col] != '.') {
            result = 'X';
            boardData[row * 15 + col] = 'X';
        } else {
            result = 'O';
        }

        // Enviar el resultado del disparo al cliente
        sendShotResult(clientSocket, result);

        // Verificar si el juego ha terminado
        if (isGameOver(boardData)) {
            std::cout << "¡El jugador " << playerSymbol << " ha ganado!" << std::endl;
            gameRunning = false;
            break;
        }

        // Enviar el tablero actualizado a todos los jugadores
        for (int socket : clientSockets) {
            sendBoard(socket, boardData);
        }
    }

    // Cerrar la conexión con el cliente y eliminarlo de los vectores
    close(clientSocket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <puerto del servidor>" << std::endl;
        return 1;
    }

    int serverPort = std::stoi(argv[1]);

    // Crear el socket del servidor
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    // Vincular el socket a la dirección del servidor
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error al vincular el socket" << std::endl;
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Error al escuchar conexiones entrantes" << std::endl;
        return 1;
    }

    std::vector<int> clientSockets;
    std::vector<char> playerSymbols;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Crear el tablero y posicionar los barcos
    char boardData[15 * 15];
    initializeBoard(boardData);
    placeShip(boardData, 5, 'P');  // Portaaviones
    placeShip(boardData, 4, 'B');  // Buque
    placeShip(boardData, 4, 'B');  // Buque
    placeShip(boardData, 3, 'S');  // Submarino
    placeShip(boardData, 3, 'S');  // Submarino
    placeShip(boardData, 1, 'L');  // Lancha
    placeShip(boardData, 1, 'L');  // Lancha
    placeShip(boardData, 1, 'L');  // Lancha

    // Ciclo principal para aceptar conexiones y jugar el juego
    while (true) {
        std::cout << "Esperando jugadores..." << std::endl;

        // Aceptar nueva conexión de un cliente
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            std::cerr << "Error al aceptar la conexión del cliente" << std::endl;
            continue;
        }

        // Agregar el nuevo cliente al vector de sockets
        clientSockets.push_back(clientSocket);

        // Definir el símbolo del jugador
        char playerSymbol = 'A' + clientSockets.size() - 1;
        playerSymbols.push_back(playerSymbol);

        std::cout << "¡Nuevo jugador conectado! Símbolo del jugador: " << playerSymbol << std::endl;

        // Enviar el tablero inicial al nuevo jugador
        sendBoard(clientSocket, boardData);

        // Seleccionar al azar quién comienza
        char startingPlayer = playerSymbols[rand() % playerSymbols.size()];
        sendStartingPlayer(clientSocket, startingPlayer);

        std::cout << "¡Comienza el jugador " << startingPlayer << "!" << std::endl;

        // Crear un hilo para manejar la conexión del cliente
        std::thread clientThread(handleClient, clientSocket, playerSymbol, boardData, clientSockets);

        // Desconectar el hilo del proceso principal
        clientThread.detach();
    }

    // Cerrar el socket del servidor
    close(serverSocket);

    return 0;
}
