#include <stdio.h>
#include "categoria.h"

int imprimirListaCategorias() {
	int categoria;
	char get[2] ="";
	printf("\nIntroduce una categoria (0 - MESAS, 1 - SILLAS, 2 - SOFAS): ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 2, stdin);
	sscanf(get, "%d", &categoria);
	return categoria;
}


const char* obtenerNombreCategoria(CategoriaProducto categoria) {
    switch (categoria) {
        case MESAS:
            return "MESAS";
        case SILLAS:
            return "SILLAS";
        case SOFAS:
            return "SOFAS";
        default:
            return "DESCONOCIDO";
    }
}
