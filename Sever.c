#include <stdio.h>
#include <Winsock2.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080
int main()
{
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 20\n\n<h1 align='center'>This is the Frist page</h1>";
    char *hello1 = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 20\n\n<h1 align='center'>404 NOT Founded!</h1>";

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Access failed\n");
        return 1;
    }
    else
    {
        printf("Acesses successful\n");
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        return 1;
    }

    listen(server_fd, 5);
    printf("Sever is Running Now...\nlocalhost:127.0.0.1:%d", PORT);
    while (true)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

        char buffer[1024] = {0};
        int value = recv(new_socket, buffer, 1024, 0);
        switch (value)
        {
        case -1:
            printf("\n\nsome Error Occurred\n");
            break;
        case 0:
            printf("\n\nConnection Closed\n");
            break;
        default:
            printf("\n\nReceived Data: %s\n", buffer);
        }
        char resource[1024] = {0};
        int found = sscanf(buffer, "GET /%s ", resource);
        if (strcmp(resource, "favicon.ico") == 0)
        {
            closesocket(new_socket);
            continue;
        }
        printf("\n\nRequested Resource: %s\n", resource);

        if (strcmp(resource, "HTTP/1.1") == 0)
        {
            send(new_socket, hello, strlen(hello), 0);
        }
        else
        {
            char *ext_ptr = strrchr(resource, '.');
            char extention[64] = "text/plain";

            if (ext_ptr != NULL)
            {
                strcpy(extention, ext_ptr + 1);
                if (strcmp(extention, "html") == 0)
                {
                    strcpy(extention, "text/html");
                }
                else if (strcmp(extention, "css") == 0)
                {
                    strcpy(extention, "text/css");
                }
                else if (strcmp(extention, "js") == 0)
                {
                    strcpy(extention, "application/javascript");
                }
                else if (strcmp(extention, "png") == 0)
                {
                    strcpy(extention, "image/png");
                }
                else if (strcmp(extention, "jpg") == 0 || strcmp(extention, "jpeg") == 0)
                {
                    strcpy(extention, "image/jpeg");
                }
                else if (strcmp(extention, "gif") == 0)
                {
                    strcpy(extention, "image/gif");
                }
                else if (strcmp(extention, "mp4") == 0)
                {
                    strcpy(extention, "video/mp4");
                }
                else if (strcmp(extention, "pdf") == 0)
                {
                    strcpy(extention, "application/pdf");
                }
                else if (strcmp(extention, "mp3") == 0)
                {
                    strcpy(extention, "audio/mpeg");
                }
            }

            FILE *file = fopen(resource, "rb");
            if (file == NULL)
            {
                send(new_socket, hello1, strlen(hello1), 0);
            }
            else
            {

                char header[256];
                sprintf(header, "HTTP/1.1 200 OK\nContent-Type: %s\r\n\r\n", extention);
                send(new_socket, header, strlen(header), 0);

                char content[1024];
                while (fread(content, 1, 1024, file))
                {
                    send(new_socket, content, strlen(content), 0);
                }
                fclose(file);
            }
        }

        closesocket(new_socket);
    }
    WSACleanup();
    return 0;
}