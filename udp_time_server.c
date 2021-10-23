#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    int sockfd;
    int portno;
    int clientlen;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    struct hostent *hostp;
    char buf[BUFSIZE];
    char *hostaddrp;
    int optval;
    int n_bytes;
    time_t current_time;


    if (argc != 2) {
        fprintf(stderr, "использование: %s <порт>\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[1]);

    if ((portno <= 0) || (portno > 65535))
        error("екорректный номер порта.\n");

    /* socket() - создаёт точку (endpoint) коммуникационного соединения */
    /*    AF_INET - формат: семейство протокола IPv4                            */
    /*    SOCK_DGRAM - тип: datagram - сообщения без подключений, без гаранированной доставки */
    /* функция возвращает целое число - файловый дескриптор, ссылающийся на созданную точку подключеня (endpoint) */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("Ошибка открытия socket.");

    optval = 1;
    /* setsockopt() - устанавливает набор опция для сокета sockfd (костанты определены в sys/socket.h) */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    /* bzero() - заполняет нулями содержимое структуры serveraddr */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    /* htonl() - преобразует длинное беззнаковое целое INADDR_ANY (константу) в сетевой порядок следования байтов TCP/IP,
          в котором числа оканчиваются старшим байтом. */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* htonl() - преобразует беззнаковое короткое (short) целое portno в сетевой порядок следования байтов TCP/IP */
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        error("Ошика bind");

    /* получаем азмер структуры clientaddr */
    clientlen = sizeof(clientaddr);

    while (1) {
        /* bzero() - заполняет нулями содержимое массива buf */
        bzero(buf, BUFSIZE);
        /* recvfrom() - получает сообщение из сокета sockfd (от клиента), данные записываются по адресу buff, размером BUFSIZE,
                    в структуру cliendaddr возвращается адрес клиента, который далее можно использовать для обмена.  */
        n_bytes = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&clientaddr, &clientlen);

        /* atol() - преображует строку в длинное целок число */
        time_t tt = atol(buf);
        /* localtime() - преобразует время в секундах в структуру struct tm - локальное время-дата */
        struct tm *local_tm = localtime(&tt);
        printf("Получено время клиента: %4d-%02d-%02d %02d:%02d:%02d\n", local_tm->tm_year + 1900, local_tm->tm_mon + 1, local_tm->tm_mday, local_tm->tm_hour, local_tm->tm_min, local_tm->tm_sec);

        /* time() - возвращает текущее время в секундах, начиная с 1970-01-01 00:00:00 +0000 (UTC);  UNIX time. */
        current_time = time(NULL);

        if (n_bytes < 0)
            printf("Ошибка recvfrom.");

        /* gethostbyaddr() - получает сетевой адрес клиента и возвращает его в структуру hostp  (для последующей проверки) */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);

        if (hostp == NULL)
            printf("Ошибка gethostbyaddr.");

        /* inet_ntoa() - преобразует сетевой адорес clientaddr.sin_addr в строковое представление IP-адреса в hostaddrp (для полседующей проверки) */
        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL)
            printf("Ошибка inet_ntoa.\n");

        /* sprintf() преобразует целое число (в данном случае) в строковое предаставление и результат помещвет по адресу массива buf */
        sprintf((char *)buf, "%ld\n", current_time);

        /* sendto() - отправляет данные в сокет sockfd, данные из адреса строки buf, размером этой строки,
                  в структуре clientaddr содержится адрес назначения (клиента)  */
        n_bytes = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &clientaddr, clientlen);

        if (n_bytes < 0)
            printf("Ошибка sendto.");
    }
}
