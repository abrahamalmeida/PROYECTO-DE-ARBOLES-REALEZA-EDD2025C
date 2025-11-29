#ifndef REALEZA_H
#define REALEZA_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

struct Noble {
    int id;
    string nombre;
    string apellido;
    char genero; 
    int edad;
    int id_padre;
    bool esta_muerto;
    bool fue_rey;
    bool es_rey;
    
    Noble* primogenito; 
    Noble* segundogenito; 
    
    int nivel; // Usado para el desempate en la regla de sucesión de mujeres

    Noble() : id(0), genero(' '), edad(0), id_padre(0), 
              esta_muerto(false), fue_rey(false), es_rey(false), 
              primogenito(nullptr), segundogenito(nullptr), nivel(0) {}
};

class ArbolDinamico {
public:
    ArbolDinamico();
    ~ArbolDinamico();
    
    void cargar_desde_csv();
    void guardar_a_csv() const;
    
    void mostrar_linea_sucesion() const;
    
    void asignar_nuevo_rey_auto();
    
    void modificar_noble(int id_noble);
    
    Noble* buscar_noble_por_id(Noble* nodo, int id) const;
    Noble* obtener_rey_actual() const;

private:
    Noble* raiz;
    const string archivo_realeza = "realeza.csv";
    
    Noble* crear_noble_desde_linea(const string& linea);
    void liberar_arbol(Noble* nodo);
    
    void insertar_noble_en_familia(Noble* nuevo);
    Noble* buscar_padre_recursivo(Noble* nodo, int id_padre) const;
    
    void mostrar_sucesion_vivos_recursivo(Noble* nodo, int& contador) const;
    void guardar_recursivo(Noble* nodo, ofstream& archivo) const;
    
    Noble* buscar_rey_recursivo(Noble* nodo) const;
    Noble* obtener_descendiente_vivo_varon(Noble* nodo, bool buscar_primogenito);
    Noble* buscar_padre(Noble* hijo) const;
    Noble* buscar_hermano_vivo(Noble* rey_muerto);
    Noble* buscar_tio_vivo(Noble* rey_muerto);
    Noble* buscar_primer_ancestro_con_dos_hijos(Noble* inicio);

    Noble* encontrar_candidato_mujer(Noble* nodo, Noble* mejor) const;
};

#endif