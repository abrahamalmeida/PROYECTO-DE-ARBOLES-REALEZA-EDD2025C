# PROYECTO-DE-ARBOLES-REALEZA-EDD2025C

# Sistema de Dinastía Real: Lógica de Sucesión

Descripción General del Proyecto:

Este proyecto es una aplicación desarrollada en C++ que simula la línea de sucesión de una casa real. Su objetivo principal es gestionar la estructura de la dinastía y aplicar las complejas reglas de primogenitura para la asignación automática de un nuevo monarca.

La estructura de la dinastía se modela mediante un Árbol Dinámico (Binary Tree), donde cada nodo representa un noble de la familia. La información de los nobles se maneja a través de un archivo CSV para la carga inicial y la persistencia de los datos entre sesiones.

## Estructura de la Aplicación

El corazón del sistema reside en la clase `ArbolDinamico` y el `struct Noble`.

* Archivo `realeza.h`**: Contiene la definición del `struct Noble` y la declaración de la clase `ArbolDinamico`.
* Archivo `realeza.cpp`**: Contiene la implementación de toda la lógica del árbol, las funciones de carga/guardado, y la lógica de sucesión.
* Archivo `main.cpp`**: Maneja el flujo del programa, el menú de opciones y la gestión de errores al inicio.
* Archivo `bin/realeza.csv`**: Almacena el estado actual de los nobles de la dinastía.

## Funcionalidades Implementadas

La aplicación ofrece un menú interactivo con las siguientes opciones completas:

### 1. Mostrar línea de sucesión actual (Solo los Vivos)

Esta función recorre el árbol dinástico para listar a todos los nobles vivos. La presentación sigue el orden jerárquico de la línea de sucesión, priorizando la primogenitura para mostrar quién está inmediatamente después del monarca actual.

### 2. Asignar Nuevo Rey Automáticamente

Esta es la funcionalidad central que implementa las reglas de sucesión. Cuando el monarca actual muere o abdica, el sistema ejecuta una búsqueda automática que prioriza:

* Primogenitura Masculina:** Busca el descendiente varón vivo más cercano en la línea principal.
* Línea Femenina:** Si no hay varones elegibles, busca la mujer viva más cercana a la línea de sucesión.
* Línea Colateral:** Si la descendencia directa está agotada, asciende en el árbol para buscar sucesores en líneas colaterales (hermanos, tíos y sus descendientes).

### 3. Modificar datos de un Noble

Permite al usuario editar campos clave de cualquier noble por su ID. Esta función es crítica para simular eventos dinásticos como la muerte de un noble, el cambio de edad o la abdicación forzada del monarca, lo cual desencadena la necesidad de usar la Opción 2.

### 4. Guardar datos y Salir

Asegura que cualquier modificación realizada en la memoria del programa (muertes, nuevos reyes, cambios de nombre) se escriba de vuelta al archivo `realeza.csv` antes de terminar la ejecución.

Estudiante: Abraham Almeida EDD2025C