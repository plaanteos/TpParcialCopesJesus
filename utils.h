#ifndef A806F08A_0808_4930_A1B7_2788B1AE465A
#define A806F08A_0808_4930_A1B7_2788B1AE465A
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h> // Este #include servirá para más adelante…
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
typedef char cadena[50]; // Para tratar a los arrays de chars como 'cadena'
#define N 50 
///////////////////////////////////////////////////////////////////////////
////////////////////////////  PROTOTIPOS  /////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int* buscarPrimosyCarga(int[], int*);
int leerEntero(cadena mensaje);
float leerFloat(cadena mensaje);
char leerCaracter(cadena mensaje);
int leerEnteroEntre(int valorMin, int valorMax, cadena mensaje);
float leerFloatEntre(float valorMin, float valorMax, cadena mensaje);
int confirmaUsuario(cadena mensaje);
void leerVectorDeEnteros(cadena mensaje, int vec[], int cant);
void leerVectorDeFloats(cadena mensaje, float vec[], int cant);
void leerVectorDeCaracteres(cadena mensaje, char vec[], int cant);
void leerVectorDeEnterosEntre(int valorMin, int valorMax, cadena mensaje, int vec[], int cant);
void leerVectorDeFloatsEntre(float valorMin, float valorMax, cadena mensaje, float vec[], int cant);
void mostrarVectorDeEnteros(int vec[], int cant);
void mostrarVectorDeFloats(float vec[], int cant);
void mostrarVectorDeCaracteres(char vec[], int cant);
void ordenarVectorDeEnterosAscendente(int vec[], int cant);
void ordenarVectorDeEnterosDescendente(int vec[], int cant);
void pausa(void);

void esperar(int segundos);

///////////////////////////////////////////////////////////////////////////
/////////////////////////  IMPLEMENTACIONES  //////////////////////////////
///////////////////////////////////////////////////////////////////////////
int esPrimo(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int* buscarPrimosyCarga(int x[], int* numPrimos) {
    int* temp = NULL;
    *numPrimos = 0;
    
    // Primero contamos los primos
    for (int i = 0; i < N; i++) {
        if (esPrimo(x[i])) (*numPrimos)++;
    }
    
    // Asignamos memoria usando realloc
    temp = (int*)realloc(temp, *numPrimos * sizeof(int));
    if (!temp) return NULL;
    
    // Cargamos los primos
    int j = 0;
    for (int i = 0; i < N; i++) {
        if (esPrimo(x[i])) {
            temp[j++] = x[i];
        }
    }
    
    return temp;
}

int leerEntero(cadena mensaje) {
    /* Muestra el mensaje al usuario, lee un entero y lo retorna */
    int numero;
    printf("%s: ", mensaje);
    fflush(stdin);
    scanf("%d", &numero);
    while (numero<0)
    {
        printf("error, vuelva a ingresar un numero entero...");
        printf("%s: ", mensaje);
        fflush(stdin);
        scanf("%d", &numero);
    }
    return numero;
}

float leerFloat(cadena mensaje) {
    /* Muestra el mensaje al usuario, lee un float y lo retorna */
    float numero;
    printf("%s: ", mensaje);
    fflush(stdin); // No es obligatorio pero por las dudas...
    scanf("%f", &numero);
    return numero;
    
}

char leerCaracter(cadena mensaje) {
    /* Muestra el mensaje al usuario, lee un char y lo retorna */
    char caracter;
    printf("%s: ", mensaje);
    fflush(stdin);
    scanf(" %c", &caracter);
    while (caracter!='f' && caracter!='F' && caracter!='m' && caracter!='M')
    {
        printf("error, no has ingresado un valor valido...");
        printf("%s: ", mensaje);
        fflush(stdin);
        scanf(" %c", &caracter);
    }
    return caracter;
}

int leerEnteroEntre(int valorMin, int valorMax, cadena mensaje) {
    /* Muestra el mensaje al usuario. Luego lee enteros mientras éstos estén
       fuera del rango [valorMin, valorMax] informando al usuario de
       su error. Cuando finalmente lea un entero válido, lo retorna */
    int numero;
    numero = leerEntero(mensaje);
    while (numero < valorMin || numero > valorMax)
    {
        printf("Error, la cantidad de puntos es incorrecta rango[%d;%d]\n",valorMin,valorMax);
        numero = leerEntero(mensaje);
    }
    /*do {
        numero = leerEntero(mensaje);
        if (numero < valorMin || numero > valorMax){
        printf("Error, la cantidad de puntos es incorrecta rango[%d;%d]",valorMax,valorMin);
    }
    } while (numero < valorMin || numero > valorMax);*/
    return numero;
}

float leerFloatEntre(float valorMin, float valorMax, cadena mensaje) {
    /* Muestra el mensaje al usuario. Luego, lee floats mientras éstos estén
       fuera del rango [valorMin, valorMax] informando al usuario de
       su error. Cuando finalmente lea un float válido, lo retorna */
    float numero;
    do {
        numero = leerFloat(mensaje);
    } while (numero < valorMin || numero > valorMax);
    return numero;
}

int confirmaUsuario(cadena mensaje) {
    /* Muestra el mensaje al usuario junto a la leyenda "[S/N]" que
       representa "Si" o "No". Luego, lee chars mientras éstos no sean los
       previstos ('S' o 'N', incluyendo minúsculas) informando al
       usuario de su error. Retorna si el usuario seleccionó que sí. */
    char caracter;
    printf("%s",mensaje);
    scanf(" %c", &caracter);
    while (caracter != 's' && caracter != 'n' && caracter != 'N' && caracter != 'S' )
    {
        printf ("error... ingrese una respuesta valida");
        printf("Llueve?(s/n)");
        scanf(" %c", &caracter);
    }
    return (caracter == 's') ? 1 : 0;
}   
void leerVectorDeEnteros(cadena mensaje, int vec[], int cant) {
     /* Lee 'cant' enteros, mostrando el mismo mensaje cada vez,
 y guarda los valores en cada posición de 'vec' */
    for (int i = 0; i < cant; i++) {
        printf("[%d] ", i);
        vec[i] = leerEntero(mensaje);
    }
}

void leerVectorDeFloats(cadena mensaje, float vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] ", i);
        vec[i] = leerFloat(mensaje);
    }
}

void leerVectorDeCaracteres(cadena mensaje, char vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] ", i);
        vec[i] = leerCaracter(mensaje);
    }
}

void leerVectorDeEnterosEntre(int valorMin, int valorMax, cadena mensaje, int vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] ", i);
        vec[i] = leerEnteroEntre(valorMin, valorMax, mensaje);
    }
}

void leerVectorDeFloatsEntre(float valorMin, float valorMax, cadena mensaje, float vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] ", i);
        vec[i] = leerFloatEntre(valorMin, valorMax, mensaje);
    }
}

void mostrarVectorDeEnteros(int vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] => %d\n", i, vec[i]);
    }
}

void mostrarVectorDeFloats(float vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] => %f\n", i, vec[i]);
    }
}

void mostrarVectorDeCaracteres(char vec[], int cant) {
    for (int i = 0; i < cant; i++) {
        printf("[%d] => %c\n", i, vec[i]);
    }
}

void ordenarVectorDeEnterosAscendente(int vec[], int cant) {
    /* Ordena los valores de 'vec' de forma ascendente (menor a mayor),
 utilizando algún algoritmo de ordenamiento (investigar) */
 int i, j, temp;
    for (i = 0; i < cant-1; i++) {
        for (j = 0; j < cant-i-1; j++) {
            if (vec[j] > vec[j+1]) {
                temp = vec[j];
                vec[j] = vec[j+1];
                vec[j+1] = temp;
            }
        }
    }
}

void ordenarVectorDeEnterosDescendente(int vec[], int cant) {
    // Implementar algoritmo de ordenamiento descendente
    int i, j, temp;
    for (i = 0; i < cant-1; i++) {
        for (j = 0; j < cant-i-1; j++) {
            if (vec[j] < vec[j+1]) {
                temp = vec[j];
                vec[j] = vec[j+1];
                vec[j+1] = temp;
            }
        }
    }
}

void pausa(void) {
    printf("\n\nPresione Enter para continuar...\n\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperar(int segundos) {
    printf("\n\nCargando");
    for (int i = 0; i < segundos; i++) {
        for (int j = 0; j < 5; j++) {
            printf(".");
            fflush(stdout); // Asegura que se imprime inmediatamente
            Sleep(1); // Espera 1 segundo
        }
        printf("\rCargando"); // Regresa a la línea anterior
    }
    printf(" Hecho!\n");
}


#endif /* A806F08A_0808_4930_A1B7_2788B1AE465A */
