#include "realeza.h"

string a_minusculas(const string& s) { return s; }
bool convertir_a_bool(const string& s) { return false; }
Noble* ArbolDinamico::crear_noble_desde_linea(const string& linea) { return nullptr; }

ArbolDinamico::ArbolDinamico() : raiz(nullptr) {}

void ArbolDinamico::liberar_arbol(Noble* nodo) {}

ArbolDinamico::~ArbolDinamico() {
    liberar_arbol(raiz);
}

Noble* ArbolDinamico::buscar_padre_recursivo(Noble* nodo, int id_padre) const { return nullptr; }
void ArbolDinamico::insertar_noble_en_familia(Noble* nuevo) {}

void ArbolDinamico::cargar_desde_csv() {}
void ArbolDinamico::guardar_a_csv() const {}
void ArbolDinamico::mostrar_linea_sucesion() const {}
void ArbolDinamico::asignar_nuevo_rey_auto() {}
void ArbolDinamico::modificar_noble(int id_noble) {}
Noble* ArbolDinamico::buscar_noble_por_id(Noble* nodo, int id) const { return nullptr; }
Noble* ArbolDinamico::obtener_rey_actual() const { return nullptr; }
// El resto de auxiliares se dejan vacias o nulas