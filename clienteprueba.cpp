#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Debe proporcionar la dirección IP del servidor y el puerto" << endl;
        return 1;
    }

    string serverIP = argv[1];
    int port = stoi(argv[2]);
    int clientSocket;
    struct sockaddr_in serverAddress;

    // Crear el socket del cliente
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Error al crear el socket" << endl;
        return 1;
    }

    // Configurar la dirección del servidor
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(serverIP.c_str());

    // Conectar al servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error al conectar al servidor" << endl;
        return 1;
    }

    // Comunicarse con el servidor
    char buffer[1024];
    string message;
    bool playerTurn = true;
    int aux = 0;

    while (true) {
        // Limpiar el buffer
        memset(buffer, 0, sizeof(buffer));

        // Verificar si es el turno del jugador
        if (playerTurn) {
            // Leer la entrada del jugador desde la consola
            if (aux == 1){
                cout << "Ingrese una coordenada para disparar (por ejemplo, A1): ";
                cin >> message;
            } else {
                cout << "Ingrese nombre de jugador: ";
                cin >> message;
                aux++;
            }

            // Enviar el disparo al servidor
            write(clientSocket, message.c_str(), message.length());

            // Cambiar el turno del jugador
            playerTurn = false;
        }

        // Leer la respuesta del servidor
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            cerr << "Error al leer la respuesta del servidor" << endl;
            break;
        }

        // Mostrar la respuesta del servidor
        cout << "Respuesta del servidor: " << buffer << endl;

        // Si el juego ha terminado, finalizar el bucle
        if (string(buffer) == "Game Over - Ganaste") {
            break;
        }

        // Cambiar el turno del jugador si el servidor no envió "Disparo inválido"
        if (string(buffer) != "Disparo inválido") {
            playerTurn = true;
        }
    }

    // Cerrar el socket del cliente
    close(clientSocket);

    return 0;
}


