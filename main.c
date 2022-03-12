#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "hash_iterador.h"
#include "pruebas.h"

#define ERROR -1
#define EXITO 0
#define CANTIDAD_CARACTERES 256

/*--------------------------------------------Funciones usadas en pruebas------------------------------------------------------------*/

void destruir_alumnos(void* elemento){
  if(!elemento)
    return;
  free(elemento);
}

char* nombre_alumno(const char* nombre){
  if(!nombre)
    return NULL;

  char* aux = malloc(strlen(nombre)+1);
  strcpy(aux,nombre);
  return aux;
}

void crear_clave_insertar(char* clave, int numero){
  char palabra[3];
  palabra[0] = (char)(numero/CANTIDAD_CARACTERES+1);
  palabra[1] = (char)(numero);
  palabra[2] = '\0';
  strcpy(clave, palabra);
}

bool iterar_todo(hash_t* hash, const char* clave, void* aux){
  return false;
}

bool iterar_cuatro(hash_t* hash, const char* clave, void* aux){
  if(*(int*)aux < 3){
    *(int*)aux += 1;
    return false;
  }
  return true;
}

/*---------------------------------------------------Pruebas----------------------------------------------------------------*/

void pruebas_crear(prueba_t* tester){
  printf("\n\n----------------------------------------------PRUEBAS CREAR-----------------------------------------------\n\n");
  {
    hash_t* hash = hash_crear(destruir_alumnos, 1);
    prueba_afirmar(tester, "Creo un Hash correctamente", hash != NULL);
    prueba_afirmar(tester, "El hash se crea vacio", hash_cantidad(hash) == 0);
    hash_destruir(hash);
  }
  {
    hash_t* hash = hash_crear(NULL, 1);
    prueba_afirmar(tester, "Creo un Hash con destructor NULL", hash != NULL);
    hash_destruir(hash);
  }
  {
    hash_t* hash1 = hash_crear(NULL, 1);
    hash_t* hash2 = hash_crear(NULL, 10);
    hash_t* hash3 = hash_crear(NULL, 1000);
    prueba_afirmar(tester, "Creo un Hash con capacidad 1", hash1 != NULL);
    prueba_afirmar(tester, "Creo un Hash con capacidad 10", hash2 != NULL);
    prueba_afirmar(tester, "Creo un Hash con capacidad 1000", hash3 != NULL);
    hash_destruir(hash1);
    hash_destruir(hash2);
    hash_destruir(hash3);

  }
}

void pruebas_null(prueba_t* tester) {
  printf("\n\n----------------------------------------------PRUEBAS NULL-----------------------------------------------\n\n");

  hash_t* hash = hash_crear(NULL,1);
  {
    prueba_afirmar(tester, "Inserto una calve NULL en un Hash valido y devuelve -1", hash_insertar(hash,NULL,NULL) == ERROR);
    prueba_afirmar(tester, "Inserto una clave vacia en un Hash valido y devuelve -1", hash_insertar(hash, "", NULL) == ERROR);
    prueba_afirmar(tester, "Inserto una clave valida en un Hash NULL y devuelve -1", hash_insertar(NULL, "aaa", NULL) == ERROR);
    prueba_afirmar(tester, "Inserto una clave NULL en un Hash NULL y devuelve -1", hash_insertar(NULL, NULL, NULL) == ERROR);
    prueba_afirmar(tester, "Inserto un elemento NULL con Hash y clave validos y devuelve 0", hash_insertar(hash,"a", NULL) == EXITO);
    prueba_afirmar(tester, "Busco un elemento en un Hash NULL con clave valida y devuelve NULL", hash_obtener(NULL, "asd") == NULL);
    prueba_afirmar(tester, "Busco un elemento en un Hash valido con una clave NULL y devuelve NULL", hash_obtener(hash, NULL) == NULL);
    prueba_afirmar(tester, "'hash_contiene' devuelve false con Hash NULL", hash_contiene(NULL, "asd") == false);
    prueba_afirmar(tester, "'hash_contiene' devuelve false con Clave NULL", hash_contiene(hash, NULL) == false);
    prueba_afirmar(tester, "Un Hash NULL se considera vacio", hash_cantidad(NULL) == 0);
    prueba_afirmar(tester, "Iterador interno devuelve 0 con Hash NULL", hash_con_cada_clave(NULL, NULL, NULL) == 0);
    prueba_afirmar(tester, "Iterador interno devuelve 0 con Hash valido y funcion NULL", hash_con_cada_clave(hash, NULL, NULL) == 0);
    prueba_afirmar(tester, "No puedo crear un iterador externo con un Hash NULL", hash_iterador_crear(NULL) == NULL);
  }
  hash_destruir(hash);
}

void pruebas_insertar(prueba_t* tester){

  printf("\n\n----------------------------------------------PRUEBAS INSERTAR-----------------------------------------------\n\n");
  char* nombre1 = nombre_alumno("Santiago");
  char* nombre2 = nombre_alumno("Marcos");
  char* nombre3 = nombre_alumno("Pepe");
  char* nombre4 = nombre_alumno("Pepito");

  {
    hash_t* hash = hash_crear(NULL, 3);
    prueba_afirmar(tester, "Creo un Hash con capacidad 3 correctamente", hash != NULL);
    prueba_afirmar(tester, "Guardo el padron 105267", hash_insertar(hash, "105267", nombre1) == EXITO);
    prueba_afirmar(tester, "Guardo el padron 102589", hash_insertar(hash, "102589", nombre2) == EXITO);
    prueba_afirmar(tester, "Guardo el padron 100483", hash_insertar(hash, "100483", nombre3) == EXITO);
    prueba_afirmar(tester, "Guardo el padron 53482", hash_insertar(hash, "53482", nombre4) == EXITO);
    prueba_afirmar(tester, "El hash tiene 4 elementos", hash_cantidad(hash) == 4);
    prueba_afirmar(tester, "Busco el padron 105267", hash_obtener(hash ,"105267") == nombre1);
    prueba_afirmar(tester, "Busco el padron 102589", hash_obtener(hash ,"102589") == nombre2);
    prueba_afirmar(tester, "Busco el padron 100483", hash_obtener(hash ,"100483") == nombre3);
    prueba_afirmar(tester, "Busco el padron 53482", hash_obtener(hash ,"53482") == nombre4);

    hash_destruir(hash);
  }

  printf("\n\n----------------------------------------------INSERTAR REPETIDOS-----------------------------------------------\n\n");
  {
    printf("\n");
    hash_t* hash = hash_crear(NULL, 3);
    prueba_afirmar(tester, "Guardo una clave/valor nueva", hash_insertar(hash, "105267", nombre1) == EXITO);
    prueba_afirmar(tester, "Se guardo el elemento correcto", hash_obtener(hash, "105267") == nombre1);
    prueba_afirmar(tester, "Guardo otro valor para la misma clave", hash_insertar(hash, "105267", nombre2) == EXITO);
    prueba_afirmar(tester, "El valor de la clave se actualizo correctamente", hash_obtener(hash, "105267") == nombre2);
    prueba_afirmar(tester, "Elimino la clave", hash_quitar(hash, "105267") == EXITO);
    printf("\n");
    prueba_afirmar(tester, "Guardo otra clave/valor nueva", hash_insertar(hash, "55642", nombre3) == EXITO);
    prueba_afirmar(tester, "Se guardo el elemento correcto", hash_obtener(hash, "55642") == nombre3);
    prueba_afirmar(tester, "Guardo otro valor para la misma clave", hash_insertar(hash, "55642", nombre2) == EXITO);
    prueba_afirmar(tester, "El valor de la clave se actualizo correctamente", hash_obtener(hash, "55642") == nombre2);
    prueba_afirmar(tester, "Guardo otro valor para la misma clave", hash_insertar(hash, "55642", nombre1) == EXITO);
    prueba_afirmar(tester, "El valor de la clave se actualizo correctamente", hash_obtener(hash, "55642") == nombre1);
    prueba_afirmar(tester, "Elimino la clave", hash_quitar(hash, "55642") == EXITO);

    prueba_afirmar(tester, "Destruyo el Hash", true);
    hash_destruir(hash);
  }

  free(nombre1);
  free(nombre2);
  free(nombre3);
  free(nombre4);
}

void pruebas_quitar(prueba_t* tester) {

    printf("\n\n----------------------------------------------PRUEBAS QUITAR-----------------------------------------------\n\n");
    char* nombre1 = nombre_alumno("Santiago");
    char* nombre2 = nombre_alumno("Marcos");
    char* nombre3 = nombre_alumno("Manuel");

    {
      hash_t* hash = hash_crear(destruir_alumnos, 3);
      prueba_afirmar(tester, "Creo un Hash con capacidad 3 correctamente", hash != NULL);
      hash_insertar(hash, "105267", nombre1);
      hash_insertar(hash, "102589", nombre2);
      hash_insertar(hash, "100483", nombre3);

      prueba_afirmar(tester, "Inserto 3 claves correctamente", hash_cantidad(hash) == 3);
      prueba_afirmar(tester, "Elimino una de las claves", hash_quitar(hash, "105267") == EXITO);
      prueba_afirmar(tester, "El hash queda con 2 elementos", hash_cantidad(hash) == 2);
      prueba_afirmar(tester, "Elimino otra de las claves", hash_quitar(hash, "102589") == EXITO);
      prueba_afirmar(tester, "El hash queda con 1 elemento", hash_cantidad(hash) == 1);
      prueba_afirmar(tester, "Elimino una clave que no existe y devuelve -1", hash_quitar(hash, "105267") == ERROR);
      prueba_afirmar(tester, "Elimino la ultima clave", hash_quitar(hash, "100483") == EXITO);
      prueba_afirmar(tester, "El hash queda vacio", hash_cantidad(hash) == 0);
      prueba_afirmar(tester, "Destruyo el hash", true);

      hash_destruir(hash);
    }

}

void pruebas_volumen(prueba_t* tester){

  printf("\n\n----------------------------------------------PRUEBAS VOLUMEN-----------------------------------------------\n\n");
  {

    int estado = EXITO;

    char* clave = calloc(3,sizeof(char));
    hash_t* hash = hash_crear(NULL, 3);
    prueba_afirmar(tester, "Creo un Hash de capcidad 3", hash != NULL);

    for (int i = 0; i < 350 && estado == EXITO; i++) {
      crear_clave_insertar(clave, i);
      estado = hash_insertar(hash, clave, NULL);
    }

    prueba_afirmar(tester, "Se insertaron 350 claves", estado == EXITO);
    prueba_afirmar(tester, "El hash tiene 350 elementos", hash_cantidad(hash) == 350);

    for(int i = 250; i < 350 && estado == EXITO; i++){
      crear_clave_insertar(clave, i);
      estado = hash_quitar(hash, clave);
    }

    prueba_afirmar(tester, "Elimino del hash las 100 ultimas claves", estado == EXITO);
    prueba_afirmar(tester, "El hash queda con 250 elementos", hash_cantidad(hash) == 250);

    for (int i = 0; i < 100 && estado == EXITO; i++) {
      crear_clave_insertar(clave, i);
      estado = hash_insertar(hash, clave, NULL);
    }

    prueba_afirmar(tester, "Vuelvo a insertar 100 claves que ya estan en el hash", estado == EXITO);
    prueba_afirmar(tester, "La cantidad sigue siendo 250", hash_cantidad(hash) == 250);

    for (int i = 300; i < 400 && estado == EXITO; i++) {
      crear_clave_insertar(clave, i);
      estado = hash_insertar(hash, clave, NULL);
    }

    prueba_afirmar(tester, "Inserto 100 claves nuevas", estado == EXITO);
    prueba_afirmar(tester, "El hash vuelve a tener 350 elementos", hash_cantidad(hash) == 350);


    free(clave);
    hash_destruir(hash);
    prueba_afirmar(tester, "Destruyo el Hash", true);
  }
}

void pruebas_iterador_externo(prueba_t* tester){
  printf("\n\n-------------------------------------------PRUEBAS ITERADOR EXTERNO--------------------------------------------\n\n");

  {

    int cantidad = 0;

    hash_t* hash = hash_crear(NULL, 5);
    prueba_afirmar(tester, "Creo un hash", hash != NULL);

    printf("Inserto elementos\n");
    hash_insertar(hash, "105267", NULL);
    hash_insertar(hash, "102261", NULL);
    hash_insertar(hash, "100041", NULL);
    prueba_afirmar(tester, "El hash tiene todos los elementos", hash_cantidad(hash) == 3);

    hash_iterador_t* it = hash_iterador_crear(hash);
    prueba_afirmar(tester, "Creo el iterador", it != NULL);

    while(hash_iterador_tiene_siguiente(it)){
      hash_iterador_siguiente(it);
      cantidad += 1;
    }

    prueba_afirmar(tester, "Se recorren todos los elementos del hash", cantidad == 3);
    hash_iterador_destruir(it);
    hash_destruir(hash);
    prueba_afirmar(tester, "Destruyo el iterador", true);
    prueba_afirmar(tester, "Destruyo el hash", true);
  }
}

void pruebas_iterador_interno(prueba_t* tester) {

  printf("\n\n----------------------------------------------PRUEBAS ITERADOR INTERNO-----------------------------------------------\n\n");

  {
    hash_t* hash = hash_crear(NULL, 5);
    prueba_afirmar(tester, "Creo un hash", hash != NULL);
    printf("Inserto elementos\n");
    hash_insertar(hash, "105267", NULL);
    hash_insertar(hash, "102261", NULL);
    hash_insertar(hash, "100041", NULL);
    hash_insertar(hash, "105041", NULL);
    hash_insertar(hash, "98541", NULL);
    hash_insertar(hash, "025464", NULL);

    prueba_afirmar(tester, "Se insertaron todos los elementos", hash_cantidad(hash) == 6);


    size_t iteraciones = hash_con_cada_clave(hash, iterar_todo, NULL);
    prueba_afirmar(tester, "Recorro todos los elementos con el iterador interno", iteraciones == 6);

    int contador = 0;
    iteraciones = hash_con_cada_clave(hash, iterar_cuatro, &contador);
    prueba_afirmar(tester, "Recorro 4 elementos con el iterador interno", iteraciones == 4);

    printf("Borro dos elementos\n");
    hash_quitar(hash,"98541");
    hash_quitar(hash,"105267");
    iteraciones = hash_con_cada_clave(hash, iterar_todo, NULL);
    prueba_afirmar(tester, "Vuelvo a recorrer todos los elementos", iteraciones == 4);

    hash_destruir(hash);
    prueba_afirmar(tester, "Destruyo el Hash", true);
  }
}

int main() {

  prueba_t* tester = prueba_crear();
  if(!tester)
    return 0;

  pruebas_crear(tester);
  pruebas_null(tester);
  pruebas_insertar(tester);
  pruebas_quitar(tester);
  pruebas_volumen(tester);
  pruebas_iterador_externo(tester);
  pruebas_iterador_interno(tester);

  imprimir_estadisticas(tester);
  prueba_destruir(tester);
  return 0;
}
