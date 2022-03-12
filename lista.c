#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

#define ERROR -1
#define EXITO 0
#define LISTA_VACIA 0
#define PRIMER_ELEMENTO 0


//RECORDATORIOS--->Ver si se puede chequear que no se llame a la lista mientras hay un iterador

typedef struct nodo{
  void* dato;
  struct nodo* siguiente;
}nodo_t;

struct lista{
  nodo_t* primer_elemento;
  nodo_t* ultimo_elemento;
  size_t cantidad_elementos;
};

struct lista_iterador{
  nodo_t* nodo_actual;
  lista_t* lista;
  size_t indice;
};

/*--------------------------------------------------FUNCIONES GENERALES------------------------------------------------------------------*/

/*
 * Crea un nodo reservando la memoria necesaria y asignadole el elemento ingresado.
 * Devuelve un puntero al nodo creado o NULL en caso de error.
 */
nodo_t* crear_nuevo_nodo(void* elemento){
  nodo_t* nuevo = calloc(1, sizeof(nodo_t));
  nuevo->dato = elemento;
  return nuevo;
}

/*
 * Recibe una lista y una posicion, itera los nodos hasta llegar al
 * nodo de la posicion ingresada y devuelve un puntero a dicho nodo.
 * Ya viene chequeado que posicion exista al igual que la lista.
 */
nodo_t* buscar_posicion(lista_t* lista, int posicion){
  nodo_t* actual = lista->primer_elemento;
  int i = 0;
  while (i < posicion) {
    actual = actual->siguiente;
    i++;
  }
  return actual;
}

/*
 * Funcion que recibe los 3 parametros validos y borra el primer elemento de
 * la lista.
 */
int borrar_primero(lista_t* lista, nodo_t* actual, nodo_t* auxiliar){
  auxiliar = actual;
  lista->primer_elemento = actual->siguiente;
  free(auxiliar);
  lista->cantidad_elementos--;
  return EXITO;
}

/*
 * Funcion que recibe los 2 parametros validos e inserta el nodo recibido
 * en la primera posicion de la lista.
 */
int insertar_primero(lista_t* lista, nodo_t* nuevo_nodo){
  nuevo_nodo->siguiente = lista->primer_elemento;
  lista->primer_elemento = nuevo_nodo;
  lista->cantidad_elementos++;
  return EXITO;
}

/*--------------------------------------------------FUNCIONES LISTA------------------------------------------------------------------*/

lista_t* lista_crear(){
  lista_t* lista = calloc(1, sizeof(lista_t));
  return lista;
}

int lista_insertar(lista_t* lista, void* elemento){
  if (!lista) {
    return ERROR;
  }

  nodo_t* nuevo_nodo = crear_nuevo_nodo(elemento);
  if (!nuevo_nodo) {
    return ERROR;
  }

  if (lista->cantidad_elementos == LISTA_VACIA) {
    lista->primer_elemento = nuevo_nodo;
  } else {
    lista->ultimo_elemento->siguiente = nuevo_nodo;
  }

  lista->ultimo_elemento = nuevo_nodo;
  lista->cantidad_elementos++;
  return EXITO;
}

int lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){

  if (!lista) {
    return ERROR;
  }

  if (posicion >= lista->cantidad_elementos || lista->cantidad_elementos == LISTA_VACIA) {
    return lista_insertar(lista, elemento);
  }

  nodo_t* nuevo_nodo = crear_nuevo_nodo(elemento);
  if (!nuevo_nodo) {
    return ERROR;
  }
  nodo_t* actual = buscar_posicion(lista, (int)(posicion-1));

  if (posicion == PRIMER_ELEMENTO) {
    return insertar_primero(lista, nuevo_nodo);
  }

  nuevo_nodo->siguiente = actual->siguiente;
  actual->siguiente = nuevo_nodo;

  lista->cantidad_elementos++;
  return EXITO;
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
  if (!lista) {
    return NULL;
  }

  if (posicion >= lista->cantidad_elementos) {
    return NULL;
  }

  nodo_t* actual = buscar_posicion(lista, (int)posicion);
  return actual->dato;
}

int lista_borrar(lista_t* lista){
  if (!lista || lista->cantidad_elementos == LISTA_VACIA) {
    return ERROR;
  }

  nodo_t* actual = buscar_posicion(lista, (int)(lista->cantidad_elementos - 2));
  lista->ultimo_elemento = actual;

  if (lista->cantidad_elementos == 1) {
    free(lista->primer_elemento);
  } else {
    free(actual->siguiente);
    actual->siguiente = NULL;
  }

  lista->cantidad_elementos--;
  return EXITO;
}

int lista_borrar_de_posicion(lista_t* lista, size_t posicion){
  if (!lista || lista->cantidad_elementos == LISTA_VACIA) {
    return ERROR;
  }

  if (posicion >= lista->cantidad_elementos) {
    return lista_borrar(lista);
  }

  nodo_t* auxiliar = NULL;
  nodo_t* actual = buscar_posicion(lista, (int)(posicion-1));

  if (posicion == PRIMER_ELEMENTO) {
    return borrar_primero(lista, actual, auxiliar);
  }

  auxiliar = actual->siguiente;
  actual->siguiente = auxiliar->siguiente;
  free(auxiliar);

  lista->cantidad_elementos--;
  return EXITO;
}

void* lista_ultimo(lista_t* lista){
  if (!lista || lista->cantidad_elementos == LISTA_VACIA) {
    return NULL;
  }
  return lista->ultimo_elemento->dato;
}

bool lista_vacia(lista_t* lista){
  if (!lista || lista->cantidad_elementos == LISTA_VACIA) {
    return true;
  }
  return false;
}

size_t lista_elementos(lista_t* lista){
  if (!lista) {
    return LISTA_VACIA;
  }
  return lista->cantidad_elementos;
}

/*--------------------------------------------------FUNCIONES PILA------------------------------------------------------------------*/

int lista_apilar(lista_t* lista, void* elemento){
  if (!lista) {
    return ERROR;
  }
  return lista_insertar(lista, elemento);
}

int lista_desapilar(lista_t* lista){
  if (!lista) {
    return ERROR;
  }
  return lista_borrar(lista);
}

void* lista_tope(lista_t* lista){
  if (!lista) {
    return NULL;
  }
  return lista_ultimo(lista);
}

/*--------------------------------------------------FUNCIONES COLA------------------------------------------------------------------*/

int lista_encolar(lista_t* lista, void* elemento){
  if (!lista) {
    return ERROR;
  }
  return lista_insertar(lista,elemento);
}

int lista_desencolar(lista_t* lista){
  if (!lista) {
    return ERROR;
  }
  return lista_borrar_de_posicion(lista, PRIMER_ELEMENTO);
}

void* lista_primero(lista_t* lista){
  if (!lista || lista->cantidad_elementos == LISTA_VACIA) {
    return NULL;
  }
  return lista_elemento_en_posicion(lista, PRIMER_ELEMENTO);
}

/*--------------------------------------------------FUNCIONES ITERADOR------------------------------------------------------------------*/

lista_iterador_t* lista_iterador_crear(lista_t* lista){
  if (!lista) {
    return NULL;
  }

  lista_iterador_t* it = malloc(sizeof(lista_iterador_t));
  if (!it) {
    return NULL;
  }

  it->lista = lista;
  it->nodo_actual = lista->primer_elemento;
  it->indice = 0;
  return it;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
  if (!iterador) {
    return false;
  }
  return iterador->indice < lista_elementos(iterador->lista);
}

void* lista_iterador_siguiente(lista_iterador_t* iterador){
  if (!iterador || !lista_iterador_tiene_siguiente(iterador)) {
    return false;
  }

  void* elemento =  iterador->nodo_actual->dato;
  iterador->nodo_actual = iterador->nodo_actual->siguiente;
  iterador->indice++;

  return elemento;
}

void lista_iterador_destruir(lista_iterador_t* iterador){
  free(iterador);
}

void lista_con_cada_elemento(lista_t* lista, void (*funcion)(void*, void*), void *contexto){
  if (!lista || !funcion || !contexto) {
    return;
  }

  nodo_t* actual = lista->primer_elemento;
  void* elemento = NULL;

  for (size_t i = 0; i < lista->cantidad_elementos; i++) {
    elemento = actual->dato;
    funcion(elemento, contexto);
    actual = actual->siguiente;
  }
}

/*--------------------------------------------------FUNCIONES DESTRUIR------------------------------------------------------------------*/

void borrar_nodos(nodo_t* nodo){
  if (nodo->siguiente) {
    borrar_nodos(nodo->siguiente);
  }
  free(nodo);
}

void lista_destruir(lista_t* lista){
  if (!lista) {
    return;
  }
  if (lista->cantidad_elementos != LISTA_VACIA) {
    borrar_nodos(lista->primer_elemento);
  }
  free(lista);
}
