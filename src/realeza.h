#include "realeza.h"

void mostrar_menu() {
    cout << "\n    SISTEMA DE DINASTIA REAL:    \n";
    cout << "1. Mostrar linea de sucesion actual (Solo los Vivos)\n";
    cout << "2. Asignar Nuevo Rey Automaticamente\n";
    cout << "3. Modificar datos de un Noble\n"; // <--- Implementado
    cout << "4. Guardar datos y Salir\n";
    cout << "Por favor, seleccione una opcion: ";
}

void limpiar_buffer() {
    cin.clear();
    cin.ignore(256, '\n'); 
}

void pausa() {
    cout << "\nPresione Enter para continuar...";
    limpiar_buffer();
}

void gestionar_opcion(ArbolDinamico& arbol, int opcion) {
    int id;
    string input;
    
    switch (opcion) {
        case 1:
            arbol.mostrar_linea_sucesion();
            break;
        case 2:
            arbol.asignar_nuevo_rey_auto();
            break;
        case 3:
            cout << "Ingrese el ID del noble a modificar: ";
            getline(cin, input);
            try {
                id = stoi(input);
                arbol.modificar_noble(id); // <--- LLAMADA IMPLEMENTADA
            } catch (...) {
                cout << "ID no valido.\n";
            }
            break;
        case 4:
            arbol.guardar_a_csv(); 
            cout << "\nDatos guardados. Saliendo del programa...\n";
            break;
        default:
            cout << "Opcion no valida. Intente de nuevo.\n";
    }
}

int main() {
    ArbolDinamico arbol;
    
    try {
        arbol.cargar_desde_csv();
        cout << "   SISTEMA DE SUCESION REAL:    \n";
        cout << "Datos de dinastia cargados correctamente desde el archivo realeza.csv.\n";
        
        Noble* rey_actual = arbol.obtener_rey_actual();
        if (!rey_actual) {
            cout << "No hay un rey inicial en los datos. Asignando el primero elegible...\n";
            arbol.asignar_nuevo_rey_auto();
        }

        int opcion;
        string opcion_str;
        do {
            mostrar_menu();
            getline(cin, opcion_str);

            try {
                opcion = stoi(opcion_str);
            } catch (...) {
                opcion = 0;
            }
            
            if (opcion != 4) {
                try {
                    gestionar_opcion(arbol, opcion);
                } catch (const exception& e) {
                    cerr << "\nError: " << e.what() << "\n";
                }
                pausa();
            }
        } while (opcion != 4);
        
    } catch (const exception& e) {
        cerr << "Error critico al iniciar: " << e.what() << "\n";
        pausa();
        return 1;
    }
    
    return 0;
}