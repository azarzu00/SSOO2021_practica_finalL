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
pthread_mutex_t semaforo, semaforoFichero, maquinas;
pthread_t hiloClientes;

int recepcionistas1;
int recepcionistas2;
int recepcionistasVip;

int maquinasCheckIn[5];
FILE * logFile;

void *nuevoCliente(void *arg);
void writeLogMessage(char *id, char *msg);

void *accionesRecepcionista1(void *arg);
void *accionesRecepcionista2(void *arg);
void *accionesRecepcionistaVip(void *arg);
// // // // // // // // char contadorLog();

void creacionClienteNormal();
void creacionClienteVip();
void finalPrograma();
int calculaAleatorios(int min, int max);
void accionesCliente();
int posicionCliente(int identidad);


struct Cliente{
  int  id;
  bool atendido; 
  bool esVip;
  bool ascensor;
} listaClientes[20];



int main(int argc, char *argv[]){
 
  system("rm logsPractica");
  system("touch logsPractica");

  
  //HILOS
  pthread_t recepcionista1,recepcionista2,recepcionistaVip;
  pthread_attr_t tattr;
  
  printf("asdf");
  
  
  while(!terminado){
    signal(SIGUSR1, &creacionClienteNormal );
    signal(SIGUSR2, &creacionClienteVip );
    signal(SIGTERM, &finalPrograma);
  }

  writeLogMessage("1", "Fin del programa.");

  //recepcionistas 
  // pthread_create(&recepcionista1, NULL, accionesRecepcionista1, "Ejecuta recepcionista1?");
  // pthread_create(&recepcionista2, NULL, accionesRecepcionista2, "Ejecuta recepcionista2?");
  // pthread_create(&recepcionistaVip, NULL, accionesRecepcionistaVip, "Ejecuta recepcionistavip?");






//LOG Y EJEMPLOS
  writeLogMessage("1", "buenas tardes miguel ANGEL");
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

    if(contadorClientes<20){

        if(*(int *)arg == 1){     //1 = CLIENTE VIP

          listaClientes[contadorClientes - 1].id = contadorIdClientes;
          contadorIdClientes++;
          
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= true;       
          listaClientes[contadorClientes - 1].ascensor = false;
          contadorClientes++;
          writeLogMessage("1", "Cliente vip ha llegado");

          accionesCliente();

        }else{                    //0 = CLIENTE NORMAL

          listaClientes[contadorClientes - 1].id = contadorIdClientes;
          contadorIdClientes++;
            
          listaClientes[contadorClientes - 1].atendido = false;
          listaClientes[contadorClientes - 1].esVip= false;
          listaClientes[contadorClientes - 1].ascensor = false;
          contadorClientes++;
          writeLogMessage("1", "Cliente normal ha llegado");

          accionesCliente(listaClientes[contadorClientes - 1].id);
        }
          
    }else{
        //llamada para salir del hilo
        writeLogMessage("1", "El hotel esta lleno");
    } 
}

void accionesCliente(int identidad) {
  

  if(listaClientes[posicionCliente(identidad)].esVip==false){
      writeLogMessage("1", "Un nuevo cliente normal ha entradado");
  }else{
       writeLogMessage("1", "Un nuevo cliente vip ha entrado");  
  }
  
  int eleccion = calculaAleatorios(1, 10);
  

  if (eleccion == 1) {      
    
    //  Van directamente a maquinas

    
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



 