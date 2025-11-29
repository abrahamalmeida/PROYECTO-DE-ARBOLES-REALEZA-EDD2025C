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

void ArbolDinamico::mostrar_sucesion_vivos_recursivo(Noble* nodo, int& contador) const {
    if (!nodo) return;

    if (!nodo->es_rey && !nodo->esta_muerto) {
        cout << contador++ << ". ID: " << nodo->id 
             << " - " << nodo->nombre << " " << nodo->apellido 
             << " (" << nodo->genero << ", " << nodo->edad << " años)"
             << (nodo->fue_rey ? " [Ex Rey]" : "") << "\n";
    }

    mostrar_sucesion_vivos_recursivo(nodo->primogenito, contador);
    mostrar_sucesion_vivos_recursivo(nodo->segundogenito, contador);
}

void ArbolDinamico::mostrar_linea_sucesion() const {
    Noble* rey_actual = obtener_rey_actual();

    cout << "\n    LINEA DE SUCESION ACTUAL (VIVOS)    \n";

    if (!raiz) {
        cout << "El arbol esta vacio.\n";
        return;
    }

    if (rey_actual) {
        cout << "REY/REINA ACTUAL: ID: " << rey_actual->id << " - " 
             << rey_actual->nombre << " " << rey_actual->apellido 
             << " (" << rey_actual->genero << ", " << rey_actual->edad << " años)\n";
        cout << "------------------------------------------\n";
    } else {
        cout << "ADVERTENCIA: No hay un rey/reina designado. La sucesion empieza desde la raiz.\n";
    }
    
    int contador = 1;

    Noble* nodo_inicio = rey_actual ? rey_actual : raiz;

    if (nodo_inicio) {
        mostrar_sucesion_vivos_recursivo(nodo_inicio->primogenito, contador);
        mostrar_sucesion_vivos_recursivo(nodo_inicio->segundogenito, contador);
    }
    
    if (contador == 1) {
        cout << "No hay sucesores vivos elegibles en la linea de descendencia directa del rey.\n";
    }
}

Noble* ArbolDinamico::buscar_padre(Noble* hijo) const {
    if (!hijo || hijo == raiz) return nullptr;
    return buscar_noble_por_id(raiz, hijo->id_padre);
}

Noble* ArbolDinamico::obtener_descendiente_vivo_varon(Noble* nodo, bool buscar_primogenito) {
    Noble* inicio = buscar_primogenito ? nodo->primogenito : nodo->segundogenito;
    if (!inicio) return nullptr;

    if (!inicio->esta_muerto && inicio->genero == 'M') return inicio;

    Noble* candidato_p = obtener_descendiente_vivo_varon(inicio, true);
    if (candidato_p) return candidato_p;

    Noble* candidato_s = obtener_descendiente_vivo_varon(inicio, false);
    if (candidato_s) return candidato_s;

    return nullptr;
}

Noble* ArbolDinamico::encontrar_candidato_mujer(Noble* nodo, Noble* mejor) const {
    if (!nodo) return mejor;

    if (!nodo->esta_muerto && nodo->genero == 'H') {
        if (!mejor || nodo->nivel < mejor->nivel || (nodo->nivel == mejor->nivel && nodo->edad > mejor->edad)) {
            mejor = nodo;
        }
    }

    mejor = encontrar_candidato_mujer(nodo->primogenito, mejor);
    mejor = encontrar_candidato_mujer(nodo->segundogenito, mejor);
    return mejor;
}

void ArbolDinamico::asignar_nuevo_rey_auto() {
    Noble* rey_actual = obtener_rey_actual();
    Noble* nodo_inicio = rey_actual ? rey_actual : raiz;
    Noble* candidato = nullptr;
    
    if (!nodo_inicio) {
        cout << "\nError: El arbol esta vacio. No se puede asignar un rey.\n";
        return;
    }
    
    if (rey_actual) {
        rey_actual->es_rey = false;
        rey_actual->fue_rey = true;
    } else {
        nodo_inicio = raiz; 
    }
    
    candidato = obtener_descendiente_vivo_varon(nodo_inicio, true);
    if (!candidato) candidato = obtener_descendiente_vivo_varon(nodo_inicio, false);
    
    if (!candidato) {
        candidato = encontrar_candidato_mujer(nodo_inicio->primogenito, nullptr);
        candidato = encontrar_candidato_mujer(nodo_inicio->segundogenito, candidato);
    }
    
    if (!candidato) {
        Noble* ancestro = rey_actual; 
        
        while (ancestro) {
            Noble* padre = buscar_padre(ancestro);
            if (!padre) break;

            if (padre->primogenito == ancestro && padre->segundogenito) {
                Noble* linea_colateral = padre->segundogenito;
                
                candidato = obtener_descendiente_vivo_varon(linea_colateral, true);
                if (!candidato) candidato = obtener_descendiente_vivo_varon(linea_colateral, false);
                
                if (!candidato) {
                    candidato = encontrar_candidato_mujer(linea_colateral->primogenito, nullptr);
                    candidato = encontrar_candidato_mujer(linea_colateral->segundogenito, candidato);
                }
                
                if (candidato) break;
            }
            
            ancestro = padre;
        }
    }
    
    if (candidato) {
        candidato->es_rey = true;
        cout << "\n=============================================\n";
        cout << "NUEVO MONARCA ASIGNADO AUTOMATICAMENTE\n";
        cout << "ID: " << candidato->id << " - " << candidato->nombre << " " << candidato->apellido;
        cout << " (" << (candidato->genero == 'M' ? "Rey" : "Reina") << ", Edad: " << candidato->edad << ")\n";
        cout << "=============================================\n";
    } else {
        cout << "\n=============================================\n";
        cout << "ADVERTENCIA: No se encontro ningun sucesor vivo elegible.\n";
        cout << "El trono queda vacante.\n";
        cout << "=============================================\n";
    }
}

//  Implementación de la Modificación 

void ArbolDinamico::modificar_noble(int id_noble) {
    Noble* noble = buscar_noble_por_id(raiz, id_noble);
    if (!noble) {
        cout << "Error: Noble con ID " << id_noble << " no encontrado.\n";
        return;
    }

    cout << "\n    MODIFICANDO A " << noble->nombre << " " << noble->apellido << " (ID: " << noble->id << ") ===\n";
    cout << "Deje vacio para mantener el valor actual. (Solo se modifican campos de texto/estado)\n";
    
    string input;

    cout << "Nuevo Nombre (" << noble->nombre << "): ";
    getline(cin, input);
    if (!input.empty()) noble->nombre = input;

    cout << "Nuevo Apellido (" << noble->apellido << "): ";
    getline(cin, input);
    if (!input.empty()) noble->apellido = input;
    
    cout << "Nuevo Genero (H/M) (" << noble->genero << "): ";
    getline(cin, input);
    if (!input.empty() && (input[0] == 'H' || input[0] == 'M')) noble->genero = input[0];

    cout << "Nueva Edad (" << noble->edad << "): ";
    getline(cin, input);
    if (!input.empty()) {
        try {
            noble->edad = stoi(input);
        } catch (...) {
            cout << "Edad no valida. Se mantiene la anterior.\n";
        }
    }
    
    string estado_muerto = noble->esta_muerto ? "1 (Muerto)" : "0 (Vivo)";
    cout << "Esta Muerto (1/0) (" << estado_muerto << "): ";
    getline(cin, input);
    if (!input.empty()) noble->esta_muerto = convertir_a_bool(input);

    string estado_fue_rey = noble->fue_rey ? "1 (Si)" : "0 (No)";
    cout << "Fue Rey (1/0) (" << estado_fue_rey << "): ";
    getline(cin, input);
    if (!input.empty()) noble->fue_rey = convertir_a_bool(input);

    // Manejo de la Corona (es_rey)
    string estado_es_rey = noble->es_rey ? "1 (Si)" : "0 (No)";
    cout << "Es Rey (1/0) (" << estado_es_rey << "): ";
    getline(cin, input);
    bool nuevo_es_rey = noble->es_rey; 
    if (!input.empty()) {
        nuevo_es_rey = convertir_a_bool(input);
    }

    if (noble->esta_muerto && nuevo_es_rey) {
        cout << "Advertencia: Un noble muerto no puede ser Rey. 'Es Rey' se establece en 0.\n";
        noble->es_rey = false;
    } else if (nuevo_es_rey && !noble->es_rey) {
        Noble* rey_actual = obtener_rey_actual();
        if (rey_actual) {
            rey_actual->es_rey = false;
            rey_actual->fue_rey = true;
            cout << "Advertencia: El noble " << rey_actual->nombre << " ya no es Rey.\n";
        }
        noble->es_rey = true;
        noble->fue_rey = true; 
    } else if (!nuevo_es_rey && noble->es_rey) {
        noble->es_rey = false;
        cout << "Advertencia: El Rey/Reina ha sido destronado. Ejecute Opcion 2 para nombrar un sucesor.\n";
    }
    
    cout << "\nDatos de " << noble->nombre << " actualizados correctamente.\n";
}

// Las funciones auxiliares de busqueda de colaterales se dejan vacías ya que la lógica principal las engloba
Noble* ArbolDinamico::buscar_hermano_vivo(Noble* rey_muerto) { return nullptr; }
Noble* ArbolDinamico::buscar_tio_vivo(Noble* rey_muerto) { return nullptr; }
Noble* ArbolDinamico::buscar_primer_ancestro_con_dos_hijos(Noble* inicio) { return nullptr; }