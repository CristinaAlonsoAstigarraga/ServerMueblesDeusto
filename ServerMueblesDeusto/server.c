// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/cliente.h"
#include "src/bbdd/consultas.h"

#include <stdio.h>
#include <winsock2.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

void leerConfig(char *nomfich, char *nombd) {
	FILE *pf;

	pf = fopen(nomfich, "r");
	if (pf != (FILE*) NULL) {
		fscanf(pf, "%s", nombd);
		fclose(pf);
	}
}

int main(int argc, char *argv[]) {
	/*AÑADIMOS LO NECESARIO PARA LA BBDD*/

	sqlite3 *db;
	char nombd[50];

	leerConfig("Configuracion.conf", nombd);
	int rc = sqlite3_open(nombd, &db);
	if (rc != SQLITE_OK) {
		printf("Error abriendo la base de datos: %s\n", sqlite3_errmsg(db));
		fflush(stdout);
		sqlite3_close(db);
		return 1;
	}
	sqlite3_close(db);

	WSADATA wsaData;
	SOCKET conn_socket; //el que lleva la conexion
	SOCKET comm_socket; //el que lo comunica
	struct sockaddr_in server;
	struct sockaddr_in client;
	char sendBuff[512], recvBuff[512]; // lo que yo envio, lo que yo recibo

	printf("\nInitialising Winsock...\n"); // inicializa la libreria
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return -1;
	}

	printf("Initialised.\n");

	//SOCKET creation creacion del socket( la primera estructura
	if ((conn_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Socket created.\n");
	// cual es la ip y cual es el puerto
	server.sin_addr.s_addr = inet_addr(SERVER_IP); //INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	//BIND (the IP/port with socket)
	if (bind(conn_socket, (struct sockaddr*) &server,
			sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		closesocket(conn_socket);
		WSACleanup();
		return -1;
	}

	printf("Bind done.\n"); //DEJAR EL SOCKET EN ESPERA

	//LISTEN to incoming connections (socket server moves to listening mode)
	if (listen(conn_socket, 1) == SOCKET_ERROR) {
		printf("Listen failed with error code: %d", WSAGetLastError());
		closesocket(conn_socket);
		WSACleanup();
		return -1;
	}

	//ACCEPT incoming connections (server keeps waiting for them)
	printf("Waiting for incoming connections...\n");
	int stsize = sizeof(struct sockaddr);
	comm_socket = accept(conn_socket, (struct sockaddr*) &client, &stsize);
	// Using comm_socket is able to send/receive data to/from connected client
	if (comm_socket == INVALID_SOCKET) {
		printf("accept failed with error code : %d", WSAGetLastError());
		closesocket(conn_socket);
		WSACleanup();
		return -1;
	}
	printf("Incomming connection from: %s (%d)\n", inet_ntoa(client.sin_addr),
			ntohs(client.sin_port));

	// Closing the listening sockets (is not going to be used anymore)
	closesocket(conn_socket);
	int fin = 0;

	Producto modificarP;
	Producto eliminarP;
	Producto caro;
	Producto mayorCantidad;
	ListaClientes lc;
	lc.numC = 0;
	ListaClientes admin;
	Producto codigoProductoDevolver;
	ListaProductos listaCategoria;
	//CLIENTES
	volcarFicheroAListaClientes(&lc, "Clientes.txt");
//	imprimirListaClientes(lc);
	int categoria;

	//ADMINISTRADORES

	volcarFicheroAListaClientes(&admin, "Administradores.txt");

	ListaProductos productoBD;
//	productoBD.numProductos = 0;
//	productoBD.aProductos = NULL;

	rc = sqlite3_open(nombd, &db);
	if (rc != SQLITE_OK) {
		printf("Error abriendo la base de datos: %s\n", sqlite3_errmsg(db));
		fflush(stdout);
		sqlite3_close(db);
		return 1;
	}

	volcarAListaProductosBD(db, &productoBD);
//	imprimirListaProductos(productoBD);
	sqlite3_close(db);

//	printf("Número de productos de la lista: %i\n", productoBD.numProductos);
//	fflush(stdout);
//	for(int i=0; i<productoBD.numProductos; i++){
//		printf("\n", productoBD.aProductos[i].cod_p);fflush(stdout);
//	}
	do {
		/*EMPIEZA EL PROGRAMA DEL SERVIDOR*/
		int opcion, opcion2;
		char dni[20], nom[20], con[20], cod[20], desc[20];
		int cantidad, tipo, modif;
		double precio = 0;
		int resul, i, clienteExiste, adminExiste, nuevaCantidad;
		do {
			recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
			sscanf(recvBuff, "%i", &opcion);
			switch (opcion) {
			case 1:
				recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el dni
				sprintf(dni, "%s", recvBuff);
				clienteExiste = 0;
				for (i = 0; i < lc.numC; i++) {
					if (strcmp(dni, lc.aClientes[i].dni) == 0) { //Comparamos el dni del cliente nuevo con el resto de nuestros clientes
						clienteExiste = 1;
						break;
					}
				}
				sprintf(sendBuff, "%d", clienteExiste);
				send(comm_socket, sendBuff, sizeof(sendBuff), 0);
				if (!clienteExiste) {
					recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el nombre
					sprintf(nom, "%s", recvBuff);
					recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el nombre
					sprintf(con, "%s", recvBuff);
					Cliente nuevoCliente;
					strcpy(nuevoCliente.dni, dni);
					strcpy(nuevoCliente.usuario, nom);
					strcpy(nuevoCliente.contrasena, con);
					anadirClientesALista(&lc, nuevoCliente);
					volcarListaClientesAFichero(&lc, "Clientes.txt");
					printf("Añadido\n");
					fflush(stdout);
				} else {
					printf("Cliente ya existe\n");
					fflush(stdout);
				}
				break;
			case 2:
				clienteExiste = 0;
				adminExiste = 0;
				recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el nombre
				sprintf(nom, "%s", recvBuff);
				recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la contrase�a
				sprintf(con, "%s", recvBuff);
				//La busc�is en la BBDD
				for (i = 0; i < lc.numC; i++) {
					if ((strcmp(nom, lc.aClientes[i].usuario) == 0)
							&& (strcmp(con, lc.aClientes[i].contrasena) == 0)) {
						clienteExiste = 1;
						strcpy(dni, buscarDniUsuario(lc, nom));
						sprintf(sendBuff, "%s", dni);
						send(comm_socket, sendBuff, sizeof(sendBuff), 0);
						break;
					}
				}
				if (!clienteExiste) {
					sprintf(sendBuff, "NULL");
					send(comm_socket, sendBuff, sizeof(sendBuff), 0);
				}
				sprintf(sendBuff, "%d", clienteExiste);
				send(comm_socket, sendBuff, sizeof(sendBuff), 0);

				for (i = 0; i < admin.numC; i++) {
					if ((strcmp(nom, admin.aClientes[i].usuario) == 0)
							&& (strcmp(con, admin.aClientes[i].contrasena) == 0)) {
						adminExiste = 1;
						break;
					}
				}
				sprintf(sendBuff, "%d", adminExiste);
				send(comm_socket, sendBuff, sizeof(sendBuff), 0);

				if (clienteExiste) {
					do {
						recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
						sscanf(recvBuff, "%i", &opcion2);
						switch (opcion2) {
						case 1:
							break;
						case 2:
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo del producto a devolver
							sprintf(codigoProductoDevolver.cod_p, "%s",
									recvBuff);
							devolverProducto(&productoBD,
									codigoProductoDevolver);
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							sqlite3_open(nombd, &db);
							devolverProductoBD(db, codigoProductoDevolver);
							sqlite3_close(db);
							break;
						case 3:
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						case 4:
//							imprimirListaProductos(productoBD);
//							recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
//							sscanf(recvBuff, "%i", &categoria);
//							if (categoria == 0) {
//								sqlite3_open(nombd, &db);
//								devolverProductoBD(db, codigoProductoDevolver);
//								sqlite3_close(db);
//							} else if (categoria == 1) {
//								sqlite3_open(nombd, &db);
//								devolverProductoBD(db, codigoProductoDevolver);
//								sqlite3_close(db);
//							} else if (categoria == 2) {
//								sqlite3_open(nombd, &db);
//								devolverProductoBD(db, codigoProductoDevolver);
//								sqlite3_close(db);
//							} else {
//								printf(
//										"Error, no ha introducido una categoria válida. ")
//							}
//							sqlite3_open(nombd, &db);
//							mostrarProductosCategoriaBDLista(db, codigoProductoDevolver);
//							sqlite3_close(db);
//							enviarListaProductos(listaCategoria, comm_socket,
//																sendBuff);

							break;
						case 5:
							break;

						}
					} while (opcion != 0);

				} else if (adminExiste) {
					do {
						recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
						sscanf(recvBuff, "%i", &opcion2);
						switch (opcion2) {
						case 1:
							printf("Servidor, opción 2\n");
							//Recibe los datos del nuevo producto para añadir a la BD
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo
							sprintf(cod, "%s", recvBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el nombre
							sprintf(nom, "%s", recvBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la descripción
							sprintf(desc, "%s", recvBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la cantidad
							sscanf(recvBuff, "%i", &cantidad);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el precio
							sscanf(recvBuff, "%lf", &precio);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el tipo
							sscanf(recvBuff, "%i", &tipo);
							Producto nuevoProducto;
							strcpy(nuevoProducto.cod_p, cod);
							strcpy(nuevoProducto.nombre, nom);
							strcpy(nuevoProducto.descripcion, desc);
							nuevoProducto.cantidad = cantidad;
							nuevoProducto.precio = precio;
							nuevoProducto.tipo = tipo;
							//sqlite3_open(nombd, &db);
							sqlite3_open(nombd, &db);
							insertarProductoBD(db, nuevoProducto);
							sqlite3_close(db);
							printf("SE ha intentado");
							fflush(stdout);
							anadirProductoLista(&productoBD, nuevoProducto);
							break;
						case 2:
							sqlite3_open(nombd, &db);
							mostrarProductosBD(db);
							sqlite3_close(db);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el tipo
							sscanf(recvBuff, "%i", &nuevaCantidad);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo
							sprintf(modificarP.cod_p, "%s", recvBuff);
							sqlite3_open(nombd, &db);
							modificarCantidadProductoBD(db, modificarP.cod_p,
									nuevaCantidad);
							sqlite3_close(db);
							break;
						case 3:
							sqlite3_open(nombd, &db);
							mostrarProductosBD(db);
							sqlite3_close(db);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo
							sprintf(eliminarP.cod_p, "%s", recvBuff);
							sqlite3_open(nombd, &db);
							borrarProductoBD(db, eliminarP.cod_p);
							sqlite3_close(db);
							break;
						case 4:
//							imprimirListaProductos(productoBD);
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						case 5:
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;

						}
					} while (opcion != 0);
				}

				break;
			case 0:
				fin = 1;
				printf("FIN DE LA CONEXI�N");
				break;
			}
		} while (opcion != '0');

		/*ACABA EL PROGRAMA DEL SERVIDOR*/

	} while (fin == 0);

	// CLOSING the sockets and cleaning Winsock...
	closesocket(comm_socket);
	WSACleanup();

	return 0;
}

