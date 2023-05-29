#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 1024

using namespace std;

void printBoard(const char *boardData) {
    cout << "  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14" << endl;
    for (int row = 0; row < 15; ++row) {
        cout << row << " ";
        for (int col = 0; col < 15; ++col) {
            cout << boardData[row * 15 + col] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Uso: " << argv[0] << " <dirección IP del servidor> <puerto del servidor>" << endl;
        return 1;
    }

    const char *serverIP = argv[1];
    int serverPort = stoi(argv[2]);

    // Crear el socket del cliente
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr)) <= 0) {
        cerr << "Dirección IP inválida o no soportada" << endl;
        return 1;
    }

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error al conectar al servidor" << endl;
        return 1;
    }

    // Recibir el tablero inicial del servidor
    char boardData[15 * 15];
    memset(boardData, 0, 15 * 15);
    int bytesRead = recv(clientSocket, boardData, 15 * 15, 0);
    if (bytesRead <= 0) {
        cerr << "Error al recibir el tablero inicial del servidor" << endl;
        return 1;
    }

    // Visualizar el tablero inicial
    printBoard(boardData);

    // Recibir el símbolo del jugador
    char playerSymbol;
    bytesRead = recv(clientSocket, &playerSymbol, 1, 0);
    if (bytesRead <= 0) {
        cerr << "Error al recibir el símbolo del jugador" << endl;
        return 1;
    }

    cout << "¡Eres el jugador " << playerSymbol << "!" << endl;

    // Bucle para jugar el juego
    bool gameRunning = true;
    while (gameRunning) {
        // Solicitar al jugador que ingrese las coordenadas del disparo
        int row, col;
        cout << "Ingresa las coordenadas del disparo (fila columna): ";
        cin >> row >> col;

        // Enviar las coordenadas del disparo al servidor
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        buffer[0] = row + '0';
        buffer[1] = ' ';
        buffer[2] = col + '0';
        send(clientSocket, buffer, strlen(buffer), 0);

        // Recibir el resultado del disparo del servidor
        char result;
        bytesRead = recv(clientSocket, &result, 1, 0);
        if (bytesRead <= 0) {
            cerr << "Error al recibir el resultado del disparo" << endl;
            return 1;
        }

        // Mostrar el resultado del disparo
        if (result == 'X') {
            cout << "¡Has impactado un barco enemigo!" << endl;
        } else if (result == 'O') {
            cout << "¡Has fallado el disparo!" << endl;
        }

        // Recibir el tablero actualizado del servidor
        memset(boardData, 0, 15 * 15);
        bytesRead = recv(clientSocket, boardData, 15 * 15, 0);
        if (bytesRead <= 0) {
            cerr << "Error al recibir el tablero actualizado" << endl;
            return 1;
        }

        // Visualizar el tablero actualizado
        printBoard(boardData);

        // Verificar si el juego ha terminado
        for (int i = 0; i < 15 * 15; ++i) {
            if (boardData[i] != '.' && boardData[i] != 'X' && boardData[i] != 'O') {
                gameRunning = true;
                break;
            }
            gameRunning = false;
        }
    }

    // Cerrar la conexión con el servidor
    close(clientSocket);

    return 0;
}


