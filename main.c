/*   COMANDOS PARA MANDAR LAS SEÑALES AL PROGRAMA CUANDO

      a.out

      ps -ef

      kill -s SIGUSR1 pid
      kill -s SIGUSR2 pid
      kill -s SITERM pid   





      pos=posicionCliente(ordenRecepcionista[0]);
          if(listaClientes[i].tipo==1){
            encontrado=true;
            writeLogMessage("1", "Entra recepcionista 1 ");

          }
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
int ascensor;
int terminado = 0;
int logs=0;
int descanso1=0;
int descanso2=0;

//semaforos
pthread_mutex_t semaforoCliente, semaforoFichero, maquinas, semaforoRecepcionistas, semaforoCheckIn;
pthread_t hiloClientes, recepcionista1, recepcionista2, recepcionistaVip;

//Recepcionistas
int recepcionistas1;
int recepcionistas2;
int recepcionistasVip;
int maquinasCheckIn[5];

//Clientes
int ordenRecepcionista[20] = {0};
int ordenCheckin[20] = {0};
int numeroCola = 0;
int numeroCheckin = 0;
int listaAscensor[6] = {0};

//1 NORMAL, 2 VIP, 3 MAQUINAS
struct Cliente{
  int  id;
  int atendido; //0 false 1 true
  int tipo;
  int ascensor; //0 false 1 true
} listaClientes[20];

bool maquinasOcupadas[5] = {false, false, false, false, false};

FILE * logFile;

//Funciones
void *nuevoCliente(void *arg);
void writeLogMessage(char *id, char *msg);
void writeLogMessageConVariable(char *id, char *msg, int i);
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
bool hayHueco();
void accionesCheckin(int identidad);
void quitarVacioCola(int cola);
void regulacionClientes();
void eliminarCliente(int identidad);
void ascensorEscalera(int identidad);




int main(int argc, char *argv[]){
  
  srand(getpid());

  system("rm logsPractica");
  system("touch logsPractica");

  
  //HILOS
  pthread_attr_t tattr;
    
  pthread_mutex_init(&semaforoCliente, NULL); 
  pthread_mutex_init(&semaforoCheckIn, NULL);
  pthread_mutex_init(&semaforoRecepcionistas, NULL);

  pthread_create(&recepcionista1, NULL, accionesRecepcionista1, "Ejecuta recepcionista1");
  pthread_create(&recepcionista2, NULL, accionesRecepcionista2, "Ejecuta recepcionista2");
  pthread_create(&recepcionistaVip, NULL, accionesRecepcionistaVip, "Ejecuta recepcionistaVIP");
  
 
  signal(SIGUSR1, &creacionClienteNormal );
  signal(SIGUSR2, &creacionClienteVip );
  signal(SIGTERM, &finalPrograma);
  
  while(terminado == 0){
    pause();
  }


  //mover a funcion de cierre de programa
  pthread_mutex_destroy(&semaforoCliente); 
  pthread_mutex_destroy(&semaforoCheckIn);
  pthread_mutex_destroy(&semaforoRecepcionistas);
 // writeLogMessage("1", "Fin del programa");

  

  //recepcionistas 

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
  fprintf(logFile, "[%s] %s: %s \n", stnow, id, msg);
  fclose(logFile);
}

void writeLogMessageConVariable(char *id, char *msg, int i) {
  // Calculamos la hora actual
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);
  char stnow[25];
  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
  // Escribimos en el log
  logFile = fopen("logsPractica", "a");
  fprintf(logFile, "[%s] %s: %s %d \n", stnow, id, msg, i);
  fclose(logFile);
}


void finalPrograma(){
  
  terminado = 1;

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

  //pthread_mutex_lock(&semaforoCliente); 
  bool encontrado = false;
  int i = 0;

  for (i = 0; i < 20 && !encontrado; i++) {

    if (listaClientes[i].id == 0) {
    encontrado = true;

    }

  }

    if(contadorClientes<20){

        if(*(int *)arg == 0){     //0 = CLIENTE NORMAL

          listaClientes[i].id = contadorIdClientes;

          writeLogMessageConVariable("1", " ESTAMOS EN NUEVOCLIENTE ----SE LE HA PUESTO EL ID ", listaClientes[i].id);

          printf("ESTAMOS EN CREACION DE UN NUEVO CLIENTE NORMAL  %d", contadorIdClientes);
          //writeLogMessage("1", "Hola todo bien estoy en creacion del cliente ok");
          printf("\n");

          writeLogMessageConVariable("1", "Un cliente NORMAL ha llegado y se le asigna el ID.( ͡👁️ ͜ʖ ͡👁️)", contadorIdClientes);
          
          contadorIdClientes++;
          
          listaClientes[i].atendido = 0;
          listaClientes[i].tipo= 1;       
          listaClientes[i].ascensor = 0;
          contadorClientes++;
          pthread_mutex_unlock(&semaforoCliente);

          accionesCliente(listaClientes[i].id);

        }else{                    //1 = CLIENTE VIP

          listaClientes[i].id = contadorIdClientes;

          printf("ID %d", contadorIdClientes);
          printf("\n");

          contadorIdClientes++;
            
          listaClientes[i].atendido = 0;
          listaClientes[i].tipo= 2;
          listaClientes[i].ascensor = 0;
          contadorClientes++;
          writeLogMessage("1", "Un cliente VIP ha llegado.");

          pthread_mutex_unlock(&semaforoCliente);
          
          accionesCliente(listaClientes[i].id);
        }
          
    }else{
        //llamada para salir del hilo
        writeLogMessage("1", "El hotel está lleno.");
    } 
    

    //pthread_exit(NULL); lo haremos cuando los clientes se vayan a la escalera o cuando hayan entrado en el ascensor
}


void accionesCliente(int identidad) {
  

  int eleccion = calculaAleatorios(1, 10);
  

  if (eleccion == 1) {                  //El cliente va directamente a maquinas de checkin    
                                        //metodo para ir a maquinas de checkin
    writeLogMessage("1", "me voy a chec");                       //Si llegan y no quedan maquinas libres se van a recepcionistas

    //CAMBIAR EL TIPO DE CLILENTES  
                           
    accionesCheckin(identidad);   
    
    
    
    
                                         //Habria que añadirlo a la cola de clientes (ordenEntradaClientes)
  } else {
    
    eleccion = calculaAleatorios(1, 10);
    writeLogMessageConVariable("1", "ACCIONES CLIENTE numero aleatorio tomado",  eleccion);
    
    if(eleccion==2 || eleccion == 3){   //El cliente se cansan de esperar en cola y se van a maquinas de checkIn

    writeLogMessage("1", "me voy a chec"); 
    accionesCheckin(identidad);
                                        //metodo para ir a maquinas de checkin
                                        //En las maquinas checking el cliente compreba si hay maquinas libres, si no las hay espera 3s en los que hay un 50% de irse 
    

    } else if (eleccion==4){            //El cliente se cansa de esperar en cola y se marcha del hotel

    
      writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE CANSA DE ESPERAR( ͡👁️ ͜ʖ ͡👁️)");
      eliminarCliente(identidad);


    } else {    

        eleccion = calculaAleatorios( 1, 20);

        if(eleccion == 1) {             //El cliente se cansa de la cola, van al baño y se marcha del hotel

          writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE VA A LAVARSE LAS MANOS Y PIERDE SU TURNO( ͡👁️ ͜ʖ ͡👁️)");
          eliminarCliente(identidad);
          
        } else {                        //El cliente se queda definitivamente y va con los recepcionistas

          writeLogMessage("1", "EL CLIENTE SE VA A LA COLA A ESPERAR QUE LOS RECEPCIONISTAS LE LLAMEN ( ͡👁️ ͜ʖ ͡👁️)");       
          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++;
          //wtfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
           writeLogMessage("1", "NUMEROCOLA SE HA AUMENTADO");
           //cerramos?

           for (int i = 0; i<20; i++) {

             writeLogMessageConVariable("1", "orden de recepcionistas: ", ordenRecepcionista[i]);
           }
        }
    } 
  }



  while (listaClientes[posicionCliente(identidad)].atendido == 0) {
    //jiji
  }

  if (listaClientes[posicionCliente(identidad)].tipo == 3 || listaClientes[posicionCliente(identidad)].tipo == 4) {
    writeLogMessage("1", "HEMOSSSSSSS SALIDOOOOOOO DE MAKINAASASSSASASASASAASS");
  }

  if (listaClientes[posicionCliente(identidad)].tipo == 1 || listaClientes[posicionCliente(identidad)].tipo == 2) {
    writeLogMessageConVariable("1", "HEMOS LLEGADO A REGULACION CLINTES", 1);
    regulacionClientes(identidad);
  }

  writeLogMessageConVariable("1", "HEMOS LLEGADO A ASCENSORES VAMOSOOOOOOOOOOOOOSSS", 1);
  
  while (listaClientes[posicionCliente(identidad)].ascensor == 0) {
    //jiji
  }

 



  //llamada a metodo ascensores escaleras
  
}


void accionesCheckin(int identidad) {

    int parar = 0;
    int irse = 0;
    int aleatorio;

    if (listaClientes[posicionCliente(identidad)].tipo == 1) {

        listaClientes[posicionCliente(identidad)].tipo = 3;  
    } else if (listaClientes[posicionCliente(identidad)].tipo == 2) {

        listaClientes[posicionCliente(identidad)].tipo = 4;  
    }
       
     while (parar == 0) {

       if (hayHueco()) {

         //pthread_mutex_lock(&semaforoCheckIn);
         
         for (int i = 0; i < 5 && parar == 0; i++) {

           if (maquinasOcupadas[i] == false) {

              maquinasOcupadas[i] = true;
              parar = 1;

              pthread_mutex_unlock(&semaforoCheckIn);
              
              writeLogMessage("1", "EL CLIENTE ESTA USANDO UNA MAKINA");

              sleep(6);                             //Espera 6 segundos al ocupar la maquina de checkin

             // pthread_mutex_lock(&semaforoCheckIn);
              
              maquinasOcupadas[i] = false;          //Termina de usar la máquina y debería irse al ascensor
              pthread_mutex_unlock(&semaforoCheckIn);
                                                    
              listaClientes[posicionCliente(identidad)].atendido = 1;
                                                  

              writeLogMessage("1", "EL CLIENTE HA TERMINADO DE USAR LA MAKINA");
           }
         }
       
       } else {             //Si no hay huecos hay un 50% de que decida irse a recepcionistas

        aleatorio = calculaAleatorios(1, 10);
              writeLogMessage("1", "NO HAY HU");

        if (aleatorio < 6) {

          irse = 1;
          parar = 1;

          if (listaClientes[posicionCliente(identidad)].tipo == 3) {

            listaClientes[posicionCliente(identidad)].tipo = 1;  
          } else if (listaClientes[posicionCliente(identidad)].tipo == 4) {         //tipo = 3 fue a cheeckin y era normal, tipo = 4 fue a checkin y era vip

            listaClientes[posicionCliente(identidad)].tipo = 2;
          }

          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++;
          
        } else {    //vuelve a intentar acceder a maquinas

          sleep(3);
          
          accionesCheckin(identidad);
                              //EL CLIENTE VUELVE A LLAMAR A ESTE MISMO METODO PARA QUE REPITA EL PROCESO
        }
       }
     }

  
}

bool hayHueco() {

  bool hueco = false;

  for (int i = 0; i < 5; i++) {

    if (maquinasOcupadas[i] == false) {

      hueco = true;
    }
  }

  return hueco;
}

//1 NORMAL, 2 VIP, 3 MAQUINAS
//RECEPCIONISTAS 1 MAQUINAS 2

//RECEPCIONISTAS
void *accionesRecepcionista1(void *arg){      
    

  int pos;
   

  //writeLogMessage("1", "inicio recepcionista 1");
  
  while(true){

   //pthread_mutex_lock(&semaforoRecepcionistas);
   

    if(numeroCola>0){
      if(descanso1==5){
         writeLogMessage("1", "Recepcionista 1 descansando");

          sleep(5);
          descanso1=0;
      }
      
       //decidir que cliente escogemos
        int i = 0;
        bool encontrado = false;
        
       // pthread_mutex_lock(&semaforoRecepcionistas);  //inicioMtux
        
        while(i<20 && !encontrado){
          
          if (ordenRecepcionista[i] != 0) {

            encontrado = true;
            
          }

          i++;
        }        

        i--;

         writeLogMessageConVariable("1","LA I DEL RECEPCIONISTA 1 ES: ", i);

        if(encontrado==true ){


           writeLogMessage("1", "REPITE recepcionista 1");
          // regulacionClientes(ordenRecepcionista[i]);

          int copiaID = ordenRecepcionista[i];

          ordenRecepcionista[i]=0;
          quitarVacioCola(1);
          
          listaClientes[posicionCliente(copiaID)].atendido = 1;

          while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
            //jiji
           
          }

          
          descanso1++;

          pthread_mutex_unlock(&semaforoRecepcionistas); 
          //finalMutex
     
           
        }else{
          sleep(1);
        }
        }else{
        sleep(1);  
      }
    }
}

void *accionesRecepcionista2(void *arg){


  int pos;
   

  writeLogMessage("1", "inicio recepcionista 2");
  
  while(true){

   //pthread_mutex_lock(&semaforoRecepcionistas);
   

    if(numeroCola>0){
      if(descanso1==5){
         writeLogMessage("1", "Recepcionista 2 descansando");

          sleep(5);
          descanso2=0;
      }
      
       //decidir que cliente escogemos
        int i=0;
        bool encontrado = false;
        
       // pthread_mutex_lock(&semaforoRecepcionistas);  //inicioMtux
        
        while(i<20 && !encontrado){
          
          if (ordenRecepcionista[i] != 0) {

            encontrado = true;
            
          }
          i++;
        }

        i--;

        writeLogMessageConVariable("1","LA I DEL RECEPCIONISTA 2 ES: ", i);

        if(encontrado==true ){
          
          writeLogMessageConVariable("1","HA ENTRADO EN EL RECEPCIONISTA NORMAL, SU ID ES: ", ordenRecepcionista[i]);
           writeLogMessage("1", "REPITE recepcionista 2");
          // regulacionClientes(ordenRecepcionista[i]);

          int copiaID = ordenRecepcionista[i];

          ordenRecepcionista[i]=0;
          quitarVacioCola(1);
          
          listaClientes[posicionCliente(copiaID)].atendido = 1;

          while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
            //jiji

          }

          
          descanso2++;

          pthread_mutex_unlock(&semaforoRecepcionistas); 
          //finalMutex
     
           
        }else{
          sleep(1);
        }
        }else{
        sleep(1);  
      }
    }
}



void *accionesRecepcionistaVip(void *arg){

  int pos;
  
//  writeLogMessage("1", "inicio recepcionista VIP ");
  
  while(true){ 
    
    writeLogMessageConVariable("1", "contadorIdClientes: ", contadorIdClientes);

//  writeLogMessage("1", "REPITE recepcionista VIP");

    if(numeroCola>0){
      
     
       //decidir que cliente escogemos

      bool encontrado=false;

      int i;
      //pthread_mutex_lock(&semaforoRecepcionistas);  //inicioMtux
      for(i=0; i<numeroCola; i++){

        pos=posicionCliente(ordenRecepcionista[i]);
        if(listaClientes[pos].tipo==2){

        writeLogMessage("1", "Entra en recepcionista vip??? ");
        
          encontrado=true;
        }

      }
      if(encontrado ){ 

        int copiaID = ordenRecepcionista[i];

        ordenRecepcionista[i]=0;
        quitarVacioCola(1);

        listaClientes[posicionCliente(copiaID)].atendido = 1;

        while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
          //jiji
        }

        pthread_mutex_unlock(&semaforoRecepcionistas);
       
        

      } else {
        
        //pthread_mutex_unlock(&semaforoRecepcionistas);
        sleep(1);
      }
    }else{
      //writeLogMessage("1", "hacemos un sleep VIP");
      sleep(1);
    }
  }

  //include pthread exit
}


void ascensorEscalera(int identidad) {


writeLogMessage("1", "ASCENSOR ESKALERA");


  int aleatorio;
  bool encontrado = false;

  aleatorio = calculaAleatorios(1, 10);

  if (aleatorio > 0 && aleatorio < 8) {

    writeLogMessage("1", "El cliente se va por las escaleras.");

  } else {
    
    while (!encontrado) {

      for (int i = 0; i < 6 && !encontrado; i++) {

        if (listaAscensor[i] == 0) {
          listaAscensor[i] = identidad;
          encontrado = true;
          
        }   //TERMINAR
     }
    }
  }
}


int posicionCliente(int identidad) {

  for(int i=0; i<20; i++) {
   if(listaClientes[i].id==identidad){
     return i;
   }
  }
  return -1;
}



int calculaAleatorios(int min, int max) {
  return rand() % (max-min+1) + min;
}

void quitarVacioCola(int cola) {  
  
  //puede que haya que meter mutex para controlar el acceso
  //OTRO MUTEX
  bool encontrado = false;

  if (cola == 1){

    for (int i = 0; i < 19; i++) {
      if (ordenRecepcionista[i] == 0) {

        encontrado = true;  
      }
      if (encontrado) {

        ordenRecepcionista[i] = ordenRecepcionista[i+1];
      }
    }
    numeroCola--;
    ordenRecepcionista[19] = 0;

  }else{    //lista de los checkin
    
    for (int i = 0; i < 19; i++) {
      if (ordenCheckin[i] == 0) {

        encontrado = true; 
      }
      if (encontrado) {

        ordenCheckin[i] = ordenCheckin[i+1];
      }
    }

    numeroCheckin--;
    ordenCheckin[19] = 0;
  }
}


void regulacionClientes(int identidad) {   //decidimos lo que pasa a los clientes antes de irse
  
  srand(getpid());
  
  int aleatorio = calculaAleatorios(1, 10);
  writeLogMessageConVariable("1", "Pasamos a REGULACIONES, a ver si hay suerte", aleatorio);
  
  
  if(aleatorio==1 || aleatorio==2 ){    //20 %

    if(aleatorio==1){ //10% mal identificados

      writeLogMessage("1", "REGULACIONES El cliente está mal identificado, se va a escaleras o ascensor");
      sleep(calculaAleatorios(2, 6));
      writeLogMessageConVariable("1", "REGULACON CLIENTES numero aleatorio obtenido para el sleep de que se va a ESCALERAS O ASCENSOR", calculaAleatorios(2, 6));
      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      //llamada para saber si va a ascensores o no
                  
    }else if(aleatorio==2){ //10% sin pasaporte covid abandonan el hotel  
      writeLogMessage("1", "REGULACIONES El cliente no tiene pasaporte covid, se marcha del hotel");
      
      int espera = 0;
      writeLogMessageConVariable("1", "REGULACON CLIENTES numero aleatorio obtenido para el sleep DE QUE SE MARCHA DEL HOTEL", espera);
      espera = sleep(calculaAleatorios(6, 10));


      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      
      //SE VAN DEL HOTEL, PODRIAMOS DEJAR ESTO TAL CUAL
      //COGER EL ID DEL CLIENTE Y CAMBIAR A 0 TODOS SUS PARAMETROS

      writeLogMessage("1", "REGULACIONES cliente eliminado");
      eliminarCliente(identidad);
      //ELIMINAR DE LA COLA DE RECEPCIONISTAS

    }   
  }else{              //80% de los clientes: todo en orden
          
      writeLogMessage("1", "REGULACIONES El cliente lo tiene todo en orden, se va a escaleras o ascensor");
      sleep(calculaAleatorios(1, 4));
      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      //llamada para saber si va a ascensores o no

    }

}

void eliminarCliente(int identidad){

      listaClientes[posicionCliente(identidad)].id=0;
      listaClientes[posicionCliente(identidad)].atendido=0;
      listaClientes[posicionCliente(identidad)].tipo=0;
      listaClientes[posicionCliente(identidad)].ascensor=0;

      contadorClientes--;

      pthread_exit(&hiloClientes);
}