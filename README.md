# Practica-Radix
 Práctica 5 Arquitecturas Avanzadas

El objetivo de esta práctica es implementar el algoritmo Radix sort con la librería de Intel TBB.
Para ello, se ha utilizado de base el ejemplo dado en clase que se puede encontrar en el github del tema de programación paralela bajo el nombre 6_Example_PackingProblem. La implementación del código se ha dividido en 3 fases, tal como se encontraba en el ejemplo. 

La primera fase consiste en, dados un vector de entrada y una máscara, devolver un vector que tuviese 1's en las posiciones cuyo bit indicado por la máscara sea 1 y devolver 0's en el resto de posiciones.

Ejemplo: 
- Máscara : 1
- Vector de entrada: [1, 2, 2, 3, 7, 12]
- Vector de salida:  [1, 0, 0, 1, 1, 0]

La segunda fase consiste en, dado el vector de salida anterior, aplicar el scan. Esto quiere decir ir acumulando los resultados posición a posición.

Ejemplo: 
- Vector de entrada:  [1, 0, 0, 1, 1, 0]
- Vector de salida: [1, 1, 1, 2, 3, 3]

El tercer y último paso, y posiblemente el más complicado, consiste en, a partir de los vectores de salida de las fases 1 y 2 y del vector inicial, ordenar el vector.
Aquí, se diferenciarán 2 casos. Por un lado aquellas posiciones en que el vector de salida tenga un 0 y aquellas que tenga un 1. Las que tengan un 1 serán más fáciles de trabajar, pues el resultado es el mismo que el dado por el ejemplo del código. En nuestro caso, sería:

Ejemplo:
- Vectores de entrada: [1, 2, 2, 3, 7, 12], [1, 0, 0, 1, 1, 0], [1, 1, 1, 2, 3, 3]
- Vector de salida posiciones 1s: [1, 3, 7]

Para calcular el resto de posiciones, se deberá llevar acabo una pequeña ecuación que se puede observar y entender más fácilmente en el código. Pero la idea que usa es tomar los valores del vector inicial que equivalen a 0s en el vector resultado de la fase 1 y ponerlos por orden a partir del último valor que equivale a un 1. Por lo que obtendríamos:
- Vector de salida final: [1, 3, 7, 2, 2, 12]

Así pues, estas 3 fases serían el ciclo que sigue el algoritmo. Este ciclo se llevará a cabo tantas veces como dígitos tengan nuestros elementos del vector inicial. De ciclo a ciclo lo único que cambiará es el vector de entrada inicial, que será el vector de salida final del ciclo anterior, y la máscara de la fase 1, que pasará a ser el siguiente dígito.
