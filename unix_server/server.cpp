
// Server side C/C++ program to demonstrate Socket programming
/**
 * Preform Sorting using Bubble Sort and Matrix Multiplication using
 * Multithreding received from client over socket .
 * Given/generated random arrays
 *
 * @author
 *   Abdalrahman Ibrahim <a2ibrahim@edu.aau.at>
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 2021

#include "./engine/MathLibrary.h"

using namespace MathLibrary;

#include <iostream>

#include "./engine/MatrixMultiplication.h"
#include "./engine/BubbleSort.h"

// nitialize an array of unsigned chars 20 bytes

// unsigned char *tempBuffer = new unsigned char[1024]();
// // and do not forget to delete it later
// delete[] tempBuffer;

const int BUFFER_SIZE = 10000; // 10 kbyte

unsigned char client_data_buffer[BUFFER_SIZE] = {0};

// char  : 1 byte
// short : 2 bytes
// int   : 4 bytes
// long  : 4 bytes
// float : 4 bytes
// double: 8 bytes

struct client_data
{
	char op[3]; //3 bytes
	mat A;
	corners ai;
	char nA[10]; //10 bytes
	mat B;       // 124 bytes
	corners bi;
	char nB[10];
	arr ary;
	indexes aryi;
};

unsigned char server_data_buffer[BUFFER_SIZE] = {0};

struct server_data
{
    mat C;
    corners ci;
    char nC[10];
    arr ary;
	indexes aryi;
};

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    double number;
    double sieve_result;

    //*******************************************************************//

    // sockfd: socket descriptor, an integer (like a file-handle)
    // domain: integer, communication domain e.g., AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol)
    // type: communication type
    // SOCK_STREAM: TCP(reliable, connection oriented)
    // SOCK_DGRAM: UDP(unreliable, connectionless)
    // protocol: Protocol value for Internet Protocol(IP), which is 0. This
    // is the same number which appears on protocol field in the IP header of a packet.

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); // convert and flip bits to match the machine

    // Forcefully attaching socket to the port 8080
    // we bind the server to the localhost, hence we use INADDR_ANY to specify the IP address.
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    //*******************************************************************//

    //*********************** READ FROM CLIENT **************************//

    valread = read(new_socket, client_data_buffer, sizeof(client_data_buffer));
    // printf("%s\n",buffer );

    client_data client_msg_data;

    memcpy(&client_msg_data, client_data_buffer, sizeof(client_data));

    std::cout << "Operation received from client is " << client_msg_data.op << std::endl;

    //*********************** OPERATION ON CLIENT DATA ********************//
    // compares strings lexicographically which means it compares
    // both the strings character by character

    // if (strcmp(client_msg_data.op,"add") == 0)
    // {
    //    sieve_result = Arithmetic::Add(client_msg_data.num2, client_msg_data.num1);

    // }

    if (strcmp(client_msg_data.op, "multi") == 0)
    {

        MatrixOpreations MatOp;

        corners ci = {0, N, 0, N};
        mat C;

        std::cout << "Received Matrix 1: \n" << std::endl;
        MatOp.print(client_msg_data.A,client_msg_data.ai,client_msg_data.nA);

        std::cout << "Received Matrix 2: \n" << std::endl;
        MatOp.print(client_msg_data.B,client_msg_data.bi,client_msg_data.nB);

        
        MatOp.mult_parallel(client_msg_data.A, client_msg_data.B,
                            C, client_msg_data.ai, client_msg_data.bi, ci);

        std::cout << "Parallel Multiplication on the Server: \n" << std::endl;  
        MatOp.print(C, ci, strdup("C Matrix"));

        //***************** OPERATION MSG CONSTRACTION *******************//

        server_data server_msg_data;

        memcpy(server_msg_data.C, &C, sizeof(mat));
        server_msg_data.ci = ci;
        strcpy(server_msg_data.nC, "C Matrix");

        //*********************** SEND TO CLIENT **************************//

        memcpy(server_data_buffer, &server_msg_data, sizeof(server_data));

        send(new_socket, server_data_buffer, sizeof(server_data_buffer), 0);

        printf("Server Calculation Returned to the client\n");

    }

    if (strcmp(client_msg_data.op, "sort") == 0)
    {
        
		SortingAlgo SA;

        SA.disply_arr(client_msg_data.ary,client_msg_data.aryi,strdup("Original"));
        SA.BubbleSort_parallel(client_msg_data.ary,client_msg_data.aryi);
        SA.disply_arr(client_msg_data.ary,client_msg_data.aryi,strdup("Sorted"));
        

        //***************** OPERATION MSG CONSTRACTION *******************//

        server_data server_msg_data;

        memcpy(server_msg_data.ary, &client_msg_data.ary, sizeof(mat));
        server_msg_data.aryi = client_msg_data.aryi;


        //*********************** SEND TO CLIENT **************************//

        memcpy(server_data_buffer, &server_msg_data, sizeof(server_data));

        send(new_socket, server_data_buffer, sizeof(server_data_buffer), 0);

        printf("Server Calculation Returned to the client\n");
    }
    


    // return 0;
}