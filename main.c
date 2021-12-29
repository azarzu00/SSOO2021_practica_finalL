/*   COMANDOS PARA MANDAR LAS SEÑALES AL PROGRAMA CUANDO

      a.out

      ps -ef

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
bool terminado = false;
int logs=0;
//semaforos
pthread_mutex_t semaforoCliente, semaforoFichero, maquinas;
pthread_t hiloClientes;
//Recepcionistas
int recepcionistas1;
int recepcionistas2;
int recepcionistasVip;
int maquinasCheckIn[5];
//Clientes
int ordenEntradaClientes[20] = {0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int numeroCola = 0;

struct Cliente{
  int  id;
  bool atendido; 
  bool esVip;
  bool ascensor;
} listaClientes[20];

FILE * logFile;

//Funciones
void *nuevoCliente(void *arg);
void writeLogMessage(char *id, char *msg);
void *accionesRecepcionista1(void *arg);
void *accionesRecepcionista2(void *arg);
void *accionesRecepcionistaVip(void *arg);
void creacionClienteNormal();
void creacionClienteVip();
void finalPrograma();
int calculaAleatorios(int min, int max);
void accionesCliente();
int posicionCliente(int identidad);
void agregarCola(int identidad);






int main(int argc, char *argv[]){
    
  system("rm logsPractica");
  system("touch logsPractica");

  
  //HILOS
  pthread_t recepcionista1,recepcionista2,recepcionistaVip;
  pthread_attr_t tattr;
    
  pthread_mutex_init(&semaforoCliente, NULL); 
  
 
  
  
  while(!terminado){
    signal(SIGUSR1, &creacionClienteNormal );
    signal(SIGUSR2, &creacionClienteVip );
    signal(SIGTERM, &finalPrograma);
  }

  pthread_mutex_destroy(&semaforoCliente); 
  writeLogMessage("1", "Fin del programa.");

  

  //recepcionistas 
  // pthread_create(&recepcionista1, NULL, accionesRecepcionista1, "Ejecuta recepcionista1?");
  // pthread_create(&recepcionista2, NULL, accionesRecepcionista2, "Ejecuta recepcionista2?");
  // pthread_create(&recepcionistaVip, NULL, accionesRecepcionistaVip, "Ejecuta recepcionistavip?");
    

    for (int i = 0; i<20; i++) {

      printf("%d", ordenEntradaClientes[i]);

    }

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


void finalPrograma(){
  
  terminado = true;

}

void creacionClienteNormal(){
  
  int vip = 0;
  pthread_create(&hiloClientes, NULL, nuevoCliente , &vip);

}

void creacionClienteVip(){

  
  int vip=1;
  pthread_create(&hiloClientes, NULL, nuevoCliente , &vip);


}

void *nuevoCliente(void *arg){

   pthread_mutex_lock(&semaforoCliente);  //cuidado PRECAUCION PELIGROSO ADVERTENCIA MUCHO PELIGROSO ALTA TENSION

    if(contadorClientes<20){

        if(*(int *)arg == 0){     //0 = CLIENTE NORMAL

          listaClientes[contadorClientes - 1].id = contadorIdClientes;

          printf("ID jaja miguel  %d", contadorIdClientes);
          printf("\n");
          
          ordenEntradaClientes[numeroCola] = contadorIdClientes;
          contadorIdClientes++;
          numeroCola++;
          
          
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= true;       
          listaClientes[contadorClientes - 1].ascensor = false;
          contadorClientes++;
          writeLogMessage("1", "Un cliente NORMAL ha llegado.");

          accionesCliente(listaClientes[contadorClientes - 1].id);

        }else{                    //1 = CLIENTE VIP

          listaClientes[contadorClientes - 1].id = contadorIdClientes;

          printf("ID jaja miguel  %d", contadorIdClientes);
          printf("\n");

          ordenEntradaClientes[numeroCola] = contadorIdClientes;
          contadorIdClientes++;
          numeroCola++;
            
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= false;
          listaClientes[contadorClientes - 1].ascensor = false;
          contadorClientes++;
          writeLogMessage("1", "Un cliente VIP ha llegado.");

          accionesCliente(listaClientes[contadorClientes - 1].id);
        }
          
    }else{
        //llamada para salir del hilo
        writeLogMessage("1", "El hotel está lleno.");
    } 
    pthread_mutex_unlock(&semaforoCliente);
    pthread_exit(NULL);
}

void accionesCliente(int identidad) {
  


  // if(listaClientes[posicionCliente(identidad)].esVip==false){
  //     writeLogMessage("1", "Un nuevo cliente normal ha entrado");
  // }else{
  //      writeLogMessage("1", "Un nuevo cliente vip ha entrado");  
  // }
  
  int eleccion = calculaAleatorios(1, 10);
  

  if (eleccion == 1) {                  //Van directamente a maquinas
 
    

    
  } else {    
    
    eleccion = calculaAleatorios(1, 10);
    
    if(eleccion==2 || eleccion == 3){   //Se cansan de esperar y se vaan a maquinas
    

    } else if (eleccion==4){            //Se cansa de esperar y se marcha 

    





    } else {    

        eleccion = calculaAleatorios( 1, 20);

        if(eleccion == 1) {             //Se cansan, van al baño y se marcha del hotel

          
        } else {                        //Los que se quedan definitivamente y van con los recepcionistas


          

          

          

          
        }
    } 
  }

//LLAMADARECEPCIONISTA(ID);



}

// void agregarCola(int identidad) {

//   int i = 0;
//   while (ordenEntradaClientes[i] != 0) {

//     i++;
//   }
//   ordenEntradaClientes[i] = identidad;
// }


int posicionCliente(int identidad) {

  for(int i=0; i<20; i++){
   if(listaClientes[i].id==identidad){
     return i;
   }
  }
  return -1;
}


int calculaAleatorios(int min, int max) {
  return rand() % (max-min+1) + min;
}

// // // // // // // // char contadorLog(){

// // // // // // // //   char cadena[] = "";

// // // // // // // //   //itoa();

// // // // // // // //   return cadena;
// // // // // // // // }