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


// 1. Carga y Guardado


void ArbolDinamico::cargar_desde_csv() {
    ifstream archivo(archivo_realeza);
    if (!archivo.is_open()) throw runtime_error("No se pudo abrir el archivo " + archivo_realeza);

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


// 2. Linea de Sucesion


void ArbolDinamico::mostrar_sucesion_vivos_recursivo(Noble* nodo, int& contador) const {
    if (!nodo) return;
    
    if (!nodo->esta_muerto) {
        contador++;
        string estado_rey = nodo->es_rey ? " (REY/REINA ACTUAL)" : "";
        string genero_str = (nodo->genero == 'H' ? "Hombre" : "Mujer");
        cout << contador << ". " << nodo->nombre << " " << nodo->apellido 
             << " (ID: " << nodo->id << ", Edad: " << nodo->edad 
             << ", Genero: " << genero_str << ")" << estado_rey << "\n";
    }
    
    mostrar_sucesion_vivos_recursivo(nodo->primogenito, contador);
    mostrar_sucesion_vivos_recursivo(nodo->segundogenito, contador);
}

void ArbolDinamico::mostrar_linea_sucesion() const {
    if (!raiz) {
        cout << "El arbol dinastico esta vacio.\n";
        return;
    }

    int contador = 0;
    cout << "\n    LINEA DE SUCESION DINASTICA (VIVOS)    \n";
    mostrar_sucesion_vivos_recursivo(raiz, contador);

    if (contador == 0) {
        cout << "No hay nobles vivos registrados.\n";
    }
}


// 3. Asignacion del Rey


Noble* ArbolDinamico::buscar_padre(Noble* hijo) const {
    if (!hijo || hijo->id_padre == 0) return nullptr;
    return buscar_noble_por_id(raiz, hijo->id_padre);
}

Noble* ArbolDinamico::obtener_descendiente_vivo_varon(Noble* nodo, bool buscar_primogenito) {
    if (!nodo) return nullptr;
    
    if (nodo->genero == 'H' && !nodo->esta_muerto && nodo->edad < 70) {
        return nodo;
    }

    Noble* sucesor_p = obtener_descendiente_vivo_varon(nodo->primogenito, buscar_primogenito);
    if (sucesor_p) return sucesor_p;

    // Solo se recorre el segundogenito si estamos en modo primogenito puro (true) o modo general (false)
    Noble* sucesor_s = obtener_descendiente_vivo_varon(nodo->segundogenito, buscar_primogenito);
    if (sucesor_s) return sucesor_s;

    return nullptr;
}

Noble* ArbolDinamico::buscar_hermano_vivo(Noble* rey_muerto) {
    Noble* padre = buscar_padre(rey_muerto);
    if (!padre) return nullptr;

    Noble* hermano = nullptr;
    if (padre->primogenito && padre->primogenito->id == rey_muerto->id && padre->segundogenito) {
        hermano = padre->segundogenito;
    } else if (padre->segundogenito && padre->segundogenito->id == rey_muerto->id && padre->primogenito) {
        hermano = padre->primogenito;
    }

    if (hermano && !hermano->esta_muerto && hermano->edad < 70) {
        return hermano;
    }
    return nullptr;
}

Noble* ArbolDinamico::buscar_tio_vivo(Noble* rey_muerto) {
    Noble* padre = buscar_padre(rey_muerto);
    if (!padre) return nullptr;

    Noble* abuelo = buscar_padre(padre);
    if (!abuelo) return nullptr;

    Noble* tio = nullptr;
    if (abuelo->primogenito && abuelo->primogenito->id == padre->id && abuelo->segundogenito) {
        tio = abuelo->segundogenito;
    } else if (abuelo->segundogenito && abuelo->segundogenito->id == padre->id && abuelo->primogenito) {
        tio = abuelo->primogenito;
    }
    
    if (tio && !tio->esta_muerto && tio->edad < 70) {
        return tio;
    }
    return nullptr;
}

Noble* ArbolDinamico::buscar_primer_ancestro_con_dos_hijos(Noble* inicio) {
    Noble* actual = inicio;
    while (actual) {
        if (actual->primogenito && actual->segundogenito) {
            return actual;
        }
        actual = buscar_padre(actual);
    }
    return nullptr; 
}

Noble* ArbolDinamico::encontrar_candidato_mujer(Noble* nodo, Noble* mejor) const {
    if (!nodo) return mejor;

    if (nodo->genero == 'M' && !nodo->esta_muerto && nodo->edad > 15 && nodo->edad < 70) {
        if (!mejor || nodo->edad < mejor->edad) {
            mejor = nodo; 
        } else if (nodo->edad == mejor->edad) {
            if (nodo->nivel < mejor->nivel) { 
                mejor = nodo;
            }
        }
    }
    
    Noble* temp = encontrar_candidato_mujer(nodo->primogenito, mejor);
    if (temp) mejor = temp;
    
    temp = encontrar_candidato_mujer(nodo->segundogenito, mejor);
    if (temp) mejor = temp;
    
    return mejor;
}

void ArbolDinamico::asignar_nuevo_rey_auto() {
    Noble* rey_actual = obtener_rey_actual();
    bool forzar_reemplazo = false;
    
    if (!rey_actual) {
        Noble* candidato_inicial = obtener_descendiente_vivo_varon(raiz, true);
        if (!candidato_inicial) candidato_inicial = encontrar_candidato_mujer(raiz, nullptr);
        
        if (candidato_inicial) {
            candidato_inicial->es_rey = true;
            cout << "\n Primer rey/reina inicial asignado:\n";
            cout << "   " << candidato_inicial->nombre << " " << candidato_inicial->apellido << "\n";
            return;
        }
        throw runtime_error("No hay nobles elegibles para iniciar la dinastia.");
    }
    
    if (!rey_actual->esta_muerto && rey_actual->edad < 70) {
        cout << "El rey/reina actual sigue vivo y es menor de 70 años. No es necesario asignar un nuevo rey.\n";
        return;
    }
    
    if (rey_actual->edad >= 70) {
        cout << rey_actual->nombre << " " << rey_actual->apellido 
             << " ha pasado la edad de 70. Se procede a la abdicacion automatica.\n";
        forzar_reemplazo = true;
    } else {
        cout << rey_actual->nombre << " " << rey_actual->apellido << " ha muerto. Asignando sucesor...\n";
        rey_actual->esta_muerto = true;
        forzar_reemplazo = true;
    }
    
    if (!forzar_reemplazo) return;
    
    rey_actual->es_rey = false;
    rey_actual->fue_rey = true;

    Noble* sucesor = nullptr;
    
    // Varon Vivo (Reglas de Primogenito)
    
    // PASO 1: Rama del Rey Muerto (Primogenitos)
    sucesor = obtener_descendiente_vivo_varon(rey_actual, true);

    if (!sucesor) {
        // PASO 2: Rama del Hermano
        Noble* hermano = buscar_hermano_vivo(rey_actual);
        if (hermano) {
            sucesor = obtener_descendiente_vivo_varon(hermano, true);
            
            if (!sucesor && hermano->genero == 'H' && hermano->edad < 70) {
                sucesor = hermano;
            }
        }
    }

    if (!sucesor) {
        // PASO 3: Rama del Tío
        Noble* tio = buscar_tio_vivo(rey_actual);
        if (tio) {
            sucesor = obtener_descendiente_vivo_varon(tio, true);
            
            if (!sucesor && !tio->primogenito && !tio->segundogenito && tio->genero == 'H' && tio->edad < 70) {
                sucesor = tio;
            }
        }
    }

    if (!sucesor) {
        // PASO 4: Ancestro con Dos Hijos
        Noble* ancestro = buscar_primer_ancestro_con_dos_hijos(rey_actual);
        if (ancestro) {
            sucesor = obtener_descendiente_vivo_varon(ancestro->primogenito, true);
        }
    }
    
    if (!sucesor) {
        // PASO 5: Varon Segundogenito (Se trata a los segundos como primogenitos)
        sucesor = obtener_descendiente_vivo_varon(raiz, false);
    }

    if (!sucesor) {
        // PASO 6: Reina 
        sucesor = encontrar_candidato_mujer(raiz, nullptr);
    }
    
    if (sucesor) {
        sucesor->es_rey = true;
        cout << "\n Nuevo rey/reina asignado automaticamente:\n";
        cout << "   " << sucesor->nombre << " " << sucesor->apellido 
             << " (ID: " << sucesor->id << ", Edad: " << sucesor->edad 
             << ", Genero: " << (sucesor->genero == 'H' ? "Hombre" : "Mujer") << ")\n";
    } else {
        cout << " ¡ALERTA! No se encontro ningun sucesor elegible (varon menor a 70 o mujer joven).\n";
    }
}


// 4. Modificacion


void ArbolDinamico::modificar_noble(int id_noble) {
    Noble* noble = buscar_noble_por_id(raiz, id_noble);
    if (!noble) {
        cout << "Noble con ID " << id_noble << " no encontrado.\n";
        return;
    }

    cout << "\n    Modificando a " << noble->nombre << " " << noble->apellido << "    \n";
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

    string estado_es_rey = noble->es_rey ? "1 (Si)" : "0 (No)";
    cout << "Es Rey (1/0) (" << estado_es_rey << "): ";
    getline(cin, input);
    if (!input.empty()) {
        bool nuevo_es_rey = convertir_a_bool(input);
        if (nuevo_es_rey && !noble->es_rey) {
            Noble* rey_antiguo = obtener_rey_actual();
            if (rey_antiguo) rey_antiguo->es_rey = false;
            noble->es_rey = true;
        } else if (!nuevo_es_rey) {
            noble->es_rey = false;
        }
    }

    cout << "\nDatos del noble ID " << id_noble << " modificados correctamente.\n";
}