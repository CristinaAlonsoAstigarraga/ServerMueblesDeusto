#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "producto.h"

//ADMINISTRADOR:
void anadirProducto(ListaProductos *lp) {
	char get[20] = "";
	Producto p;
	printf("Codigo: \n");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", p.cod_p);
	printf("Nombre: \n");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", p.nombre);
	printf("Cantidad: \n");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%d", &p.cantidad);
	printf("Precio: \n");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%lf", &p.precio);
	printf("Categoria: \n");
	fflush(stdout);
	fflush(stdin);
}

ListaProductos buscarProducto(ListaProductos lp, CategoriaProducto c) {
	ListaProductos lpCategoria;
	lpCategoria.numProductos = 0;
	lpCategoria.aProductos = (Producto*) malloc(
			lp.numProductos * sizeof(Producto));
	imprimirListaProductos(lp);
	fflush(stdout);
	for (int i = 0; i < lp.numProductos; i++) {
		if (lp.aProductos[i].tipo == c) {
			lpCategoria.aProductos[lpCategoria.numProductos++] =
					lp.aProductos[i];
		}
	}
	return lpCategoria;
}


//CLIENTE
void devolverProducto(ListaProductos *lp, Producto nombreProducto) {
	Producto p;
	strcpy(p.cod_p, nombreProducto.cod_p);
	int i;
	for (i = 0; i < lp->numProductos; i++) {
		if (strcmp(lp->aProductos[i].cod_p, p.cod_p) == 0) {
			lp->aProductos[i].cantidad++;
			break;
		}
	}
	printf("\nDevolucion finalizada. \n");
	fflush(stdout);
}

void visualizarTienda(ListaProductos lp) {
	int i;
	for (i = 0; i < lp.numProductos; i++) {
		printf("CÓDIGO DEL PRODUCTO: %s\n", lp.aProductos[i].cod_p);
		fflush(stdout);
		printf("NOMBRE: %s\n", lp.aProductos[i].nombre);
		fflush(stdout);
		printf("CANTIDAD: %d\n", lp.aProductos[i].cantidad);
		fflush(stdout);
		printf("PRECIO: %.2f\n", lp.aProductos[i].precio);
		fflush(stdout);
		printf("CATEGORIA: %s\n",
				obtenerNombreCategoria(lp.aProductos[i].tipo));
		fflush(stdout);
	}
}

void imprimirListaProductos(ListaProductos lp) {
	printf("\nLista de productos de MueblesDeusto: \n");
	fflush(stdout);
	for (int i = 0; i < lp.numProductos; i++) {
		printf("[PRODUCTO %d: ", i + 1);
		fflush(stdout);
		printf("CODIGO: %s, ", lp.aProductos[i].cod_p);
		fflush(stdout);
		printf("NOMBRE: %s, ", lp.aProductos[i].nombre);
		fflush(stdout);
		printf("DESCRIPCION: %s, ", lp.aProductos[i].descripcion);
		fflush(stdout);
		printf("CANTIDAD: %d, ", lp.aProductos[i].cantidad);
		fflush(stdout);
		printf("PRECIO: %.2f, ", lp.aProductos[i].precio);
		fflush(stdout);
		printf("CATEGORIA: %s]\n",
				obtenerNombreCategoria(lp.aProductos[i].tipo));
		fflush(stdout);
	}
}

void volcarFicheroAListaProductos(ListaProductos *lp, char *nombreFichero) {
	FILE *pf;
	int tam;
	int cat;
	lp->numProductos = 0;
	pf = fopen(nombreFichero, "r");
	if (pf != (FILE*) NULL) {
		fscanf(pf, "%d", &tam);
		fflush(stdout);
		lp->aProductos = (Producto*) malloc(tam * sizeof(Producto));
		while (fscanf(pf, "%s%s%s%d%lf%d",
				lp->aProductos[lp->numProductos].cod_p,
				lp->aProductos[lp->numProductos].nombre,
				lp->aProductos[lp->numProductos].descripcion,
				&(lp->aProductos[lp->numProductos].cantidad),
				&(lp->aProductos[lp->numProductos].precio), &cat) != EOF) {

			lp->aProductos[lp->numProductos].tipo = (CategoriaProducto) cat;
			lp->numProductos++;
		}
		fclose(pf);
	} else {
		lp->aProductos = NULL;
		return;
	}

}

Producto nombreProductoBorrar() {
	char get[20] = "";
	Producto p;
	printf(
			"\n¿Qué producto desea eliminar? (introduzca el codigo del producto): ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	if(get[strlen(get)-1]=='\n')
		get[strlen(get) - 1] = '\0';
	sprintf(p.cod_p, "%s", get);
	return p;
}

Producto nombreProductoDevolver() {
	char get[20] = "";
	Producto p;
	printf(
			"\n¿Qué producto desea devolver? (introduzca el codigo del producto): ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	get[strlen(get) - 1] = '\0';
	sprintf(p.cod_p, "%s", get);
	return p;
}

Producto codigoProductoBorrar() {
	char get[20] = "";
	Producto p;
	printf(
			"\n¿Qué producto desea eliminar? (introduzca el código del producto): ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	sscanf(get, "%s", p.cod_p);
	return p;
}

int nuevaCantidadProducto() {
	int nuevaCantidad;
	char get[1000] = "";
	printf("Introduce la nueva cantidad del producto: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 1000, stdin);
	sscanf(get, "%d", &nuevaCantidad);
	return nuevaCantidad;
}

Producto codigoProductoModificar() {
	char get[20] = "";
	Producto p;
	printf(
			"\n¿Qué producto desea modificar? (introduzca el código del producto): ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	get[strlen(get) - 1] = '\0';
	sscanf(get, "%s", p.cod_p);
	return p;
}

void quitarSalto(char *cad){
	if(cad[strlen(cad)-1]=='\n')
		cad[strlen(cad)-1] = '\0';
}

Producto anadirProductoBD() {
	char get[20] = "";
	Producto p;
	printf("\nIntroduce los datos del nuevo producto: \n");
	fflush(stdout);
	printf("Codigo: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%s", p.cod_p);
	printf("Nombre: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%s", p.nombre);
	printf("Descripcion: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%s", p.descripcion);
	printf("Cantidad: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%d", &p.cantidad);
	printf("Precio: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%lf", &p.precio);
	printf("Categoria: ");
	fflush(stdout);
	fflush(stdin);
	fgets(get, 20, stdin);
	quitarSalto(get);
	sscanf(get, "%d", &p.tipo);
	return p;
}
int buscarProductoCategoria() {
	int opcion;

	return opcion;
}

Producto* buscarProd(ListaProductos lista, char *codigo) {
	Producto *p = malloc(sizeof(Producto));
	fflush(stdout);
	imprimirListaProductos(lista);
	for (int i = 0; i < lista.numProductos; i++) {
		if (strcmp(lista.aProductos[i].cod_p, codigo) == 0) {
			strcpy(p->cod_p, lista.aProductos[i].cod_p);
			strcpy(p->nombre, lista.aProductos[i].nombre);
			strcpy(p->descripcion, lista.aProductos[i].descripcion);
			p->cantidad = lista.aProductos[i].cantidad;
			p->precio = lista.aProductos[i].precio;
			p->tipo = lista.aProductos[i].tipo;
		}
	}
	return p;
}
