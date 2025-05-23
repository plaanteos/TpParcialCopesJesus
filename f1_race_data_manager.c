#include "utils.h"

// Estructuras para representar los datos
typedef struct {
    int numero_corredor;
    char nombre[16];     // 15 caracteres + terminador nulo
    char apellido[16];   // 15 caracteres + terminador nulo
    char escuderia[16];  // 15 caracteres + terminador nulo
} Corredor;
typedef struct {
    int puesto_salida;   // Posición de salida
    int numero_vuelta;
    int numero_corredor;
    float tiempo;        // Tiempo en segundos
} Tiempo;
// Estructura para manejar los resultados finales
typedef struct {
    char nombre[16];
    char apellido[16];
    char escuderia[16];
    int numero_corredor;
    float tiempo_total;
    int vueltas_completadas;
} ResultadoFinal;
// Función para verificar si un archivo existe
int archivo_existe(const char* nombre_archivo) {
    FILE* archivo = fopen(nombre_archivo, "r");
    if (archivo) {
        fclose(archivo);
        return 1;
    }
    return 0;
}
// La función utf8() ya está definida in utils.h
// Función para leer el archivo de configuración
int leer_configuracion(char *app_nombre, char *circuito, char *version) {
    FILE *config;
    config = fopen("config.txt", "r"); // Abrir el archivo de configuración
    if (config == NULL) {
        printf("Error: no se puede abrir el archivo de configuración.\n");
        printf("Creando archivo config.txt con valores predeterminados...\n");
        // Crear archivo de configuración con valores predeterminados
        config = fopen("config.txt", "w");
        if (config == NULL) {
            printf("Error: no se puede crear el archivo de configuración.\n");
            return 0;
        }
        fprintf(config, "F1 Data Manager\n");
        fprintf(config, "Monaco\n");
        fprintf(config, "1.0\n");
        fclose(config);
        // Reabrir en modo lectura
        config = fopen("config.txt", "r");
        if (config == NULL) {
            printf("Error: no se puede abrir el archivo de configuración.\n");
            return 0;
        }
    }
    // Lee línea por línea
    if (fgets(app_nombre, 16, config) == NULL ||
        fgets(circuito, 16, config) == NULL ||
        fgets(version, 6, config) == NULL) {
        printf("Error: Formato de configuración incorrecto.\n");
        fclose(config);
        return 0;
    }
    // Eliminar saltos de línea
    app_nombre[strcspn(app_nombre, "\n")] = 0;
    circuito[strcspn(circuito, "\n")] = 0;
    version[strcspn(version, "\n")] = 0;
    fclose(config);
    return 1;
}
// Función para actualizar corredores desde CSV
int actualizar_corredores_desde_csv(const char *archivo_csv, int modificacion_num) {
    FILE *csv, *dat;
    Corredor corredor;
    char linea[100];
    char archivo_dat[60];
    // Verificar si el archivo CSV existe
    csv = fopen(archivo_csv, "r");
    if (csv == NULL) {
        printf("Error: no se puede abrir el archivo CSV '%s'.\n", archivo_csv);
        return 0;
    }
    // Generar el nombre del archivo .dat a partir del nombre del CSV
    strcpy(archivo_dat, archivo_csv);
    char *extension = strrchr(archivo_dat, '.');
    if (extension) {
        strcpy(extension, ".dat");
    } else {
        strcat(archivo_dat, ".dat");
    }
    printf("Generando archivo binario: %s\n", archivo_dat);
    // Crear o sobrescribir el archivo .dat
    dat = fopen(archivo_dat, "wb");
    if (dat == NULL) {
        printf("Error: No se puede crear el archivo binario '%s'.\n", archivo_dat);
        fclose(csv);
        return 0;
    }
    // Saltar la línea de encabezados si existe
    if (fgets(linea, sizeof(linea), csv) == NULL) {
        printf("Error: archivo CSV vacío.\n");
        fclose(csv);
        fclose(dat);
        return 0;
    }
    // Leer y procesar cada línea del CSV
    printf("Modificación %d:\n", modificacion_num);
    int corredores_agregados = 0;
    while (fgets(linea, sizeof(linea), csv) != NULL) {
        // Parsear la línea CSV 
        if (sscanf(linea, "%d,%15[^,],%15[^,],%15[^\n]", 
                   &corredor.numero_corredor, 
                   corredor.nombre, 
                   corredor.apellido, 
                   corredor.escuderia) == 4) {
            // Escribir en el archivo binario
            fwrite(&corredor, sizeof(Corredor), 1, dat);
            printf("Corredor: %d, %s %s, Escudería: %s\n", 
                   corredor.numero_corredor, 
                   corredor.nombre, 
                   corredor.apellido, 
                   corredor.escuderia);
            corredores_agregados++;
        }
    }
    fclose(csv);
    fclose(dat);
    if (corredores_agregados > 0) {
        esperar(5);
        printf("Archivo binario '%s' generado exitosamente. Se agregaron %d corredores.\n", archivo_dat, corredores_agregados);
        // Mostrar el contenido del archivo binario recién creado
        printf("\nMostrando contenido del archivo binario creado: %s\n", archivo_dat);
        printf("------------------------------------------\n");
        printf("Número | Nombre  | Apellido | Escudería\n");
        printf("------------------------------------------\n");
        // Abrir el archivo binario para lectura
        dat = fopen(archivo_dat, "rb");
        if (dat != NULL) {
            while (fread(&corredor, sizeof(Corredor), 1, dat) == 1) {
                printf("%6d | %-7s | %-8s | %s\n", 
                       corredor.numero_corredor, 
                       corredor.nombre, 
                       corredor.apellido, 
                       corredor.escuderia);
            }
            fclose(dat);
        } else {
            printf("Error al abrir el archivo binario para verificación.\n");
        }
        // Además, copiar los datos al archivo general de corredores
        printf("\n¿Desea añadir estos corredores al archivo general? (s/n): ");
        char opcion;
        scanf(" %c", &opcion);
        if (opcion == 's' || opcion == 'S') {
            // Abrir el archivo fuente y destino
            dat = fopen(archivo_dat, "rb");
            FILE *general = fopen("corredores.dat", "r+b");
            if (dat == NULL) {
                printf("Error al abrir el archivo temporal.\n");
                return 1;
            }
            if (general == NULL) {
                // Si el archivo no existe, crearlo
                general = fopen("corredores.dat", "wb");
                if (general == NULL) {
                    printf("Error al crear el archivo general.\n");
                    fclose(dat);
                    return 1;
                }
            }
            // Leer y almacenar corredores existentes
            Corredor *corredores = NULL;
            int contador = 0, capacidad = 10;
            corredores = malloc(capacidad * sizeof(Corredor));
            if (corredores == NULL) {
                printf("Error: No se puede asignar memoria.\n");
                fclose(dat);
                fclose(general);
                return 1;
            }
            // Leer corredores existentes
            while (fread(&corredor, sizeof(Corredor), 1, general) == 1) {
                if (contador >= capacidad) {
                    capacidad *= 2;
                    Corredor *temp = realloc(corredores, capacidad * sizeof(Corredor));
                    if (temp == NULL) {
                        printf("Error: No se puede reasignar memoria.\n");
                        free(corredores);
                        fclose(dat);
                        fclose(general);
                        return 1;
                    }
                    corredores = temp;
                }
                corredores[contador++] = corredor;
            }
            // Agregar nuevos corredores sin duplicar
            while (fread(&corredor, sizeof(Corredor), 1, dat) == 1) {
                int duplicado = 0;
                for (int i = 0; i < contador; i++) {
                    if (corredores[i].numero_corredor == corredor.numero_corredor) {
                        duplicado = 1;
                    }
                }
                if (!duplicado) {
                    if (contador >= capacidad) {
                        capacidad *= 2;
                        Corredor *temp = realloc(corredores, capacidad * sizeof(Corredor));
                        if (temp == NULL) {
                            printf("Error: No se puede reasignar memoria.\n");
                            free(corredores);
                            fclose(dat);
                            fclose(general);
                            return 1;
                        }
                        corredores = temp;
                    }
                    corredores[contador++] = corredor;
                }
            }
            // Reescribir el archivo general con todos los corredores sin duplicados
            fclose(general);
            general = fopen("corredores.dat", "wb");
            if (general != NULL) {
                for (int i = 0; i < contador; i++) {
                    fwrite(&corredores[i], sizeof(Corredor), 1, general);
                }
                printf("Corredores añadidos al archivo general correctamente.\n");
                fclose(general);
            }
            free(corredores);
        } else {
            printf("Error al copiar datos al archivo general.\n");
        }
        if (dat != NULL) {
            fclose(dat);
        }
        return 1;
    } else {
        printf("No se agregaron corredores. Verifique el formato del archivo CSV.\n");
        return 0;
    }
}
// Función para actualizar tiempos desde CSV
int actualizar_tiempos_desde_csv(const char *archivo_csv, int modificacion_num) {
    FILE *csv, *dat;
    Tiempo tiempo;
    char linea[100];
    int contador = 1; // Para asignar puesto_salida automáticamente
    char archivo_dat[60];
    // Verificar si el archivo CSV existe
    csv = fopen(archivo_csv, "r");
    if (csv == NULL) {
        printf("Error: No se puede abrir el archivo CSV '%s'.\n", archivo_csv);
        return 0;
    }
    // Generar el nombre del archivo .dat a partir del nombre del CSV
    strcpy(archivo_dat, archivo_csv);
    char *extension = strrchr(archivo_dat, '.');
    if (extension) {
        strcpy(extension, ".dat");
    } else {
        strcat(archivo_dat, ".dat");
    }
    printf("Generando archivo binario: %s\n", archivo_dat);
    // Crear o sobrescribir el archivo .dat
    dat = fopen(archivo_dat, "wb");
    if (dat == NULL) {
        printf("Error: No se puede crear el archivo binario '%s'.\n", archivo_dat);
        fclose(csv);
        return 0;
    }
    // Saltar la línea de encabezados si existe
    if (fgets(linea, sizeof(linea), csv) == NULL) {
        printf("Error: Archivo CSV vacío.\n");
        fclose(csv);
        fclose(dat);
        return 0;
    }
    // Leer y procesar cada línea del CSV
    printf("Modificación %d:\n", modificacion_num);
    int tiempos_agregados = 0;
    char tiempo_str[20];
    while (fgets(linea, sizeof(linea), csv) != NULL) {
        // Parsear la línea CSV para extraer número de vuelta, número de corredor y tiempo
        if (sscanf(linea, "%d,%d,%19[^\n]", 
                  &tiempo.numero_vuelta, 
                  &tiempo.numero_corredor, 
                  tiempo_str) == 3) {
            // Convertir el tiempo de formato "minuto:segundo.milisegundo" a segundos
            int minutos, segundos;
            float milisegundos;
            // Separar el tiempo en minutos y segundos
            if (sscanf(tiempo_str, "%d:%f", &minutos, &milisegundos) == 2) {
                segundos = (int)(milisegundos); // Obtener los segundos como entero
                milisegundos -= segundos; // Obtener la parte decimal de los milisegundos
                tiempo.tiempo = minutos * 60 + segundos + milisegundos; // Convertir todo a segundos
            } else {
                printf("Error: Formato de tiempo incorrecto en la línea: %s\n", linea);
                continue; // Si el formato no es correcto, saltar a la siguiente línea
            }
            // Asignar puesto_salida en orden de aparición
            tiempo.puesto_salida = contador++;
            // Escribir en el archivo binario
            fwrite(&tiempo, sizeof(Tiempo), 1, dat);
            // Mostrar tiempo en formato "minuto:segundo.milisegundo"
            printf("Vuelta: %d, Corredor: %d, Tiempo: %d:%.3f\n", 
                   tiempo.numero_vuelta, 
                   tiempo.numero_corredor, 
                   minutos, 
                   segundos + milisegundos); // Mostrar correctamente segundos y milisegundos
            tiempos_agregados++;
        }
    }
    fclose(csv);
    fclose(dat);
    if (tiempos_agregados > 0) {
        esperar(5);
        printf("Archivo binario '%s' generado exitosamente. Se agregaron %d tiempos.\n", archivo_dat, tiempos_agregados);
        // Mostrar el contenido del archivo binario recién creado
        printf("\nMostrando contenido del archivo binario creado: %s\n", archivo_dat);
        printf("------------------------------------------\n");
        printf("Vuelta | Corredor | Puesto | Tiempo\n");
        printf("------------------------------------------\n");
        // Abrir el archivo binario para lectura
        dat = fopen(archivo_dat, "rb");
        if (dat != NULL) {
            while (fread(&tiempo, sizeof(Tiempo), 1, dat) == 1) {
                int minutos = (int)(tiempo.tiempo / 60);
                float segundos = tiempo.tiempo - (minutos * 60);
                printf("%6d | %8d | %6d | %d:%.3f\n", 
                       tiempo.numero_vuelta, 
                       tiempo.numero_corredor,
                       tiempo.puesto_salida,
                       minutos, 
                       segundos);
            }
            fclose(dat);
        } else {
            printf("Error al abrir el archivo binario para verificación.\n");
        }
        // Además, copiar los datos al archivo general de tiempos
        printf("\n¿Desea añadir estos tiempos al archivo general? (s/n): ");
        char opcion;
        scanf(" %c", &opcion);
        esperar(5);
        if (opcion == 's' || opcion == 'S') {
            // Abrir el archivo fuente y destino
            dat = fopen(archivo_dat, "rb");
            FILE *general = fopen("tiempos.dat", "r+b");
            if (dat == NULL) {
                printf("Error al abrir el archivo temporal.\n");
                return 1;
            }
            if (general == NULL) {
                // Si el archivo no existe, crearlo
                general = fopen("tiempos.dat", "wb");
                if (general == NULL) {
                    printf("Error al crear el archivo general.\n");
                    fclose(dat);
                    return 1;
                }
            }
            // Leer y almacenar tiempos existentes
            Tiempo *tiempos_existentes = NULL;
            int contador = 0, capacidad = 10;
            tiempos_existentes = malloc(capacidad * sizeof(Tiempo));
            if (tiempos_existentes == NULL) {
                printf("Error: No se puede asignar memoria.\n");
                fclose(dat);
                fclose(general);
                return 1;
            }
            // Leer tiempos existentes
            while (fread(&tiempo, sizeof(Tiempo), 1, general) == 1) {
                if (contador >= capacidad) {
                    capacidad *= 2;
                    Tiempo *temp = realloc(tiempos_existentes, capacidad * sizeof(Tiempo));
                    if (temp == NULL) {
                        printf("Error: No se puede reasignar memoria.\n");
                        free(tiempos_existentes);
                        fclose(dat);
                        fclose(general);
                        return 1;
                    }
                    tiempos_existentes = temp;
                }
                tiempos_existentes[contador++] = tiempo;
            }
            int tiempos_nuevos = 0;
            int tiempos_duplicados = 0;
            int total_existentes = contador;

            // Agregar nuevos tiempos sin duplicar
            while (fread(&tiempo, sizeof(Tiempo), 1, dat) == 1) {
                int duplicado = 0;
                for (int i = 0; i < contador; i++) {
                    // Verifica si ya existe un tiempo para esa vuelta y corredor
                    if (tiempos_existentes[i].numero_vuelta == tiempo.numero_vuelta &&
                        tiempos_existentes[i].numero_corredor == tiempo.numero_corredor) {
                        duplicado = 1;
                        tiempos_duplicados++;
                    }
                }
                if (!duplicado) {
                    if (contador >= capacidad) {
                        capacidad *= 2;
                        Tiempo *temp = realloc(tiempos_existentes, capacidad * sizeof(Tiempo));
                        if (temp == NULL) {
                            printf("Error: No se puede reasignar memoria.\n");
                            free(tiempos_existentes);
                            fclose(dat);
                            fclose(general);
                            return 1;
                        }
                        tiempos_existentes = temp;
                    }
                    tiempos_existentes[contador++] = tiempo;
                    tiempos_nuevos++;
                }
            }
            // Reescribir el archivo general con todos los tiempos sin duplicados
            fclose(general);
            general = fopen("tiempos.dat", "wb");
            if (general != NULL) {
                for (int i = 0; i < contador; i++) {
                    fwrite(&tiempos_existentes[i], sizeof(Tiempo), 1, general);
                }
                printf("\nResumen de la actualización:\n");
                printf("- Tiempos existentes: %d\n", total_existentes);
                printf("- Tiempos nuevos agregados: %d\n", tiempos_nuevos);
                printf("- Tiempos duplicados (ignorados): %d\n", tiempos_duplicados);
                printf("- Total de tiempos actuales: %d\n", contador);
                fclose(general);
            }
            free(tiempos_existentes);
        } else {
            printf("No se copiaron datos al archivo general.\n");
        }
        if (dat != NULL) {
            fclose(dat);
        }
        return 1;
    } else {
        printf("No se agregaron tiempos. Verifique el formato del archivo CSV.\n");
        return 0;
    }
}
// Función para mostrar los corredores
void mostrar_corredores() {
    FILE *dat;
    Corredor corredor;
    Corredor *corredores = NULL;
    int contador = 0, capacidad = 10;
    // Verificar si el archivo existe
    if (!archivo_existe("corredores.dat")) {
        printf("El archivo corredores.dat no existe. No hay datos para mostrar.\n");
        return;
    }
    dat = fopen("corredores.dat", "rb");
    if (dat == NULL) {
        printf("Error: No se puede abrir el archivo de corredores.\n");
        return;
    }
    // Asignar memoria inicial
    corredores = malloc(capacidad * sizeof(Corredor));
    if (corredores == NULL) {
        printf("Error: No se puede asignar memoria.\n");
        fclose(dat);
        return;
    }
    // Leer todos los corredores y almacenarlos sin duplicados
    while (fread(&corredor, sizeof(Corredor), 1, dat) == 1) {
        int duplicado = 0;
        // Verificar si este corredor ya existe
        for (int i = 0; i < contador; i++) {
            if (corredores[i].numero_corredor == corredor.numero_corredor) {
                duplicado = 1;
            }
        }
        if (!duplicado) {
            if (contador >= capacidad) {
                capacidad *= 2;
                Corredor *temp = realloc(corredores, capacidad * sizeof(Corredor));
                if (temp == NULL) {
                    printf("Error: No se puede reasignar memoria.\n");
                    free(corredores);
                    fclose(dat);
                    return;
                }
                corredores = temp;
            }
            corredores[contador++] = corredor;
        }
    }
    fclose(dat);
    esperar(5);
    printf("Lista de Corredores:\n");
    printf("------------------------------------------\n");
    printf("Número | Nombre  | Apellido | Escudería\n");
    printf("------------------------------------------\n");
    // Mostrar corredores sin duplicados
    for (int i = 0; i < contador; i++) {
        printf("%6d | %-7s | %-8s | %s\n", 
               corredores[i].numero_corredor, 
               corredores[i].nombre, 
               corredores[i].apellido, 
               corredores[i].escuderia);
    }
    esperar(5);
    if (contador == 0) {
        printf("No hay corredores registrados.\n");
    } else {
        printf("------------------------------------------\n");
        printf("Total de corredores: %d\n", contador);
    }
    free(corredores);
}
// Función para comparar dos tiempos (para qsort)
int comparar_tiempos(const void *a, const void *b) {
    Tiempo *tiempoA = (Tiempo *)a;
    Tiempo *tiempoB = (Tiempo *)b;
    // Si las vueltas son diferentes, ordenar por vuelta
    if (tiempoA->numero_vuelta != tiempoB->numero_vuelta) {
        return tiempoA->numero_vuelta - tiempoB->numero_vuelta;
    }
    // Si algún tiempo es 0, debe ir al final de su vuelta (descalificado)
    if (tiempoA->tiempo == 0.0) return 1;  // A va después
    if (tiempoB->tiempo == 0.0) return -1; // B va después
    // Ordenamiento normal por tiempo para la misma vuelta
    if (tiempoA->tiempo < tiempoB->tiempo) return -1;
    if (tiempoA->tiempo > tiempoB->tiempo) return 1;
    return 0;
}
// Función para mostrar los tiempos
void mostrar_tiempos() {
    FILE *dat;
    Tiempo tiempo;
    Tiempo *tiempos = NULL;
    int contador = 0, capacidad = 10;
    if (!archivo_existe("tiempos.dat")) {
        printf("El archivo tiempos.dat no existe. No hay datos para mostrar.\n");
        return;
    }
    dat = fopen("tiempos.dat", "rb");
    if (dat == NULL) {
        printf("Error: No se puede abrir el archivo de tiempos.\n");
        return;
    }
    tiempos = malloc(capacidad * sizeof(Tiempo));
    if (tiempos == NULL) {
        printf("Error: No se puede asignar memoria.\n");
        fclose(dat);
        return;
    }
    // Leer todos los tiempos y almacenarlos sin duplicados
    while (fread(&tiempo, sizeof(Tiempo), 1, dat) == 1) {
        int duplicado = 0;
        // Verificar si este tiempo ya existe
        for (int i = 0; i < contador; i++) {
            if (tiempos[i].numero_vuelta == tiempo.numero_vuelta && 
                tiempos[i].numero_corredor == tiempo.numero_corredor) {
                duplicado = 1;
            }
        }
        if (!duplicado) {
            if (contador >= capacidad) {
                capacidad *= 2;
                Tiempo *temp = realloc(tiempos, capacidad * sizeof(Tiempo));
                if (temp == NULL) {
                    printf("Error: No se puede reasignar memoria.\n");
                    free(tiempos);
                    fclose(dat);
                    return;
                }
                tiempos = temp;
            }
            tiempos[contador++] = tiempo;
        }
    }
    fclose(dat);
    if (contador == 0) {
        esperar(5);
        printf("No hay tiempos registrados.\n");
        free(tiempos);
        return;
    }
    // Mostrar vuelta clasificatoria (vuelta 0)
    printf("\nClasificación Vuelta 0 (Clasificatoria):\n");
    printf("------------------------------------------\n");
    printf("Puesto | Número Corredor | Tiempo\n");
    printf("------------------------------------------\n");
    // Crear array temporal para la vuelta 0
    Tiempo *vuelta0 = malloc(contador * sizeof(Tiempo));
    int cont_vuelta0 = 0;
    // Filtrar tiempos de vuelta 0
    for (int i = 0; i < contador; i++) {
        if (tiempos[i].numero_vuelta == 0) {
            vuelta0[cont_vuelta0++] = tiempos[i];
        }
    }
    // Ordenar vuelta 0
    qsort(vuelta0, cont_vuelta0, sizeof(Tiempo), comparar_tiempos);
    // Mostrar resultados de vuelta 0
    for (int i = 0; i < cont_vuelta0; i++) {
        int minutos = (int)(vuelta0[i].tiempo / 60);
        float segundos = vuelta0[i].tiempo - (minutos * 60);
        if (vuelta0[i].tiempo == 0.0) {
            printf("%6d | %15d | No tiempo\n", 
                   i + 1, 
                   vuelta0[i].numero_corredor);
        } else {
            printf("%6d | %15d | %d:%.3f\n", 
                   i + 1, 
                   vuelta0[i].numero_corredor, 
                   minutos, 
                   segundos);
        }
    }
    free(vuelta0);
    // Mostrar tiempos de las demás vueltas
    printf("\nTiempos de las demás vueltas:\n");
    printf("------------------------------------------\n");
    printf("Vuelta | Número Corredor | Tiempo\n");
    printf("------------------------------------------\n");
    // Ordenar tiempos usando qsort con la función de comparación
    qsort(tiempos, contador, sizeof(Tiempo), comparar_tiempos);
    // Mostrar tiempos ordenados (excluyendo vuelta 0)
    for (int i = 0; i < contador; i++) {
        if (tiempos[i].numero_vuelta > 0) {
            int minutos = (int)(tiempos[i].tiempo / 60);
            float segundos = tiempos[i].tiempo - (minutos * 60);
            if (tiempos[i].tiempo == 0.0) {
                printf("%6d | %15d | No tiempo\n", 
                       tiempos[i].numero_vuelta,
                       tiempos[i].numero_corredor);
            } else {
                printf("%6d | %15d | %d:%.3f\n", 
                       tiempos[i].numero_vuelta,
                       tiempos[i].numero_corredor,
                       minutos,
                       segundos);
            }
        }
    }
    free(tiempos);
}
// Función para borrar archivos
void borrar_archivos() {
    char opcion;
    printf("¿Desea borrar los contenidos de los archivos? (s/n): ");
    scanf(" %c", &opcion);
    if (opcion == 's' || opcion == 'S') {
        // Borrar los archivos
        FILE *f1 = fopen("corredores.dat", "wb");
        if (f1 != NULL) {
            fclose(f1);
        } else {
            printf("Error al intentar borrar corredores.dat\n");
        }
        FILE *f2 = fopen("tiempos.dat", "wb");
        if (f2 != NULL) {
            fclose(f2);
        } else {
            printf("Error al intentar borrar tiempos.dat\n");
        }
        printf("Archivos borrados exitosamente.\n");
    } else {
        printf("No se han borrado los archivos.\n");
    }
}
// Función para borrar solo corredores
void borrar_corredores() {
    char opcion;
    printf("¿Está seguro que desea borrar los corredores? (s/n): ");
    scanf(" %c", &opcion);
    if (opcion == 's' || opcion == 'S') {
        FILE *f = fopen("corredores.dat", "wb");
        if (f != NULL) {
            esperar(5);
            fclose(f);
            printf("Archivo de corredores borrado exitosamente.\n");
        } else {
            printf("Error al intentar borrar el archivo de corredores.\n");
        }
    } else {
        esperar(5);
        printf("No se han borrado los corredores.\n");
    }
}
// Función para borrar solo tiempos
void borrar_tiempos() {
    char opcion;
    printf("¿Está seguro que desea borrar los tiempos? (s/n): ");
    scanf(" %c", &opcion);
    if (opcion == 's' || opcion == 'S') {
        FILE *f = fopen("tiempos.dat", "wb");
        if (f != NULL) {
            esperar(5);
            fclose(f);
            printf("Archivo de tiempos borrado exitosamente.\n");
        } else {
            printf("Error al intentar borrar el archivo de tiempos.\n");
        }
    } else {
        esperar(5);
        printf("No se han borrado los tiempos.\n");
    }
}
// Función para comparar resultados finales (para qsort)
int comparar_resultados(const void *a, const void *b) {
    ResultadoFinal *resultadoA = (ResultadoFinal *)a;
    ResultadoFinal *resultadoB = (ResultadoFinal *)b;
    
    // Si tienen diferente número de vueltas, el que completó más vueltas va primero
    if (resultadoA->vueltas_completadas != resultadoB->vueltas_completadas) {
        return resultadoB->vueltas_completadas - resultadoA->vueltas_completadas;
    }
    
    // Si tienen las mismas vueltas, ordenar por tiempo total
    if (resultadoA->tiempo_total < resultadoB->tiempo_total) return -1;
    if (resultadoA->tiempo_total > resultadoB->tiempo_total) return 1;
    return 0;
}

void mostrar_resultados_finales() {
    esperar(5);
    FILE *dat_corredores, *dat_tiempos;
    Corredor corredor;
    Tiempo tiempo;
    ResultadoFinal *resultados = NULL;
    int num_corredores = 0, capacidad = 10;
    // Verificar si los archivos existen
    if (!archivo_existe("corredores.dat") || !archivo_existe("tiempos.dat")) {
        printf("Error: Faltan archivos necesarios (corredores.dat o tiempos.dat).\n");
        return;
    }
    // Abrir archivos
    dat_corredores = fopen("corredores.dat", "rb");
    dat_tiempos = fopen("tiempos.dat", "rb");
    if (dat_corredores == NULL || dat_tiempos == NULL) {
        printf("Error al abrir los archivos.\n");
        return;
    }
    // Asignar memoria inicial
    resultados = malloc(capacidad * sizeof(ResultadoFinal));
    if (resultados == NULL) {
        printf("Error: No se puede asignar memoria.\n");
        fclose(dat_corredores);
        fclose(dat_tiempos);
        return;
    }
    // Leer corredores y crear estructura de resultados
    while (fread(&corredor, sizeof(Corredor), 1, dat_corredores) == 1) {
        int encontrado = 0;
        // Verificar si ya tenemos este corredor
        for (int i = 0; i < num_corredores; i++) {
            if (resultados[i].numero_corredor == corredor.numero_corredor) {
                encontrado = 1;
            }
        }
        if (!encontrado) {
            if (num_corredores >= capacidad) {
                capacidad *= 2;
                ResultadoFinal *temp = realloc(resultados, capacidad * sizeof(ResultadoFinal));
                if (temp == NULL) {
                    printf("Error: No se puede reasignar memoria.\n");
                    free(resultados);
                    fclose(dat_corredores);
                    fclose(dat_tiempos);
                    return;
                }
                resultados = temp;
            }
            
            // Inicializar nuevo corredor
            strcpy(resultados[num_corredores].nombre, corredor.nombre);
            strcpy(resultados[num_corredores].apellido, corredor.apellido);
            strcpy(resultados[num_corredores].escuderia, corredor.escuderia);
            resultados[num_corredores].numero_corredor = corredor.numero_corredor;
            resultados[num_corredores].tiempo_total = 0;
            resultados[num_corredores].vueltas_completadas = 0;
            num_corredores++;
        }
    }
    // Leer tiempos y acumularlos para cada corredor
    while (fread(&tiempo, sizeof(Tiempo), 1, dat_tiempos) == 1) {
        if (tiempo.numero_vuelta > 0) { // No contar la vuelta de clasificación
            for (int i = 0; i < num_corredores; i++) {
                if (resultados[i].numero_corredor == tiempo.numero_corredor) {
                    if (tiempo.tiempo > 0) { // Solo contar tiempos válidos
                        resultados[i].tiempo_total += tiempo.tiempo;
                        resultados[i].vueltas_completadas++;
                    }
                }
            }
        }
    }
    // Ordenar resultados
    qsort(resultados, num_corredores, sizeof(ResultadoFinal), comparar_resultados);
    // Mostrar resultados
    printf("\n===========================================\n");
    printf("          RESULTADOS FINALES              \n");
    printf("===========================================\n\n");
    for (int i = 0; i < num_corredores; i++) {
        int minutos_total = (int)(resultados[i].tiempo_total / 60);
        float segundos_total = resultados[i].tiempo_total - (minutos_total * 60); 
        printf("%d. %s %s\n", i + 1, resultados[i].nombre, resultados[i].apellido);
        printf("   Escudería: %s\n", resultados[i].escuderia);
        printf("   Vueltas completadas: %d\n", resultados[i].vueltas_completadas);
        if (resultados[i].vueltas_completadas > 0) {
            printf("   Tiempo total: %d:%06.3f\n", minutos_total, segundos_total);
        } else {
            printf("   No completó ninguna vuelta\n");
        }
        printf("-------------------------------------------\n");
    }
    // Liberar memoria y cerrar archivos
    free(resultados);
    fclose(dat_corredores);
    fclose(dat_tiempos);
}
// Función principal
int main() {
    utf8();
    int opcion;
    char app_nombre[16], circuito[16], version[6];
    char archivo_csv[50];
    int ejecutar = 1;
    int modificacion_num = 1; // Contador de modificaciones
    // Leer configuración
    if (!leer_configuracion(app_nombre, circuito, version)) {
        printf("Error al leer la configuracion. Se utilizarán valores predeterminados.\n");
        strcpy(app_nombre, "F1 Data Manager");
        strcpy(circuito, "Default");
        strcpy(version, "1.0");
    }
    mostrar_animacion_f1();
    printf("Bienvenido a %s v%s\n", app_nombre, version);
    printf("Circuito: %s\n\n", circuito);
    while (ejecutar) {
        printf("\nMenú Principal:\n");
        printf("1. Generar archivo binario de corredores desde CSV\n");
        printf("2. Generar archivo binario de tiempos desde CSV\n");
        printf("3. Mostrar corredores\n");
        printf("4. Mostrar tiempos\n");
        printf("5. Borrar corredores\n");
        printf("6. Borrar tiempos\n");
        printf("7. mostrar resultados finales\n");
        printf("8. Salir\n");
        printf("Elija una opción: ");
        if (scanf("%d", &opcion) != 1) {
            // Limpiar el buffer de entrada si la entrada no es un número
            while (getchar() != '\n');
            printf("Entrada inválida. Por favor ingrese un número.\n");
            continue;
        }
        switch (opcion) {
            case 1:
                printf("Ingrese el nombre del archivo CSV de corredores (nombredelarchivo.csv): ");
                scanf("%49s", archivo_csv);
                actualizar_corredores_desde_csv(archivo_csv, modificacion_num++);
                break;
            case 2:
                printf("Ingrese el nombre del archivo CSV de tiempos (nombredelarchivo.csv): ");
                scanf("%49s", archivo_csv);
                actualizar_tiempos_desde_csv(archivo_csv, modificacion_num++);
                break;
            case 3:
                mostrar_corredores();
                break;
            case 4:
                mostrar_tiempos();
                break;
            case 5:
                borrar_corredores();
                break;
            case 6:
                borrar_tiempos();
                break;
            case 7:
                mostrar_resultados_finales();
                break;
            case 8:
                printf("Saliendo del programa...\n");
                esperar(5);
                ejecutar = 0;
                break;
            default:
                printf("Opción inválida. Intente nuevamente.\n");
        }
    }
    return 0;
}