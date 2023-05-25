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

/**
 * Método para leer el fichero de configuración que permite realizar la conexión a la base de datos.
 */
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

	/* EMPIEZA EL PROGRAMA DEL SERVIDOR*/

	//Lista de Clientes:
	ListaClientes lc;
	lc.numC = 0;
	ListaClientes admin;

	//Clientes:
	volcarFicheroAListaClientes(&lc, "Clientes.txt");

	//Administradores:
	volcarFicheroAListaClientes(&admin, "Administradores.txt");

	//Lista de Productos:
	ListaProductos productoBD;
	ListaProductos acutalizadaModificado;
	ListaProductos acutalizadaBorrado;
	ListaProductos mostrarAlmacen;

	//Productos:
	Producto modificarP;
	Producto eliminarP;
	Producto codigoProductoDevolver;

	//Abrimos la BBDD para poder volcar los productos que hay en la misma a una Lista de Productos.
	rc = sqlite3_open(nombd, &db);
	if (rc != SQLITE_OK) {
		printf("Error abriendo la base de datos: %s\n", sqlite3_errmsg(db));
		fflush(stdout);
		sqlite3_close(db);
		return 1;
	}

	volcarAListaProductosBD(db, &productoBD);
	sqlite3_close(db);

	//MENÚ PRINCIPAL
	do {
		//Enteros, doubles y cadenas:
		int opcion, opcion2, cantidad, tipo, i, clienteExiste, adminExiste,
				nuevaCantidad;
		char dni[20], nom[20], con[20], cod[20], desc[20];
		double precio = 0;
		do {
			recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
			sscanf(recvBuff, "%i", &opcion);
			switch (opcion) {
			case 1: //REGISTRO DE UN NUEVO CLIENTE
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
					recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la contraseña
					sprintf(con, "%s", recvBuff);
					Cliente nuevoCliente;
					strcpy(nuevoCliente.dni, dni);
					strcpy(nuevoCliente.usuario, nom);
					strcpy(nuevoCliente.contrasena, con);
					anadirClientesALista(&lc, nuevoCliente);
					volcarListaClientesAFichero(&lc, "Clientes.txt");
					fflush(stdout);
				}
				break;
			case 2:	//INICIO DE SESIÓN (DE CLIENTE O DE ADMIN)
				clienteExiste = 0;
				adminExiste = 0;
				recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el nombre
				sprintf(nom, "%s", recvBuff);
				recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la contrase�a
				sprintf(con, "%s", recvBuff);
				//La buscamos en la BBDD
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

				if (clienteExiste) { //INICIO DE SESIÓN DE CLIENTE Y SU MENÚ
					do {
						recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
						sscanf(recvBuff, "%i", &opcion2);
						switch (opcion2) {
						case 1: //VISUALIZAR EL CARRITO DE LA COMPRA

							break;
						case 2: //DEVOLVER UN PRODUCTO
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
						case 3: //VISUALIZAR TODOS LOS PRODUCTOS DE LA TIENDA
							sqlite3_open(nombd, &db);
							volcarAListaProductosBD(db, &productoBD);
							sqlite3_close(db);
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						case 4: //BUSCAR UN PRODUCTO EN LA TIENDA - POR CATEGORIAS
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						}
					} while (opcion != 0);

				} else if (adminExiste) { //INICIO DE SESIÓN ADMINISTRADOR Y SU MENÚ
					do {
						recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
						sscanf(recvBuff, "%i", &opcion2);
						switch (opcion2) {
						case 1: //AÑADIR UN PRODUCTO A LA BBDD DE LA TIENDA
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
							sqlite3_open(nombd, &db);
							insertarProductoBD(db, nuevoProducto);
							sqlite3_close(db);
							anadirProductoLista(&productoBD, nuevoProducto);
							break;
						case 2: //MODIFICAR UN PRODUCTO DE LA BBDD DE LA TIENDA
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe la nueva cantidad para el producto
							sscanf(recvBuff, "%i", &nuevaCantidad);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo del producto a modificar
							sprintf(modificarP.cod_p, "%s", recvBuff);
							sqlite3_open(nombd, &db);
							modificarCantidadProductoBD(db, modificarP.cod_p,
									nuevaCantidad);
							volcarAListaProductosBD(db, &productoBD);
							sqlite3_close(db);
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						case 3: //ELIMINAR UN PRODUCTO DE LA BBDD DE LA TIENDA
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							recv(comm_socket, recvBuff, sizeof(recvBuff), 0); //Recibe el codigo del producto a eliminar
							sprintf(eliminarP.cod_p, "%s", recvBuff);
							sqlite3_open(nombd, &db);
							borrarProductoBD(db, eliminarP.cod_p);
							volcarAListaProductosBD(db, &productoBD);
							sqlite3_close(db);
							enviarListaProductos(productoBD, comm_socket,
									sendBuff);
							break;
						case 4: //MOSTRAR EL ALMACEN DE LA TIENDA (LOS PRODUCTOS DE LA BBDD)
							sqlite3_open(nombd, &db);
							volcarAListaProductosBD(db, &mostrarAlmacen);
							sqlite3_close(db);
							enviarListaProductos(mostrarAlmacen, comm_socket,
									sendBuff);
							break;
						case 5: //MOSTRAR LAS ESTADÍSTICAS DE LA TIENDA
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

