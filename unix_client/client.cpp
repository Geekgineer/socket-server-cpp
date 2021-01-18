// Client side C/C++ program to demonstrate Socket programming
/**
 * Preform Sorting using Bubble Sort and Matrix Multiplication using
 * Multithreding on a remote socket server.
 * Given/generated random arrays
 *
 * @author
 *   Abdalrahman Ibrahim <a2ibrahim@edu.aau.at>
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <random>
#include <iostream>

#define PORT 2021

#define OP1 "multi"
#define OP2 "sort"

const char OPERATION[6] = OP1;

const int BUFFER_SIZE = 10000; // 10 kbyte

#include "./helpers/MatrixMultiplication.h"
#include "./helpers/BubbleSort.h"

// unsigned char *tempBuffer = new unsigned char[1024]();
// // and do not forget to delete it later
// delete[] tempBuffer;

unsigned char client_data_buffer[BUFFER_SIZE] = {0};

struct client_data
{
	char op[3]; //3 bytes
	mat A;   // 124 bytes
	corners ai;
	char nA[10];
	mat B;
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
	int sock = 0, valread;

	// Structure describing an Internet socket address.
	struct sockaddr_in serv_addr;

	//*******************************************************************//
	// create socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	// bare bone server
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT); // convert and flip bits to match the machine

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	//*******************************************************************//

	if (strcmp(OPERATION, "multi") == 0)
	{

		//******************* OPERATION MSG CONSTRACTION ********************//

		//---------------------------------------//
		// for a given square matrix 8x8 random
		corners ai = {0, N, 0, N};
		corners bi = {0, N, 0, N};

		mat A;
		mat B;
		

		MatrixOpreations op;

		op.rand_mat(A, ai, 0, 10);

		op.rand_mat(B, bi, 0, 10);

		// op.print(A, ai, "A");
		// std::cout << sizeof(A) << std::endl;

		client_data client_msg_data;

		// fill the massage
		strcpy(client_msg_data.op, "multi");

		memcpy(client_msg_data.A, &A, sizeof(mat));
		client_msg_data.ai = ai;
		strcpy(client_msg_data.nA, "A Matrix");

		memcpy(client_msg_data.B, &B, sizeof(mat));
		client_msg_data.bi = bi;
		strcpy(client_msg_data.nB, "B Matrix");

		std::cout << "Operation sent to server is " << client_msg_data.op << std::endl;

		op.print(A, ai, strdup("A Matrix"));
		op.print(B, bi, strdup("B Matrix"));

		//*********************** SEND TO SERVER **************************//

		// Fill the buffer with the massage
		memcpy(client_data_buffer, &client_msg_data, sizeof(client_data));

		// Send to server
		send(sock, client_data_buffer, sizeof(client_data_buffer), 0);

		//*********************** READ FROM SERVER **************************//

		valread = read(sock, server_data_buffer, sizeof(server_data_buffer));

		server_data server_msg;

		memcpy(&server_msg, server_data_buffer, sizeof(server_data));
		std::cout << "response from server is " << server_msg.nC << std::endl;

		op.print(server_msg.C, server_msg.ci, server_msg.nC);
	}

	if (strcmp(OPERATION, "sort") == 0){


		//---------------------------------------//
		arr A;

		// Array limit
		indexes ai = {0, 15};

		SortingAlgo SA;

		// Generate Random Array[3999] each element from rang 1 to 15000
		SA.rand_arr(A, ai, 1, 50);


		client_data client_msg_data;

		// fill the massage
		strcpy(client_msg_data.op, "sort");
		memcpy(client_msg_data.ary, &A, sizeof(arr));
		client_msg_data.aryi = ai;

		std::cout << "Operation sent to server is " << client_msg_data.op << std::endl;

		SA.disply_arr(A,ai,strdup("Original"));

		//*********************** SEND TO SERVER **************************//

		// Fill the buffer with the massage
		memcpy(client_data_buffer, &client_msg_data, sizeof(client_data));

		// Send to server
		send(sock, client_data_buffer, sizeof(client_data_buffer), 0);

		//*********************** READ FROM SERVER **************************//

		valread = read(sock, server_data_buffer, sizeof(server_data_buffer));

		server_data server_msg;

		memcpy(&server_msg, server_data_buffer, sizeof(server_data));
		std::cout << "response from server " << server_msg.nC << std::endl;

		SA.disply_arr(server_msg.ary,server_msg.aryi,strdup("Sorted"));
	}

	// return 0;
}
