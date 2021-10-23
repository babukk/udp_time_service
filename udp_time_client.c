#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define BUFSIZE 1024

/**
 * Функция выаодит сообщение на stderr и выполняет выход из программы.
 */

void error(char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char **argv)
{
    int sockfd, portno, n;
    int serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];
    time_t current_time;

    if (argc != 3) {
       fprintf(stderr, "использование: %s <хост или IP-адрес> <порт>\n", argv[0]);
       exit(0);
    }

    hostname = argv[1];
    /* aatoi() преобразует строку argv[2] в целое число */
    portno = atoi(argv[2]);

    /* socket() - создаёт точку (endpoint) коммуникационного соединения */
    /*    AF_INET - формат: семейство протокола IPv4                            */
    /*    SOCK_DGRAM - тип: datagram - сообщения без подключений, без гаранированной доставки */
    /* функция возвращает целое число - файловый дескриптор, ссылающийся на созданную точку подключеня (endpoint) */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Ошибка открытия сокета.");

    /* gethostbyname() преобразует строку, содержащую имя хоста или IP-адрес (hotname) в структуру struct hostent *server */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Ошибка, нет такого хоста: %s\n", hostname);
        exit(0);
    }

    /* bzero() - заполняет нулями содержимое структуры serveraddr */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    /* bcopy() - копрует по-байтно содержимое server->h_addr  в  &serveraddr.sin_addr.s_addr, размером  server->h_length */
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    /* htons() - преобразует целое число portno (номер порта) в сетевой порядок следования байтов TCP/IP (возвращает тип uint16_t) */
    serveraddr.sin_port = htons(portno);

    /* time() - возвращает текущее время в секундах, начиная с 1970-01-01 00:00:00 +0000 (UTC);  UNIX time. */
    current_time = time(NULL);
    /* sprintf() преобразует целое число (в данном случае) в строковое предаставление и результат помещвет по адресу массива buf */
    sprintf((char *)buf, "%ld\n", current_time);

    serverlen = sizeof(serveraddr);
    /* sendto() - отправляет данные в сокет sockfd, данные из адреса строки buf, размером этой строки, 
                  в структуре serveraddr содержится адрес назначения (сервера)  */
    n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
    if (n < 0)
        error("Ошибка sendto.");

    /* recvfrom() - получает сообщение из сокета sockfd, данные записываются по адресу buff, размеромстроки buf,
                    в структуре serveraddr содержится адрес назначения (сервера)  */
    n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);

    if (n < 0)
        error("Ошибка recvfrom.");

    printf("Получен ответ сервера: %s", buf);
    return 0;
}
