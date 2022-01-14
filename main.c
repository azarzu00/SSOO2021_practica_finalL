
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
pthread_mutex_t semaforoCliente, semaforoFichero, semaforoRecepcionistas, semaforoCheckIn, semaforoLogs, semaforoAscensor;
pthread_t hiloClientes, recepcionista1, recepcionista2, recepcionistaVip;

pthread_cond_t condicion;

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
int clientesEsperando = 0;

struct Cliente{
  int  id;
  int atendido; //0 NO ATENDIDO 1 ATENDIENDO 2 ATENDIDO
  int tipo; //1 normal, 2 vip, 3 fue a cheeckin y era normal,  4 fue a checkin y era vip
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
  pthread_mutex_init(&semaforoLogs, NULL);
  pthread_mutex_init(&semaforoAscensor, NULL);

  if (pthread_cond_init(&condicion, NULL) != 0) exit(-1);


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
  pthread_mutex_destroy(&semaforoFichero);
 // writeLogMessage("1", "Fin del programa");

  

  //recepcionistas 

    return 0;
}





void writeLogMessage(char *id, char *msg) {
  pthread_mutex_lock(&semaforoFichero);
  // Calculamos la hora actual
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);
  char stnow[25];
  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
  // Escribimos en el log
  logFile = fopen("logsPractica", "a");
  fprintf(logFile, "[%s] %s: %s \n", stnow, id, msg);
  fclose(logFile);
  pthread_mutex_unlock(&semaforoFichero);
}

void writeLogMessageConVariable(char *id, char *msg, int i) {
  pthread_mutex_lock(&semaforoFichero);
  // Calculamos la hora actual
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);
  char stnow[25];
  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
  // Escribimos en el log
  logFile = fopen("logsPractica", "a");
  fprintf(logFile, "[%s] %s: %s %d \n", stnow, id, msg, i);
  fclose(logFile);
  pthread_mutex_unlock(&semaforoFichero);
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

  
  int encontrado = 0;
  int i = 0;

  pthread_mutex_lock(&semaforoCliente); 
  for (i = 0; i < 20 && encontrado==0; i++) {

    if (listaClientes[i].id == 0) {
    encontrado = 1;

    }

  }
  pthread_mutex_unlock(&semaforoCliente); 


    pthread_mutex_lock(&semaforoCliente); 
    if(contadorClientes<20){
        
        if(*(int *)arg == 0){     //0 = CLIENTE NORMAL


          
          
          listaClientes[i].id = contadorIdClientes;

         

          writeLogMessageConVariable("1", " ESTAMOS EN NUEVOCLIENTE ----SE LE HA PUESTO EL ID ", listaClientes[i].id);

          printf("ESTAMOS EN CREACION DE UN NUEVO CLIENTE NORMAL  %d", contadorIdClientes);
          //writeLogMessage("1", "Hola todo bien estoy en creacion del cliente ok");
          printf("\n");

          writeLogMessageConVariable("1", "Un cliente NORMAL ha llegado y se le asigna el ID: ", contadorIdClientes);
          
     
     
          contadorIdClientes++;
          
          listaClientes[i].atendido = 0;
          listaClientes[i].tipo= 1;       
          listaClientes[i].ascensor = 0;
          contadorClientes++;
 
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

        
          
          accionesCliente(listaClientes[i].id);     //cuidado
          
        }
          
    }else{
        //llamada para salir del hilo
        writeLogMessage("1", "El hotel está lleno.");
         pthread_mutex_unlock(&semaforoCliente); 
        
    } 
    

    //pthread_exit(NULL); lo haremos cuando los clientes se vayan a la escalera o cuando hayan entrado en el ascensor
}


void accionesCliente(int identidad) {
  
  pthread_mutex_unlock(&semaforoCliente);
  
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
                                        //En las maquinas checking el cliente compreba si hay
                                        // maquinas libres, si no las hay espera 3s 
                                        // en los que hay un 50% de irse 
    

    } else if (eleccion==4){            //El cliente se cansa de esperar en cola y se marcha del hotel

    
      writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE CANSA DE ESPERAR");
      eliminarCliente(identidad);


    } else {    

        eleccion = calculaAleatorios( 1, 20);
        writeLogMessageConVariable("1", " Cliente puede /cansarse de la cola / quedarse e irse con recepcionistas/ ", eleccion);       
        if(eleccion == 1) {             //El cliente se cansa de la cola, van al baño y se marcha del hotel

          writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE VA A LAVARSE LAS MANOS Y PIERDE SU TURNO");
          eliminarCliente(identidad);
          
        } else {                        //El cliente se queda definitivamente y va con los recepcionistas

          writeLogMessage("1", "EL CLIENTE SE VA A LA COLA A ESPERAR QUE LOS RECEPCIONISTAS LE LLAMEN");       
          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++;

           writeLogMessage("1", "NUMEROCOLA SE HA AUMENTADO");
           //cerramos?

           
        }
    } 
  }

  writeLogMessage("1", "NOS METEMOS EN EL PRIMER WHILE");

  while (listaClientes[posicionCliente(identidad)].atendido == 0) {
    //ESPERA
  }

  writeLogMessage("1", "SALIMOS DEL PRIMER WHILE");

  if (listaClientes[posicionCliente(identidad)].tipo == 3 || listaClientes[posicionCliente(identidad)].tipo == 4) {
    writeLogMessage("1", "SALE DE MAQUINAS");
  }

  
  if(listaClientes[posicionCliente(identidad)].tipo == 1 || listaClientes[posicionCliente(identidad)].tipo == 2) {
    writeLogMessageConVariable("1", "LLEGA A REGULACION CLINTES", 1);
    regulacionClientes(identidad);
  }

  writeLogMessageConVariable("1", "LLEGA A ASCENSORES", 1);
  
  ascensorEscalera(identidad);
 

 //llamada a funcion ascensores escaleras
  
}


void accionesCheckin(int identidad) {

    int parar = 0;
    int irse = 0;
    int aleatorio;

    
    pthread_mutex_lock(&semaforoCliente);
    
    if (listaClientes[posicionCliente(identidad)].tipo == 1) {

        listaClientes[posicionCliente(identidad)].tipo = 3;  
    } else if (listaClientes[posicionCliente(identidad)].tipo == 2) {

        listaClientes[posicionCliente(identidad)].tipo = 4;  
    }

    pthread_mutex_unlock(&semaforoCliente);
       
     while (parar == 0) {

        pthread_mutex_lock(&semaforoCheckIn);

       if (hayHueco()) {

      
         for (int i = 0; i < 5 && parar == 0; i++) {

           if (maquinasOcupadas[i] == false) {

              maquinasOcupadas[i] = true;
              parar = 1;

              pthread_mutex_unlock(&semaforoCheckIn);
              
              writeLogMessage("1", "EL CLIENTE ESTA USANDO UNA MAQUIINA");

              sleep(6);                             //Espera 6 segundos al ocupar la maquina de checkin

             //thread_mutex_lock(&semaforoCheckIn);

              pthread_mutex_lock(&semaforoCheckIn);
              
              maquinasOcupadas[i] = false;          //Termina de usar la máquina y debería irse al ascensor
                                                    
              listaClientes[posicionCliente(identidad)].atendido = 1;
              
              pthread_mutex_unlock(&semaforoCheckIn);                              

              writeLogMessage("1", "EL CLIENTE HA TERMINADO DE USAR LA MAQUINA");
           }
         }
       
       } else {             //Si no hay huecos hay un 50% de que decida irse a recepcionistas
//Varios mutes variabñes
         pthread_mutex_unlock(&semaforoCheckIn);
         aleatorio = calculaAleatorios(1, 10);
         writeLogMessage("1", "NO HAY HUECO");

          if (aleatorio < 6) {

          irse = 1;
          parar = 1;

          pthread_mutex_lock(&semaforoCliente);  
          
          if (listaClientes[posicionCliente(identidad)].tipo == 3) {

            listaClientes[posicionCliente(identidad)].tipo = 1;  
            
            
          } else if (listaClientes[posicionCliente(identidad)].tipo == 4) {


            listaClientes[posicionCliente(identidad)].tipo = 2; 
          }
          pthread_mutex_unlock(&semaforoCliente);  

          
          pthread_mutex_lock(&semaforoRecepcionistas);
          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++;
          pthread_mutex_unlock(&semaforoRecepcionistas);

           

          
        } else {    //vuelve a intentar acceder a maquinas

          pthread_mutex_unlock(&semaforoCheckIn);

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

   
   pthread_mutex_lock(&semaforoRecepcionistas);

    if(numeroCola>0){

      if(descanso1==5){
         writeLogMessage("1", "Recepcionista 1 descansando");
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(5);
          descanso1=0;
          pthread_mutex_lock(&semaforoRecepcionistas);
      }
      
       //decidir que cliente escogemos
        int i = 0;
        int encontrado = 0;
        
        
        while(i<20 && encontrado==0){

          
          
          if (ordenRecepcionista[i] != 0) {

            encontrado = 1;
            
          }

          i++;
        }    

        i--;

         writeLogMessageConVariable("1","LA I DEL RECEPCIONISTA 1 ES: ", i);

        if(encontrado==1 ){


          writeLogMessage("1", "entramos en recepcionista 1");
          // regulacionClientes(ordenRecepcionista[i]);

          int copiaID = ordenRecepcionista[i];

          ordenRecepcionista[i]=0;
          quitarVacioCola(1);
          
          listaClientes[posicionCliente(copiaID)].atendido = 1;

          

          for (int i = 0; i<20; i++) {

             writeLogMessageConVariable("1", "orden de recepcionistas: ", ordenRecepcionista[i]);
           }

          while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
            //jiji
           
          }

          writeLogMessageConVariable("1", "EL CLIENTE HA SIDO ATENDIDO", ordenRecepcionista[i]);

          descanso1++;

    
           
        }else{
          
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
        }
        }else{
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);  
      }
    }
}

void *accionesRecepcionista2(void *arg){


  int pos;
  int i;
  int encontrado;
   

  writeLogMessage("1", "inicio recepcionista 2");
  


  while(true){

   pthread_mutex_lock(&semaforoRecepcionistas);
   

    if(numeroCola>0){
      if(descanso2==5){
         writeLogMessage("1", "Recepcionista 2 descansando");
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(5);
          descanso2=0;
          pthread_mutex_lock(&semaforoRecepcionistas);
      }
      
       //decidir que cliente escogemos
        i=0;
        encontrado = 0;
        

        
        while(i<20 && encontrado==0){
          
          if (ordenRecepcionista[i] != 0) {

            encontrado = 1;
            
          }
          i++;
        }

        i--;
        writeLogMessageConVariable("1","LA I DEL RECEPCIONISTA 2 ES: ", i);

        if(encontrado==1 ){
          
          writeLogMessageConVariable("1","entramos en recepcionista 2, el id es: ", ordenRecepcionista[i]);
         
          // regulacionClientes(ordenRecepcionista[i]);


          int copiaID = ordenRecepcionista[i];

          ordenRecepcionista[i]=0;
          quitarVacioCola(1);
          
          listaClientes[posicionCliente(copiaID)].atendido = 1;

          pthread_mutex_unlock(&semaforoRecepcionistas);

          while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
            //jiji

          }
          
          descanso2++;
     
           
        }else{
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
        }
        }else{
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);  
      }
    }
}



void *accionesRecepcionistaVip(void *arg){

  int pos;
  int encontrado;
  int i;
  int copiaID;
  
//  writeLogMessage("1", "inicio recepcionista VIP ");
  
  while(true){ 
    
    pthread_mutex_lock(&semaforoRecepcionistas);

    writeLogMessageConVariable("1", "contadorIdClientes: ", contadorIdClientes);

//  writeLogMessage("1", "REPITE recepcionista VIP");
    // pthread_mutex_lock(&semaforoRecepcionistas);  //inicioMtux
    if(numeroCola>0){
      
       //decidir que cliente escogemos

      encontrado=0;

     
      for(i=0; i<numeroCola; i++){

        pos=posicionCliente(ordenRecepcionista[i]);
        if(listaClientes[pos].tipo==2){

        writeLogMessage("1", "Entra en recepcionista vip ");
        
          encontrado=1;
        }

      }
      if(encontrado == 1){ 

        copiaID = ordenRecepcionista[i];

        ordenRecepcionista[i]=0;
        quitarVacioCola(1);

        listaClientes[posicionCliente(copiaID)].atendido = 1;

          
       // pthread_mutex_unlock(&semaforoRecepcionistas);

        while(listaClientes[posicionCliente(copiaID)].atendido != 2) {
          //jiji
        }

      } else {
        
        pthread_mutex_unlock(&semaforoRecepcionistas);
        sleep(1);
      }
    }else{
      //writeLogMessage("1", "hacemos un sleep VIP");
      pthread_mutex_unlock(&semaforoRecepcionistas);
      sleep(1);
    }
  }

  //include pthread exit
}


void ascensorEscalera(int identidad) {

writeLogMessage("1", "INICIA LA FUNCION DE ASCENSOR");


  int aleatorio;
  int encontrado = 0;

  aleatorio = 9;
  
  //calculaAleatorios(1, 10);

  if (aleatorio > 0 && aleatorio < 8) {

    writeLogMessage("1", "El cliente se va por las escaleras. :(");

  } else {
      writeLogMessage("1", "El cliente se va a secensor. :)");

    // pthread_mutex_lock(&semaforoAscensor);
    clientesEsperando++;
    
    if (clientesEsperando >= 6) {

      // pthread_mutex_unlock(&semaforoAscensor);
      
       writeLogMessage("1", "ASCENSOR SUBIENDO");
      sleep(6);


      pthread_cond_signal(&condicion);
    }else{
      pthread_mutex_unlock(&semaforoAscensor);
    }

    pthread_cond_wait(&condicion, &semaforoAscensor);

    sleep(calculaAleatorios(3, 6));

    writeLogMessage("1", "UN CLIENTE HA PASADO EL WAIT");

   //  pthread_mutex_lock(&semaforoAscensor);
    if (clientesEsperando != 0) {
      
      //pthread_mutex_unlock(&semaforoAscensor);
      
      writeLogMessage("1", "DEBERIA MANDAR LA SEGUNDA SEÑAL");
      pthread_cond_signal(&condicion);
      
    } else {
        writeLogMessage("1", "HAN SALIDO TODOS DEL ASCENSOR");

      // pthread_mutex_unlock(&semaforoAscensor);
    }


      writeLogMessage("1", "UN CLIENTE HA SALIDO DEL ASCENSOR");

      eliminarCliente(identidad);
    
    
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
  
  int aleatorio = 5;
  
  //  calculaAleatorios(1, 10);
  writeLogMessageConVariable("1", "Pasamos a REGULACIONES, a ver si hay suerte", aleatorio);
  
  
  if(aleatorio==1 || aleatorio==2 ){    //20 %

    if(aleatorio==1){ //10% mal identificados

      writeLogMessage("1", "REGULACIONES El cliente está mal identificado, se va a escaleras o ascensor");
      sleep(calculaAleatorios(2, 6));
      writeLogMessageConVariable("1", "REGULACON CLIENTES numero aleatorio obtenido para el sleep de que se va a ESCALERAS O ASCENSOR", calculaAleatorios(2, 6));

      pthread_mutex_lock(&semaforoRecepcionistas);
      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      pthread_mutex_unlock(&semaforoRecepcionistas);
      //llamada para saber si va a ascensores o no
                  
    }else if(aleatorio==2){ //10% sin pasaporte covid abandonan el hotel  
      writeLogMessage("1", "REGULACIONES El cliente no tiene pasaporte covid, se marcha del hotel");
      
      int espera = 0;
      writeLogMessageConVariable("1", "REGULACON CLIENTES numero aleatorio obtenido para el sleep DE QUE SE MARCHA DEL HOTEL", espera);
      espera = sleep(calculaAleatorios(6, 10));

      pthread_mutex_lock(&semaforoRecepcionistas);
      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      pthread_mutex_unlock(&semaforoRecepcionistas);

      //SE VAN DEL HOTEL, PODRIAMOS DEJAR ESTO TAL CUAL
      //COGER EL ID DEL CLIENTE Y CAMBIAR A 0 TODOS SUS PARAMETROS

      writeLogMessage("1", "REGULACIONES cliente eliminado");
      eliminarCliente(identidad);
      //ELIMINAR DE LA COLA DE RECEPCIONISTAS

    }   
  }else{              //80% de los clientes: todo en orden
          
      writeLogMessage("1", "REGULACIONES El cliente lo tiene todo en orden, se va a escaleras o ascensor");
      sleep(calculaAleatorios(1, 4));

      writeLogMessage("1", "HACE EL SLEEP ZZZZZZZZZZ");

      pthread_mutex_lock(&semaforoCliente);

      writeLogMessage("1", "EJECUTA EL MUTEXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;

      writeLogMessage("1", "CAMBIA LAS FLAGS");
      //listaClientes[posicionCliente(identidad)].tipo = 1;
      pthread_mutex_unlock(&semaforoCliente);
      //llamada para saber si va a ascensores o no
      

      writeLogMessage("1", "FINAL DEL ELSE DE REGULACIONES");
    }

    writeLogMessage("1", "ULTIMO LOG DE REGULACION");

}

void eliminarCliente(int identidad){

      pthread_mutex_lock(&semaforoCliente);
      listaClientes[posicionCliente(identidad)].id=0;
      listaClientes[posicionCliente(identidad)].atendido=0;
      listaClientes[posicionCliente(identidad)].tipo=0;
      listaClientes[posicionCliente(identidad)].ascensor=0;

      contadorClientes--;
      
      pthread_mutex_unlock(&semaforoCliente);
      pthread_exit(&hiloClientes);
}