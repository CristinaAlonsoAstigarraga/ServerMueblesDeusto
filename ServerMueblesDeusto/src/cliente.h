#ifndef CLIENTE_H_
#define CLIENTE_H_

typedef struct {
	char dni[10];
	char usuario[20];
	char contrasena[20];
} Cliente;

typedef struct {
	Cliente *aClientes;
	int numC;
} ListaClientes;

Cliente registro();
void anadirClientesALista(ListaClientes *lc, Cliente nuevoCliente);
void imprimirListaClientes(ListaClientes lc);
void volcarFicheroAListaClientes(ListaClientes *lc, char *nombreFichero);
void volcarListaClientesAFichero(ListaClientes *lc, char *nombreFichero);
char* buscarDniUsuario(ListaClientes lista, char *nombreUsuario);
void liberarMemoria(ListaClientes *lc);

#endif /* CLIENTE_H_ */
