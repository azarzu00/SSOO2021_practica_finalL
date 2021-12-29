/*   COMANDOS PARA MANDAR LAS SEÑALES AL PROGRAMA CUANDO
     SE ESTA EJECUTANDO UWU UWU UWU UWU UWU UWU

      kill -s SIGUSR1 pid
      kill -s SIGUSR2 pid
      kill -s SITERM pid   
*/

//Incluir las librerias
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h> 
#include <stdbool.h> 

//DECLARACIONES
//contadores
int contadorClientes = 0;
int numClientesAscensor = 0;
int contadorIdClientes = 1;
bool ascensor;
//semaforos
pthread_mutex_t semaforo, semaforoFichero, maquinas;
pthread_t hiloClientes;

int recepcionistas[3];
int maquinasCheckIn[5];
FILE * logFile;

void *nuevoCliente(void *arg);
void writeLogMessage(char *id, char *msg);
// void *accionesRecepcionista1(void *arg);
// void *accionesRecepcionista2(void *arg);
// void *accionesRecepcionistaVip(void *arg);
void creacionClienteNormal();
void creacionClienteVip();


struct Cliente{
  int  id;
  bool atendido; 
  bool esVip;
  bool ascensor;
} listaClientes[20];



int main(int argc, char *argv[]){

  // cuando se llame SIGUSR1
  //     pthread_create(dhuifhowejifiowe, "NOVIP")
  //  ¡cuando se llame SIGUSR2
  //     pthread_create(dhuifhowejifiowe, "VIP")

  

  // system("rm logsPractica");
  // system("touch logsPractica");

  
  //HILOS
  pthread_t recepcionista1,recepcionista2,recepcionistaVip;
  pthread_attr_t tattr;
  
  
  
  signal(SIGUSR1, &creacionClienteNormal );
  signal(SIGUSR2, &creacionClienteVip );

  sleep(40);

  //recepcionistas 
  // pthread_create(&recepcionista1, NULL, accionesRecepcionista1, "Ejecuta recepcionista1?");
  // pthread_create(&recepcionista2, NULL, accionesRecepcionista2, "Ejecuta recepcionista2?");
  // pthread_create(&recepcionistaVip, NULL, accionesRecepcionistaVip, "Ejecuta recepcionistavip?");






//LOG Y EJEMPLOS
  writeLogMessage("1", "buenas ASDFASDFASDFtardes");
  // pthread_mutex_init( &semaforo, NULL);
    return 0;
}


void writeLogMessage(char *id, char *msg) {
  // Calculamos la hora actual
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);
  char stnow[25];
  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
  // Escribimos en el log
  logFile = fopen("logsPractica", "a");
  fprintf(logFile, "[%s] %s: %s\n", stnow, id, msg);
  fclose(logFile);
}

// // SIGUSR1
// crearCliente(1) vip

// SIGUSR2

// crearCliente(0) no vip



/* sR1

if sR1

//signal o sigaction SIGUSR1, cliente normal.
int pthread_create (hilo, 1 ,void *(*start)(void *),void* arg); 

else sR2

int pthread_create (crearCliente, 2 ,void *(*start)(void *),void* arg);  */

void creacionClienteNormal(){
  
  printf("estamos aqui miguel");
  pthread_create(&hiloClientes, NULL, nuevoCliente , (void *)0);

}

void creacionClienteVip(){

  pthread_create(&hiloClientes, NULL, nuevoCliente , (void *)1);
  printf("Cliente vip creado");

}

void *nuevoCliente(void *arg){

    if(contadorClientes<20){


        if(*(int *)arg == 1){     //1 = CLIENTE VIP

          listaClientes[contadorClientes - 1].id = contadorIdClientes;
          contadorIdClientes++;
          
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= true;       
          listaClientes[contadorClientes - 1].ascensor = false;
          printf("Cliente vip creado");

        }else{                    //0 = CLIENTE NORMAL

          listaClientes[contadorClientes - 1].id = contadorIdClientes;
          contadorIdClientes++;
            
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= false;
          listaClientes[contadorClientes - 1].ascensor = false;
          
          printf("Cliente normal creado");
        }
          
    }else{
        //llamada para salir del hilo
        printf("Está el hotel lleno");
    }

    
}