#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cliente.h"

Cliente registro() {
	char get[20] = "";
	Cliente c;
	printf("\nIntroduce tu DNI: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", c.dni);
	printf("Introduce el nombre de usuario: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", c.usuario);
	printf("Introduce la contrasena: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", c.contrasena);
	return c;

}

void anadirClientesALista(ListaClientes *lc, Cliente nuevoCliente) {
	lc->aClientes = realloc(lc->aClientes, sizeof(Cliente) * (lc->numC + 1));
	lc->aClientes[lc->numC] = nuevoCliente;
	lc->numC++;
}

void imprimirListaClientes(ListaClientes lc) {
	printf("Lista de clientes de MueblesDeusto: \n");
	fflush(stdout);
	for (int i = 0; i < lc.numC; i++) {
		printf("Cliente %d: \n", i + 1);
		fflush(stdout);
		printf("DNI: %s\n", lc.aClientes[i].dni);
		fflush(stdout);
		printf("USUARIO: %s\n", lc.aClientes[i].usuario);
		fflush(stdout);
		printf("CONTRASENA: %s\n", lc.aClientes[i].contrasena);
		fflush(stdout);

	}
}

void volcarFicheroAListaClientes(ListaClientes *lc, char *nombreFichero) {
	FILE *pf;
	int tam;
	lc->numC = 0;
	pf = fopen(nombreFichero, "r");
	if (pf != (FILE*) NULL) {
		fscanf(pf, "%d", &tam);
		lc->aClientes = (Cliente*) malloc(tam * sizeof(Cliente));

		while (fscanf(pf, "%s %s %s", lc->aClientes[lc->numC].dni,
				lc->aClientes[lc->numC].usuario,
				lc->aClientes[lc->numC].contrasena) != EOF) {
			lc->numC++;
		}
		fclose(pf);
	}

}

//VOLCAMOS LA LISTA DE CLIENTES AL FICHERO "Clientes.txt"
void volcarListaClientesAFichero(ListaClientes *lc, char *nombreFichero) {
	FILE *pf;
	pf = fopen(nombreFichero, "w");
	if (pf != (FILE*) NULL) {
		fprintf(pf, "%d\n", lc->numC);
		fflush(stdout);
		for (int i = 0; i < lc->numC; i++) {
			fprintf(pf, "%s %s %s \n", lc->aClientes[i].dni,
					lc->aClientes[i].usuario, lc->aClientes[i].contrasena);
			fflush(stdout);
		}
		fclose(pf);
	}
}

char* buscarDniUsuario(ListaClientes lista, char *nombreUsuario) {
	for (int i = 0; i < lista.numC; i++) {
		if (strcmp(lista.aClientes[i].usuario, nombreUsuario) == 0) {
			return lista.aClientes[i].dni;
		}
	}
	return NULL;
}

void liberarMemoria(ListaClientes *lc) {
	free(lc->aClientes);
}

