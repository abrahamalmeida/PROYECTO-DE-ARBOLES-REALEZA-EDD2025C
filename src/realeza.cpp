#include "realeza.h"

string a_minusculas(const string& s) {
    string temp = s;
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    return temp;
}

bool convertir_a_bool(const string& s) {
    string minusculas = a_minusculas(s);
    return minusculas == "1" || minusculas == "si" || minusculas == "true" || minusculas == "1 si";
}

Noble* ArbolDinamico::crear_noble_desde_linea(const string& linea) {
    Noble* noble = new Noble();
    stringstream ss(linea);
    string token;
    
    getline(ss, token, ','); noble->id = stoi(token);
    getline(ss, noble->nombre, ',');
    getline(ss, noble->apellido, ',');
    getline(ss, token, ','); noble->genero = token[0];
    getline(ss, token, ','); noble->edad = stoi(token);
    getline(ss, token, ','); noble->id_padre = stoi(token);
    getline(ss, token, ','); noble->esta_muerto = convertir_a_bool(token);
    getline(ss, token, ','); noble->fue_rey = convertir_a_bool(token);
    getline(ss, token, ','); noble->es_rey = convertir_a_bool(token);
    
    return noble;
}

ArbolDinamico::ArbolDinamico() : raiz(nullptr) {}

void ArbolDinamico::liberar_arbol(Noble* nodo) {
    if (!nodo) return;
    liberar_arbol(nodo->primogenito);
    liberar_arbol(nodo->segundogenito);
    delete nodo;
}

ArbolDinamico::~ArbolDinamico() {
    liberar_arbol(raiz);
}

Noble* ArbolDinamico::buscar_padre_recursivo(Noble* nodo, int id_padre) const {
    if (!nodo) return nullptr;
    if (nodo->id == id_padre) return nodo;

    Noble* encontrado = buscar_padre_recursivo(nodo->primogenito, id_padre);
    if (encontrado) return encontrado;

    return buscar_padre_recursivo(nodo->segundogenito, id_padre);
}

void ArbolDinamico::insertar_noble_en_familia(Noble* nuevo) {
    if (nuevo->id_padre == 0) {
        if (raiz) throw runtime_error("La raiz ya existe.");
        raiz = nuevo;
        raiz->nivel = 1;
        return;
    }

    Noble* padre = buscar_padre_recursivo(raiz, nuevo->id_padre);
    if (!padre) {
        throw runtime_error("Padre no encontrado para el ID: " + to_string(nuevo->id));
    }

    nuevo->nivel = padre->nivel + 1;
    if (!padre->primogenito) {
        padre->primogenito = nuevo;
    } else if (!padre->segundogenito) {
        padre->segundogenito = nuevo;
    } 
}

void ArbolDinamico::cargar_desde_csv() {
    ifstream archivo(archivo_realeza);
    if (!archivo.is_open()) throw runtime_error("No se pudo abrir el archivo " + archivo_realeza + ". Asegurese de que este en la carpeta bin/");

    string linea;
    getline(archivo, linea); 

    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        try {
            Noble* nuevo = crear_noble_desde_linea(linea);
            insertar_noble_en_familia(nuevo);
        } catch (...) {
        }
    }
    archivo.close();
}

// NUEVAS FUNCIONES DE PERSISTENCIA Y BUSQUEDA:
void ArbolDinamico::guardar_recursivo(Noble* nodo, ofstream& archivo) const {
    if (!nodo) return;

    archivo << nodo->id << ","
            << nodo->nombre << ","
            << nodo->apellido << ","
            << nodo->genero << ","
            << nodo->edad << ","
            << nodo->id_padre << ","
            << (nodo->esta_muerto ? "1" : "0") << ","
            << (nodo->fue_rey ? "1" : "0") << ","
            << (nodo->es_rey ? "1" : "0") << "\n";

    guardar_recursivo(nodo->primogenito, archivo);
    guardar_recursivo(nodo->segundogenito, archivo);
}

void ArbolDinamico::guardar_a_csv() const {
    ofstream archivo(archivo_realeza);
    if (!archivo.is_open()) throw runtime_error("No se pudo abrir/crear el archivo " + archivo_realeza);

    archivo << "id,nombre,apellido,genero,edad,id_padre,esta_muerto,fue_rey,es_rey\n";
    guardar_recursivo(raiz, archivo);
    archivo.close();
}

Noble* ArbolDinamico::buscar_noble_por_id(Noble* nodo, int id) const {
    if (!nodo) return nullptr;
    if (nodo->id == id) return nodo;
    
    Noble* encontrado = buscar_noble_por_id(nodo->primogenito, id);
    if (encontrado) return encontrado;
    
    return buscar_noble_por_id(nodo->segundogenito, id);
}

Noble* ArbolDinamico::buscar_rey_recursivo(Noble* nodo) const {
    if (!nodo) return nullptr;
    if (nodo->es_rey) return nodo;

    Noble* encontrado = buscar_rey_recursivo(nodo->primogenito);
    if (encontrado) return encontrado;

    return buscar_rey_recursivo(nodo->segundogenito);
}

Noble* ArbolDinamico::obtener_rey_actual() const {
    return buscar_rey_recursivo(raiz);
}

// El resto de funciones se dejan vacías/nulas
void ArbolDinamico::mostrar_linea_sucesion() const {}
void ArbolDinamico::asignar_nuevo_rey_auto() {}
void ArbolDinamico::modificar_noble(int id_noble) {}
// ... el resto de auxiliares ...