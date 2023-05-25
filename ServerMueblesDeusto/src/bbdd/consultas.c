#include "consultas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int abrirBD(sqlite3 **db) {
	int rc = sqlite3_open("MueblesDeusto.db", db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error al abrir la base de datos: %s\n",
				sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return rc;
	}
	return SQLITE_OK;
}

int crearTablaProducto(char *nombreBaseDatos) {
	sqlite3 *db;
	char *errMsg = NULL;
	int rc;

	// Abrir la base de datos
	rc = sqlite3_open(nombreBaseDatos, &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error al abrir la base de datos: %s\n",
				sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	// Crear la tabla producto
	char *sql =
			"CREATE TABLE if not exists producto (cod_p VARCHAR(10) PRIMARY KEY, nombre VARCHAR(20) NOT NULL, descripcion VARCHAR(20), cantidad INT NOT NULL, precio DOUBLE NOT NULL, tipo VARCHAR(20) NOT NULL);";
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error al crear la tabla producto: %s\n", errMsg);
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return 1;
	}

	// Cerrar la base de datos
	sqlite3_close(db);
	return 0;
}

int insertarProductoBD(sqlite3 *db, Producto p) {
	char sql[300];
	char cad[10];
	sqlite3_stmt *stmt;
	// Crear la sentencia SQL para insertar el producto
	sprintf(cad, "%d", p.tipo);
	sprintf(sql,
			"INSERT INTO producto VALUES ('%s', '%s', '%s', %d, %lf, '%s')",
			p.cod_p, p.nombre, p.descripcion, p.cantidad, p.precio, cad);

	// Ejecutar la sentencia SQL
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Error preparando la consulta: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return 0;
}

int mostrarProductosBD(sqlite3 *db) {
	char *sql = "SELECT * FROM producto;";
	sqlite3_stmt *stmt;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Error preparando la consulta: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}

	printf("\n%-10s %-10s %-15s %-10s %-10s %-10s\n", "cod_p", "nombre",
			"descripcion", "cantidad", "precio", "tipo");
	fflush(stdout);
	printf(
			"-----------------------------------------------------------------\n");
	fflush(stdout);

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		char *cod_p = (char*) sqlite3_column_text(stmt, 0);
		char *nombre = (char*) sqlite3_column_text(stmt, 1);
		char *descripcion = (char*) sqlite3_column_text(stmt, 2);
		int cantidad = sqlite3_column_int(stmt, 3);
		double precio = sqlite3_column_double(stmt, 4);
		char *tipo = (char*) sqlite3_column_text(stmt, 5);
		printf("%-10s %-10s %-15s %-10d %-10.2f %-10s\n", cod_p, nombre,
				descripcion, cantidad, precio, tipo);
		fflush(stdout);
	}

	sqlite3_finalize(stmt);
	return 0;
}

int modificarCantidadProductoBD(sqlite3 *db, char *cod_p, int nueva_cantidad) {
	char sql[100];
	char *errMsg = NULL;
	int rc;

	// Verificar si el producto existe en la base de datos
	sprintf(sql, "SELECT * FROM producto WHERE cod_p = '%s'", cod_p);
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (rc == SQLITE_DONE) {
		fprintf(stderr,
				"El producto con código %s no existe en la base de datos.\n",
				cod_p);
		sqlite3_free(errMsg);
		return 1;
	}

	// Crear la sentencia SQL para actualizar la cantidad del producto
	sprintf(sql, "UPDATE producto SET cantidad = %d WHERE cod_p = '%s';",
			nueva_cantidad, cod_p);
	// Ejecutar la sentencia SQL
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error al modificar la cantidad del producto: %s\n",
				errMsg);
		sqlite3_free(errMsg);
		return 1;
	}

	return 0;
}

int borrarProductoBD(sqlite3 *db, char *cod_p) {
	char *errMsg = NULL;
	int rc;

	// Crear la sentencia SQL para borrar un producto
	char sql[100];
	sprintf(sql, "DELETE FROM producto WHERE cod_p='%s'", cod_p);

	// Ejecutar la sentencia SQL
	rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error al borrar el producto %s: %s\n", cod_p, errMsg);
		sqlite3_free(errMsg);
		return 1;
	}

	return 0;
}

void volcarListaProductosABD(sqlite3 *db, ListaProductos *lista) {
	char sql[] =
			"INSERT INTO producto (cod_p, nombre, descripcion, cantidad, precio, tipo) VALUES (?, ?, ?, ?, ?, ?)";
	int result;
	sqlite3_stmt *stmt;

	for (int i = 0; i < lista->numProductos; i++) {
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_bind_text(stmt, 1, lista->aProductos[i].cod_p,
				strlen(lista->aProductos[i].cod_p), SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 2, lista->aProductos[i].nombre,
				strlen(lista->aProductos[i].nombre), SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 3, lista->aProductos[i].descripcion,
				strlen(lista->aProductos[i].descripcion), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 4, lista->aProductos[i].cantidad);
		sqlite3_bind_double(stmt, 5, lista->aProductos[i].precio);
		sqlite3_bind_int(stmt, 6, lista->aProductos[i].tipo);

		result = sqlite3_step(stmt);
		if (result != SQLITE_DONE) {
			printf("Error insertando producto: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return;
		}

		sqlite3_finalize(stmt);
	}
}

Producto buscarProductoCaro(sqlite3 *db) {
	char sql[] = "SELECT * FROM producto ORDER BY precio DESC LIMIT 1";
	sqlite3_stmt *stmt;
	int rc;
	Producto producto;

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
//		return NULL;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW) {
		fprintf(stderr, "Error ejecutando statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
//		return NULL;
	}

	strncpy(producto.cod_p, sqlite3_column_text(stmt, 0),
			sizeof(producto.cod_p));
	strncpy(producto.nombre, sqlite3_column_text(stmt, 1),
			sizeof(producto.nombre));
	strncpy(producto.descripcion, sqlite3_column_text(stmt, 2),
			sizeof(producto.descripcion));
	producto.cantidad = sqlite3_column_int(stmt, 3);
	producto.precio = sqlite3_column_double(stmt, 4);
	producto.tipo = sqlite3_column_int(stmt, 5);

	sqlite3_finalize(stmt);
	return producto;
}

int numeroProductosCategoria(sqlite3 *db) {
	char sql[100];
	sqlite3_stmt *stmt;
	int rc;
	int numMesas = 0, numSillas = 0, numSofas = 0;

	//Contamos los productos de cada categoria
	sprintf(sql, "SELECT COUNT(*) FROM producto WHERE tipo = %d", MESAS);
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement para contar mesas: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		numMesas = sqlite3_column_int(stmt, 0);
	} else {
		fprintf(stderr, "Error ejecutando statement para contar mesas: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);

	sprintf(sql, "SELECT COUNT(*) FROM producto WHERE tipo = %d", SILLAS);
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement para contar sillas: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		numSillas = sqlite3_column_int(stmt, 0);
	} else {
		fprintf(stderr, "Error ejecutando statement para contar sillas: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);

	sprintf(sql, "SELECT COUNT(*) FROM producto WHERE tipo = %d", SOFAS);
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement para contar sofás: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		numSofas = sqlite3_column_int(stmt, 0);
	} else {
		fprintf(stderr, "Error ejecutando statement para contar sofás: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);

	//Imprimimos los resultados
	printf("Productos de la categoría MESAS: %d\n", numMesas);
	printf("Productos de la categoría SILLAS: %d\n", numSillas);
	printf("Productos de la categoría SOFAS: %d\n", numSofas);

	return 0;
}

int productoMayorCantidad(sqlite3 *db) {
	char sql[] = "SELECT * FROM producto ORDER BY cantidad DESC LIMIT 1";
	sqlite3_stmt *stmt;
	int rc;
	Producto producto;

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW) {
		fprintf(stderr, "Error ejecutando statement: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}

	strncpy(producto.cod_p, sqlite3_column_text(stmt, 0),
			sizeof(producto.cod_p));
	strncpy(producto.nombre, sqlite3_column_text(stmt, 1),
			sizeof(producto.nombre));
	strncpy(producto.descripcion, sqlite3_column_text(stmt, 2),
			sizeof(producto.descripcion));
	producto.cantidad = sqlite3_column_int(stmt, 3);
	producto.precio = sqlite3_column_double(stmt, 4);
	producto.tipo = sqlite3_column_int(stmt, 5);

	//Imprimimos los datos del producto con mayor cantidad
	printf("[Código: %s, ", producto.cod_p);
	printf("Nombre: %s, ", producto.nombre);
	printf("Descripción: %s, ", producto.descripcion);
	printf("Cantidad: %d, ", producto.cantidad);
	printf("Precio: %.2f, ", producto.precio);

	// Imprimimos el nombre de la categoría según su valor en la enumeración
	switch (producto.tipo) {
	case MESAS:
		printf("Categoria: MESAS]\n");
		break;
	case SILLAS:
		printf("Categoria: SILLAS]\n");
		break;
	case SOFAS:
		printf("Categoria: SOFAS]\n");
		break;
	default:
		printf("Categoria: Desconocida]\n");
		break;
	}

	sqlite3_finalize(stmt);
	return 0;
}

int volcarAListaProductosBD(sqlite3 *db, ListaProductos *lp) {
	char sql[200];
	int rc, n;
	sqlite3_stmt *stmt;

	sprintf(sql, "SELECT COUNT(*) FROM producto");
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error preparando statement para contar mesas: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		n = sqlite3_column_int(stmt, 0);
		lp->numProductos = 0;
		lp->aProductos = (Producto*) malloc(n * sizeof(Producto));
		if (lp->aProductos == NULL) {
			fprintf(stderr, "Error al asignar memoria para aProductos\n");
			sqlite3_finalize(stmt);
			return -1;
		}
		sqlite3_finalize(stmt);
		sprintf(sql, "SELECT * FROM producto");
		rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

		if (rc != SQLITE_OK) {
			printf("Error preparando la consulta: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return 1;
		}

		while (sqlite3_step(stmt) == SQLITE_ROW) {
			char *cod_p = (char*) sqlite3_column_text(stmt, 0);
			char *nombre = (char*) sqlite3_column_text(stmt, 1);
			char *descripcion = (char*) sqlite3_column_text(stmt, 2);
			int cantidad = sqlite3_column_int(stmt, 3);
			double precio = sqlite3_column_double(stmt, 4);
			int tipo = sqlite3_column_int(stmt, 5);
			strcpy(lp->aProductos[lp->numProductos].cod_p, cod_p);
			strcpy(lp->aProductos[lp->numProductos].nombre, nombre);
			strcpy(lp->aProductos[lp->numProductos].descripcion, descripcion);
			lp->aProductos[lp->numProductos].cantidad = cantidad;
			lp->aProductos[lp->numProductos].precio = precio;
			lp->aProductos[lp->numProductos].tipo = tipo;
			lp->numProductos++;
		}

	} else {
		fprintf(stderr,
				"Error ejecutando statement para volcar los productos de la BD a la lista de productos: %s\n",
				sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);
}

int mostrarProductosCategoriaBD(sqlite3 *db, int categoria) {
	char *sql;
	switch (categoria) {
	case 0:
		sql = "SELECT * FROM producto WHERE tipo = '0';";
		break;
	case 1:
		sql = "SELECT * FROM producto WHERE tipo = '1';";
		break;
	case 2:
		sql = "SELECT * FROM producto WHERE tipo = '2';";
		break;
	default:
		printf("Categoría no válida\n");
		return 1;
	}

	sqlite3_stmt *stmt;
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	if (rc != SQLITE_OK) {
		printf("Error preparando la consulta: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}

	printf("\n%-10s %-10s %-15s %-10s %-10s %-10s\n", "cod_p", "nombre",
			"descripcion", "cantidad", "precio", "tipo");
	printf(
			"-----------------------------------------------------------------\n");

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		char *cod_p = (char*) sqlite3_column_text(stmt, 0);
		char *nombre = (char*) sqlite3_column_text(stmt, 1);
		char *descripcion = (char*) sqlite3_column_text(stmt, 2);
		int cantidad = sqlite3_column_int(stmt, 3);
		double precio = sqlite3_column_double(stmt, 4);
		char *tipo = (char*) sqlite3_column_text(stmt, 5);

		printf("%-10s %-10s %-15s %-10d %-10.2f %-10s\n", cod_p, nombre,
				descripcion, cantidad, precio, tipo);
	}

	sqlite3_finalize(stmt);
	return 0;
}

int devolverProductoBD(sqlite3 *db, Producto p) {
	char sql[200];
	sqlite3_stmt *stmt;

	sprintf(sql,
			"UPDATE producto SET cantidad = (cantidad+1) WHERE cod_p = '%s';",
			p.cod_p);
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("Error preparando la consulta: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return 0;

}
