#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

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

