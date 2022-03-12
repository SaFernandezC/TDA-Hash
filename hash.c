#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_iterador.h"
#include "hash.h"
#include "lista.h"

#define EXITO 0
#define ERROR -1
#define CAPACIDAD_MINIMA 3
#define UMBRAL_MAX 1
#define VACIO 0
#define IGUALES 0

struct hash{
  lista_t** tabla;
  size_t cantidad_elementos;
  size_t capacidad;
  hash_destruir_dato_t destructor;
};

typedef struct hash_elemento{
  void* valor;
  char* clave;
}hash_elemento_t;

struct hash_iter{
  hash_t* hash;
  size_t claves_leidas;
  size_t indice_lista;
  size_t posicion_en_lista;
};


/*--------------------------------------------PROTOTIPOS NECESARIOS------------------------------------------------------------*/

int rehash(hash_t* hash);
bool poco_espacio(hash_t* hash);
void destruir_elemento(void* hash_elemento, void* hash);
size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux);
void hash_destruir(hash_t* hash);


/*--------------------------------------------FUNCIONES BUSCAR NUM. PRIMO------------------------------------------------------------*/
/*
 * Determina si el numero que recibe por parametro es primo o no.
 */
bool es_primo(int numero){
  bool primo = true;

  if(numero == 2)
    return true;

  if(numero%2 == 0 || numero <= 1)
    return false;

  for(int i=3; i<=numero/2; i+=2){
    if(numero%i == 0)
      primo = false;
  }

  return primo;
}

/*
 * Funcion que verifica si el numero que recibe es primo,
 * si no,  llama a 'es_primo' con cada numero siguiente al que recibe
 * hasta encontrar uno que cumpla.
 */
size_t siguiente_primo(size_t numero){
  if(es_primo((int)numero))
    return numero;
  return numero = siguiente_primo(numero+1);
}


/*--------------------------------------------------FUNCIONES CREAR------------------------------------------------------------------*/
/*
 * Funcion que rencibe un "array" de punteros a listas y el
 * tamanio del mismo, y va inicializando las listas de cada posicion.
 * Si hay algun problema en la creacion de alguna lista, comienza a destruir
 * las listas que llegaron a crearse bien.
 */
int crear_listas(lista_t** tabla, int capacidad){
  bool problemas = false;
  int posicion;

  for(int i = 0; i < capacidad && !problemas; i++){
    tabla[i] = lista_crear();
    if(!tabla[i]){
      problemas = true;
      posicion = i-1;
    }
  }

  if(problemas){ //SI FALLA ALGUN LISTA_CREAR LIBERO LAS LISTAS QUE LLEGARON A CREARSE
    for(int j = posicion; j >= 0; j--){
      lista_destruir(tabla[j]);
    }
  }

  return (problemas==false)?EXITO:ERROR;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_elemento, size_t capacidad){

  hash_t* hash = calloc(1, sizeof(hash_t));
  if(!hash)
    return NULL;

  hash->destructor = destruir_elemento;

  if((int)capacidad < CAPACIDAD_MINIMA){
    hash->capacidad = CAPACIDAD_MINIMA;
  } else hash->capacidad = siguiente_primo(capacidad);

  hash->tabla = calloc(hash->capacidad, sizeof(lista_t*));
  if(!hash->tabla){
    free(hash);
    return NULL;
  }

  if(crear_listas(hash->tabla, (int)hash->capacidad) == ERROR){
    free(hash);
    free(hash->tabla);
    return NULL;
  }

  return hash;
}


/*--------------------------------------------------FUNCION HASH------------------------------------------------------------------*/
/*
 * Recibe una clave (string) y se encarga de transformarla en un
 * numero. El numero depende unicamente del contenido de la clave.
 */
size_t f_hash(const char* string){
  size_t cantidad = 0;

  for(size_t i = 0; i < strlen(string); i++){
    cantidad += (size_t)string[i]*(i+3);
  }
  return cantidad;
}


/*-----------------------------------------FUNCIONES CANT, CONTIENE, OBTENER---------------------------------------------------------*/
/*
 * Funcion que crea un iterador externo para la lista que recibe y compara
 * la clave del elemento que devuelve el iterador con la recibida por parametro.
 * Si las claves coinciden devuelve el elemento asociado a dicha clave,
 * si ninguna coincide devuelve NULL.
 */
hash_elemento_t* obtener_elemento(lista_t* lista, const char* clave, int* posicion){

  hash_elemento_t* elemento = NULL;
  bool seguir = true;
  int contador = 0;

  lista_iterador_t* it = lista_iterador_crear(lista);
  if(!it)
    return NULL;

  while(lista_iterador_tiene_siguiente(it) && seguir){
    elemento = lista_iterador_siguiente(it);
    if(strcmp(elemento->clave, clave) == IGUALES)
      seguir = false;
    contador++;
  }

  lista_iterador_destruir(it);

  if(seguir) //No existe el elemento
    return NULL;

  if(posicion)
    *posicion = contador-1;

  return elemento;
}

void* hash_obtener(hash_t* hash, const char* clave){
  if(!hash || !clave || *clave == VACIO)
    return NULL;

  size_t posicion = f_hash(clave);
  lista_t* lista = hash->tabla[posicion%hash->capacidad];
  hash_elemento_t* elemento = obtener_elemento(lista, clave, NULL);

  if(!elemento)
    return NULL;
  return elemento->valor;
}

bool hash_contiene(hash_t* hash, const char* clave){
  if(!hash || !clave || *clave == VACIO)
    return false;

  size_t posicion = f_hash(clave);
  lista_t* lista = hash->tabla[posicion%hash->capacidad];

  hash_elemento_t* elemento = obtener_elemento(lista, clave, NULL);

  return (elemento == NULL)?false:true;
}

size_t hash_cantidad(hash_t* hash){
  if(!hash)
    return VACIO;
  return hash->cantidad_elementos;
}


/*--------------------------------------------------FUNCIONES INSERTAR------------------------------------------------------------------*/
/*
 * Crea una copia de la clave que recibe por parametro y la retorna.
 */
char* crear_clave(const char* clave){
  char* clave_nueva = malloc(strlen(clave)+1);
  if(!clave_nueva)
    return NULL;

  strcpy(clave_nueva, (char*)clave);
  return clave_nueva;
}

/*
 * Reserva la memoria necesaria para un elemento del hash.
 * Se le asigna el elemento que recibe por parametro y  una copia
 * de la clave. Devuelve el elemento o NULL en caso de error.
 */
hash_elemento_t* crear_elemento(const char* clave, void* elemento){
  hash_elemento_t* nuevo = calloc(1, sizeof(hash_elemento_t));
  if(!nuevo)
    return NULL;

  nuevo->clave = crear_clave(clave);
  if(!nuevo->clave){
    free(nuevo);
    return NULL;
  }

  nuevo->valor = elemento;
  return nuevo;
}

/*
 * Inserta el elemento que recibe en la lista que recibe.
 */
int insertar_elemento(lista_t* lista, hash_elemento_t* elemento){
  return lista_insertar(lista, elemento);
}

/*
 * Determina si ya existe un elemento en el hash con la clave dada.
 * Si el elemento existe le actualiza el valor y devuelve true.
 * Si el elemento no existe devuelve false.
 */
bool esta_repetido(hash_t* hash, lista_t* lista, const char* clave, void* valor_nuevo){

  hash_elemento_t* elemento = obtener_elemento(lista, clave, NULL);
  if(!elemento)
    return false;

  if(hash->destructor)
    hash->destructor(elemento->valor);

  elemento->valor = valor_nuevo;
  return true;
}

int hash_insertar(hash_t* hash, const char* clave, void* elemento){

  if(!hash || !clave || *clave == VACIO)
    return ERROR;

  size_t posicion = f_hash(clave);
  int estado = EXITO;

  if(esta_repetido(hash ,hash->tabla[posicion%hash->capacidad], clave, elemento))
    return EXITO;

  hash_elemento_t* elemento_nuevo = crear_elemento(clave, elemento);
  if(!elemento_nuevo)
    return ERROR;

  estado = insertar_elemento(hash->tabla[posicion%hash->capacidad], elemento_nuevo);
  if(estado == ERROR)
    return ERROR;

  hash->cantidad_elementos++;

  if(poco_espacio(hash))
    estado = rehash(hash);

  if(estado == ERROR)
    return ERROR;
  return EXITO;
}


/*--------------------------------------------------FUNCIONES REHASH------------------------------------------------------------------*/
/*
 * Determina si el factor de carga del hash es mayor o no
 * al maximo permitido.
 */
bool poco_espacio(hash_t* hash){
  float factor_carga = ((float)hash->cantidad_elementos / (float)hash->capacidad);
  if(factor_carga < UMBRAL_MAX)
   return false;
  return true;
}

/*
 * Llama a la funcion hash_insertar (en el hash nuevo) con la clave recibida y el elemento
 * asociado a dicha clave. Devuelve true si hubo errores en el proceso
 * o false si fue exitoso.
 */
bool insertar_nuevo_hash(hash_t* hash, const char* clave, void* nuevo_hash){
  int estado = EXITO;
  hash_t* nuevo = (hash_t*)nuevo_hash;

  estado = hash_insertar(nuevo, clave, hash_obtener(hash,clave));
  if(estado == ERROR)
    return true;
  return false;
}

/*
 * Crea un nuevo Hash con el doble de capacidad que el anterior. Se itera
 * cada clave del hash viejo usando una funcion que inserte las claves en el hash nuevo.
 * Se hace un swap y se libera el hash que no se sigue utilizando.
 * Devuelve 0 en caso de exito o -1 en caso de error.
 */
int rehash(hash_t* hash){
  hash_t* nuevo = hash_crear(hash->destructor, siguiente_primo((hash->capacidad)*2));
  if(!nuevo)
    return ERROR;

  size_t capacidad = hash_con_cada_clave(hash, insertar_nuevo_hash, nuevo);
  if(capacidad != hash->capacidad){
    hash_destruir(nuevo);
    return ERROR;
  }

  hash_t aux = *hash;
  *hash = *nuevo;
  *nuevo = aux;
  nuevo->destructor = NULL;
  hash_destruir(nuevo);

  return EXITO;
}


/*--------------------------------------------------FUNCIONES QUITAR------------------------------------------------------------------*/
/*
 * Busca en la lista el elemento a borrar con la clave que recibe,
 * y lo destruye. Tambien borra la posicion de la lista donde se encontraba
 * el elemento. Reduce en uno la cantidad de elementos del hash.
 * Devuelve 0 en caso de exito o -1 en caso de error.
 */
int borrar_elemento(hash_t* hash, const char* clave, lista_t* lista) {

  int posicion = 0;
  int estado = EXITO;

  hash_elemento_t* elemento = obtener_elemento(lista, clave, &posicion);
  if(!elemento)
    return ERROR;

  destruir_elemento(elemento, hash);

  estado = lista_borrar_de_posicion(lista, (size_t)posicion);
  if(estado == ERROR)
    return estado;

  hash->cantidad_elementos--;
  return estado;
}

int hash_quitar(hash_t* hash, const char* clave){
  if(!hash || !clave)
    return ERROR;

  size_t posicion = f_hash(clave);

  return borrar_elemento(hash, clave, hash->tabla[posicion%hash->capacidad]);
}


/*--------------------------------------------------FUNCIONES ITERADOR------------------------------------------------------------------*/
/*
 * Crea un iterador para la lista que recibe e itera la misma llamando con cada
 * elemento a la funcion que recibe por parametro, pasandole la clave del elemento y
 * el auxiliar. Cada vez que se llama a la funcion se aumenta la cantidad de iteraciones.
 * Corta cuando la funcion devuelve true o no hay mas elementos por iterar en la lista.
 */
bool iterar_listas(hash_t* hash, lista_t* lista, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux, size_t* iteraciones){

  bool cortar = false;
  hash_elemento_t* elemento = NULL;

  lista_iterador_t* it = lista_iterador_crear(lista);
  if(!it)
    return false;

  while(lista_iterador_tiene_siguiente(it) && !cortar){
    elemento = lista_iterador_siguiente(it);
    cortar = funcion(hash, elemento->clave, aux);
    *iteraciones += 1;
  }

  lista_iterador_destruir(it);
  return cortar;
}

size_t hash_con_cada_clave(hash_t* hash, bool (*funcion)(hash_t* hash, const char* clave, void* aux), void* aux){
  if(!hash || !funcion)
    return 0;

  bool cortar = false;
  size_t iteraciones = 0;
  int i = 0;

  while(i < hash->capacidad && !cortar){
    cortar = iterar_listas(hash, hash->tabla[i], funcion, aux, &iteraciones);
    i++;
  }
  return iteraciones;
}


/*--------------------------------------------------FUNCIONES DESTRUIR------------------------------------------------------------------*/
/*
 * Si existe, invoca al destructor mandandole el valor el elemento.
 * Libera la clave y el elemento.
 */
void destruir_elemento(void* hash_elemento, void* hash){
  hash_elemento_t* aux = ((hash_elemento_t*)hash_elemento);
  hash_t* hash_aux = (hash_t*)hash;

  if(hash_aux->destructor)
    hash_aux->destructor(aux->valor);
  free(aux->clave);
  free(aux);
}

/*
 * Recorre todas las listas del hash y con cada elemento de cada
 * lista llama a 'destruir_elemento', luego destruye la lista.
 */
void destruir_listas(hash_t* hash){
  for (int i = 0; i < hash->capacidad; i++) {
    lista_con_cada_elemento(hash->tabla[i], destruir_elemento, hash);
    lista_destruir(hash->tabla[i]);
  }
}

void hash_destruir(hash_t* hash){
  if(!hash)
    return;
  destruir_listas(hash);
  free(hash->tabla);
  free(hash);
}


/*--------------------------------------------------FUNCIONES ITERADOR------------------------------------------------------------------*/
hash_iterador_t* hash_iterador_crear(hash_t* hash){
  if(!hash)
    return NULL;

  hash_iterador_t* it = calloc(1, sizeof(hash_iterador_t));
  if(!it)
    return NULL;

  it->hash = hash;
  return it;
}

bool hash_iterador_tiene_siguiente(hash_iterador_t* iterador){
  if(!iterador)
    return false;
  return iterador->claves_leidas < iterador->hash->cantidad_elementos;
}

/*
 * Busca un elemento en la lista que le llega. Si el elemento es el ultimo de la lista
 * aumenta el indice_lista y vuelve a 0 la posicion_en_lista. Si no, aumenta solo la
 * posicion_en_lista para seguir buscando en la misma lista la proxima iteracion.
 * Aumenta en 1 la cantidad de claves leidas. Devuelve la clave del elemento.
 */
const char* buscar_clave(hash_iterador_t* iterador, lista_t* lista){

   hash_elemento_t* elemento = lista_elemento_en_posicion(lista, iterador->posicion_en_lista);

   if(iterador->posicion_en_lista >= lista_elementos(lista)-1){
     iterador->indice_lista++;
     iterador->posicion_en_lista = 0;
   } else iterador->posicion_en_lista++;

   iterador->claves_leidas++;
   return elemento->clave;
 }

const char* hash_iterador_siguiente(hash_iterador_t* iterador){
  if(!iterador || !hash_iterador_tiene_siguiente(iterador))
    return NULL;

  const char* clave = NULL;
  lista_t* lista_actual = NULL;
  size_t i = iterador->indice_lista;

  while(i < iterador->hash->capacidad && clave == NULL){
    lista_actual = iterador->hash->tabla[i];
    if(!lista_vacia(lista_actual))
      clave = buscar_clave(iterador, lista_actual);
    i++;
    if(!clave)
      iterador->indice_lista = i;
  }

  return clave;
}

void hash_iterador_destruir(hash_iterador_t* iterador){
  if(iterador)
    free(iterador);
}
