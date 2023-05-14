#ifndef CATEGORIA_H_
#define CATEGORIA_H_

typedef enum {
   MESAS = 0,
   SILLAS = 1,
   SOFAS = 2,
}CategoriaProducto;

int imprimirListaCategorias();
const char* obtenerNombreCategoria(CategoriaProducto categoria);

#endif
