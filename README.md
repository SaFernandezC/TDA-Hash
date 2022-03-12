## TDA Hash

**Alumno:** Santiago Fernandez Caruso

**Materia:** Algoritmos y programacion II

**Catedra:** Mendez

------

El trabajo prensentado consta de la implementacion del TDA Hash. Un hash es una estructura de datos que se utiliza para almacenar elementos en una determinada posicion. La posicion de cada elemento esta dada por una clave que identifica a dicho elemento. La implementacion realizada es la de un Hash abierto por lo que la tabla donde se almacenan los datos no es un vector sino que es un "vector" de listas. 
  Al almacenar los elementos en listas, aunque se produzcan colisiones, nunca va a faltar espacio de almacenamiento, pero esta no es la idea del Hash, por eso cuando el factor de carga es mayor a 1 (cuando hay igual cantidad de elementos que de listas) se agranda el tamanio del vector y se pasa por la funcion de Hasheo a cada clave para volver a reinsertarla en otra posicion. Lo ideal es tener un elemento por lista para poder obtener los mejores rendimientos. La complejidad de un hash es una complejidad constante, con una buena funcion de Hasheo y teniendo solo un elemento por posicion se puede obtener una complejidad de O(1) para insertar, buscar y borrar elementos.

Aclaraciones sobre la implementacion:

- El umbral maximo de carga elegido es 1.
- Cuando se crea el Hash y cuando se rehashea se crean las listas para todas las posiciones.
- El iterador externo esta implementado en el mismo .c que el Hash.

------

### Pasos a seguir para compilar y ejecutar el programa 		


Para compilar el programa basta con abrir la terminal en el directorio donde se encuentran los archivos del programa y ejecutar el siguiente comando:
		

	gcc *.c -o "nombre_ejecutable"

donde "nombre_ejecutable" puede ser a elección y luego para ejecutar el programa:

	./"nombre_ejecutable"

Tambien se puede compilar pasandole Flags mas estrictos al compilador para detectar mas "errores" como por ejemplo -Wall -Werror -Wconversion, un ejemplo de compilacion y ejecucion podria ser:

	Compilacion ->	gcc *.c -Wall -Werror -Wconversion -std=c99 -o hash
	
	Ejecucion -> ./hash

Aclaracion: Se puede correr el programa con Valgrind para detectar posibles fallas con:

	valgrind ./hash o, para mas detalles:
	
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./hash


