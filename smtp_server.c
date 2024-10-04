#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 25 // Porta SMTP padrão
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // Mensagem de boas-vindas do servidor SMTP
    char *welcome_message = "220 Simple SMTP Server\r\n";
    send(client_socket, welcome_message, strlen(welcome_message), 0);

    // Loop para receber comandos do cliente
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Comando recebido: %s", buffer);

        // Verifica comando HELO
        if (strncmp(buffer, "HELO", 4) == 0) {
            char *helo_response = "250 Hello\r\n";
            send(client_socket, helo_response, strlen(helo_response), 0);
        }
        // Verifica comando MAIL FROM
        else if (strncmp(buffer, "MAIL FROM:", 10) == 0) {
            char *mail_from_response = "250 OK\r\n";
            send(client_socket, mail_from_response, strlen(mail_from_response), 0);
        }
        // Verifica comando RCPT TO
        else if (strncmp(buffer, "RCPT TO:", 8) == 0) {
            char *rcpt_to_response = "250 OK\r\n";
            send(client_socket, rcpt_to_response, strlen(rcpt_to_response), 0);
        }
        // Verifica comando DATA
        else if (strncmp(buffer, "DATA", 4) == 0) {
            char *data_response = "354 End data with <CR><LF>.<CR><LF>\r\n";
            send(client_socket, data_response, strlen(data_response), 0);

            // Receber o corpo da mensagem
            bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0);
            buffer[bytes_read] = '\0';
            printf("Corpo da mensagem: %s", buffer);

            // Resposta final ao comando DATA
            char *data_end_response = "250 OK: Message accepted\r\n";
            send(client_socket, data_end_response, strlen(data_end_response), 0);
        }
        // Verifica comando QUIT
        else if (strncmp(buffer, "QUIT", 4) == 0) {
            char *quit_response = "221 Bye\r\n";
            send(client_socket, quit_response, strlen(quit_response), 0);
            break;
        }
        // Comando não reconhecido
        else {
            char *unrecognized_response = "500 Command not recognized\r\n";
            send(client_socket, unrecognized_response, strlen(unrecognized_response), 0);
        }
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Criação do socket do servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Definição de endereços e portas
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Aceita conexões de qualquer endereço
    server_addr.sin_port = htons(PORT); // Porta SMTP padrão (25)

    // Vinculação do socket ao endereço e porta
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Falha ao fazer bind no socket");
        exit(EXIT_FAILURE);
    }

    // Começar a escutar conexões
    if (listen(server_socket, 3) < 0) {
        perror("Falha ao escutar no socket");
        exit(EXIT_FAILURE);
    }

    printf("Servidor SMTP rodando na porta %d...\n", PORT);

    // Loop para aceitar conexões de clientes
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) >= 0) {
        printf("Cliente conectado!\n");
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
