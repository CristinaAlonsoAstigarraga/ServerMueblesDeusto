#ifndef PRODUCTO_H_
#define PRODUCTO_H_
#include "categoria.h"
#include "cliente.h"
#include <winsock2.h>
typedef struct{
    char cod_p[10];
    char nombre[20];
    char descripcion[20];
    int cantidad;
    double precio;
    int tipo;
} Producto;

typedef struct{
	Producto *aProductos;
	int numProductos;
} ListaProductos;

void quitarSalto(char *cad);
//ADMIN
void anadirProducto(ListaProductos *lp);
void anadirProductoLista(ListaProductos *lp, Producto p);
ListaProductos buscarProducto(ListaProductos lp, CategoriaProducto c);
void modificarProducto(Producto *p);
void volcarFicheroAListaProductos(ListaProductos *lp, char *nombreFichero);

void eliminarProducto(Producto p, ListaProductos *lp);
void modificarNombre(Producto *p);
void modificarDescripcion(Producto *p);
void modificarCantidad(Producto *p);
void modificarPrecio(Producto *p);

//CLIENTE
void devolverProducto(ListaProductos *lp, Producto nombreProducto);
void visualizarTienda(ListaProductos lp);
void imprimirListaProductos (ListaProductos lp);
Producto nombreProductoBorrar();
Producto nombreProductoDevolver();
Producto codigoProductoBorrar();
int nuevaCantidadProducto();
Producto codigoProductoModificar();
Producto anadirProductoBD();
int buscarProductoCategoria();
Producto* buscarProd(ListaProductos lista, char *codigo);


void enviarListaProductos(ListaProductos lp, SOCKET comm_socket, char *sendBuff);
#endif /* PRODUCTO_H_ */
