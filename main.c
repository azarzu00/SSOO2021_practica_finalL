
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
int ascensor = 0;                 //0=parado 1=funcionando
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
  int atendido;                                     //0 = NO ATENDIDO, 1 = ATENDIENDO, 2 = ATENDIDO
  int tipo;                                         //1 = normal, 2 = vip, 3 = fue a checkin y era normal,  4 = fue a checkin y era vip
  int ascensor;                                     //0 = false, 1 = true
} listaClientes[20];

int maquinasOcupadas[5] = {0};

FILE * logFile;

//Funciones
void *nuevoCliente(void *arg);
// void writeLogMessage(char *id, char *msg);
void writeLogMessageConVariable(char *msg, int i);
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
int hayHueco();
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

  
 
  //signal(SIGUSR1, &creacionClienteNormal);
  //signal(SIGUSR2, &creacionClienteVip);
  //signal(SIGINT, &finalPrograma);


  struct sigaction normal;
	sigemptyset(&normal.sa_mask);
	normal.sa_handler=creacionClienteNormal;
	normal.sa_flags = 0;
  
	if (sigaction(SIGUSR1, &normal, NULL) == -1) {
		perror("Error en el sigaction del cliente normal.");
		return 1;
	}



  struct sigaction vip;
	sigemptyset(&vip.sa_mask);
	vip.sa_handler=creacionClienteVip;
	vip.sa_flags = 0;
  
	if (sigaction(SIGUSR2, &vip, NULL) == -1) {
		perror("Error en el sigaction del cliente VIP.");
		return 1;
	}


  struct sigaction terminar;
	sigemptyset(&terminar.sa_mask);
	terminar.sa_handler=finalPrograma;
	terminar.sa_flags = 0;
  
	if (sigaction(SIGINT, &terminar, NULL) == -1) {
		perror("Error en el sigaction de la llamada finalizacion al programa.");
		return 1;
	}


  while(terminado == 0){
    //sleep(1);

    pause();
  }

  writeLogMessageConVariable("Esperando a que todos los clientes se vayan para finalizar el programa", 0);

  while (contadorClientes > 0) {

    //espera a que todos los clientes se vayan para finalizar el programa
    
  }

  writeLogMessageConVariable("Fin del programa", 0);

  //mover a funcion de cierre de programa
  pthread_mutex_destroy(&semaforoCliente); 
  pthread_mutex_destroy(&semaforoCheckIn);
  pthread_mutex_destroy(&semaforoRecepcionistas);
  pthread_mutex_destroy(&semaforoFichero);
  pthread_mutex_destroy(&semaforoAscensor); 
  pthread_cond_destroy(&condicion);
 
  
    return 0;
}





// void writeLogMessage(char *id, char *msg) {
//   pthread_mutex_lock(&semaforoFichero);
//   // Calculamos la hora actual
//   time_t now = time(0);
//   struct tm *tlocal = localtime(&now);
//   char stnow[25];
//   strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
//   // Escribimos en el log
//   logFile = fopen("logsPractica", "a");
//   fprintf(logFile, "[%s] %s: %s \n", stnow, id, msg);
//   fclose(logFile);
//   pthread_mutex_unlock(&semaforoFichero);
// }

void writeLogMessageConVariable( char *msg, int i) {
  pthread_mutex_lock(&semaforoFichero);
  // Calculamos la hora actual
  time_t now = time(0);
  struct tm *tlocal = localtime(&now);
  char stnow[25];
  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
  // Escribimos en el log
  logFile = fopen("logsPractica", "a");
  fprintf(logFile, "[%s] %d: %s \n", stnow, i, msg);
  fclose(logFile);
  pthread_mutex_unlock(&semaforoFichero);
}

//LOG DE ENTREGA

// void writeLogMessageConVariable(char *msg, int i) {
//  pthread_mutex_lock(&semaforoFichero);
//  //Calculamos la hora actual
//  time_t now = time(0);
//  struct tm *tlocal = localtime(&now);
//  char stnow[25];
//  strftime(stnow, 25, "%d/%m/%y %H:%M:%S", tlocal);
//  //Escribimos en el log
//  logFile = fopen("logsPractica", "a");
//  fprintf(logFile, "[%s] %d: %s \n", stnow, i, msg);
//  fclose(logFile);
//  pthread_mutex_unlock(&semaforoFichero);
// }


void finalPrograma(){
  
  terminado = 1;
  

}

void creacionClienteNormal(){
  
  int vip = 0;
  writeLogMessageConVariable(" ", 0);
  pthread_create(&hiloClientes, NULL, nuevoCliente , &vip);

}

void creacionClienteVip(){

  
  int vip=1;
  writeLogMessageConVariable(" ", 0);
  pthread_create(&hiloClientes, NULL, nuevoCliente , &vip);

  

}

void *nuevoCliente(void *arg){

  
  int encontrado = 0;
  int i = 0;

  pthread_mutex_lock(&semaforoCliente); 
  for (i = 0; i < 20 && encontrado==0; i++) {   //buscamos en la lista de clientes el primer espacio libre que haya

    if (listaClientes[i].id == 0) {
      encontrado = 1;

    }

  }
  pthread_mutex_unlock(&semaforoCliente); 

    
    pthread_mutex_lock(&semaforoCliente); 
    if(contadorClientes<20){
        
        if(*(int *)arg == 0){     //0 = CLIENTE NORMAL

          listaClientes[i].id = contadorIdClientes;

          

          writeLogMessageConVariable("Un cliente normal ha llegado al hotel", listaClientes[i].id);

     
          contadorIdClientes++;
          
          listaClientes[i].atendido = 0;              //se inicializan los flags del cliente vip
          listaClientes[i].tipo= 1;       
          listaClientes[i].ascensor = 0;
          contadorClientes++;
          
          int numeroIDCliente = listaClientes[i].id;
          pthread_mutex_unlock(&semaforoCliente);
          accionesCliente(numeroIDCliente);        
          

        }else{                    //1 = CLIENTE VIP
        
          listaClientes[i].id = contadorIdClientes;
          writeLogMessageConVariable("Un cliente vip ha llegado al hotel", listaClientes[i].id);
          printf("ID %d", contadorIdClientes);
          printf("\n");

          contadorIdClientes++;
            
          listaClientes[i].atendido = 0;                //se inicializan los flags del cliente vip
          listaClientes[i].tipo= 2;
          listaClientes[i].ascensor = 0;
          contadorClientes++;
          

          int numeroID=listaClientes[i].id;
          pthread_mutex_unlock(&semaforoCliente);
          accionesCliente(numeroID);   
          
        }
          
    }else{        //no hay espacio en el hotel, salimos del hilo
        
        
        writeLogMessageConVariable("No pueden acceder mas clientes al hotel porque está lleno", 0);
        pthread_mutex_unlock(&semaforoCliente);
        //pthread_exit(NULL);   CREO QUE HAY QUE PONERLO
        
    } 
    

    // pthread_exit(NULL); //lo haremos cuando los clientes se vayan a la escalera o cuando hayan entrado en el ascensor
}


void accionesCliente(int identidad) {
  
 
  
  int eleccion = calculaAleatorios(1, 10);
  

  if (eleccion == 1) {                  //El cliente va directamente a maquinas de checkin    

    writeLogMessageConVariable("El cliente se va a maquinas de checkin", identidad);                                
             
                           
    accionesCheckin(identidad);   
    
                               
  } else {
    
    eleccion = calculaAleatorios(1, 10);
    
    
    
    if(eleccion==2 || eleccion == 3){               //El cliente se cansan de esperar en cola y se van a maquinas de checkIn

      writeLogMessageConVariable("El cliente se cansa de esperar en la cola y se va a maquinas de checkIn", identidad); 
     
      accionesCheckin(identidad);                   
    

    } else if (eleccion==4){                        //El cliente se cansa de esperar en cola y se marcha del hotel

    writeLogMessageConVariable("El cliente se cansa de esperar en la cola y se marcha del hotel", identidad); 
   
    eliminarCliente(identidad);


    } else {    

        
        eleccion = calculaAleatorios( 1, 20);
            
        if(eleccion == 1) {                      //El cliente se cansa de la cola, va al baño y se marcha del hotel

          writeLogMessageConVariable("El cliente se cansa de estar en la cola, se va al baño y marcha del hotel", identidad);
   
          eliminarCliente(identidad);
          
        } else {                                //El cliente se queda definitivamente y va con los recepcionistas

          writeLogMessageConVariable("El cliente se queda en la cola de los recepcionistas", identidad);

          pthread_mutex_lock(&semaforoRecepcionistas);

          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++;

          pthread_mutex_unlock(&semaforoRecepcionistas);

    
         
        }
    } 
  }


  pthread_mutex_lock(&semaforoCliente);
  while (listaClientes[posicionCliente(identidad)].atendido == 0) {               //Espera hasta que se este antendiendo


    pthread_mutex_unlock(&semaforoCliente);  
    pthread_mutex_lock(&semaforoCliente);  
  }
  pthread_mutex_unlock(&semaforoCliente);  



  pthread_mutex_lock(&semaforoCliente);
  if (listaClientes[posicionCliente(identidad)].tipo == 3 || listaClientes[posicionCliente(identidad)].tipo == 4) {   //si entra en el if, es que vuelve de maquinas
    
  }
  pthread_mutex_unlock(&semaforoCliente);
  
  pthread_mutex_lock(&semaforoCliente);
  if(listaClientes[posicionCliente(identidad)].tipo == 1 || listaClientes[posicionCliente(identidad)].tipo == 2) {
    
    pthread_mutex_unlock(&semaforoCliente);
    regulacionClientes(identidad);
  }
  pthread_mutex_unlock(&semaforoCliente);
 
  
  ascensorEscalera(identidad);
 


  
}


void accionesCheckin(int identidad) {

    srand(time(NULL));
    int parar = 0;
    int irse = 0;
    int aleatorio;

    

    pthread_mutex_lock(&semaforoCliente);
    
    if (listaClientes[posicionCliente(identidad)].tipo == 1) {          //cambiamos el tipo de cliente a 3 si antes era normal

        listaClientes[posicionCliente(identidad)].tipo = 3;  
    } else if (listaClientes[posicionCliente(identidad)].tipo == 2) {   //cambiamos el tipo de cliente a 4 si antes era vip

        listaClientes[posicionCliente(identidad)].tipo = 4;  
    }



    pthread_mutex_unlock(&semaforoCliente);
       
     while (parar == 0) {

        pthread_mutex_lock(&semaforoCheckIn);



       if (hayHueco() == 1) {                                           //Hay maquinas libres

         writeLogMessageConVariable("El cliente ha accedido a una maquina de checkin", identidad);

         for (int i = 0; i < 5 && parar == 0; i++) {

           if (maquinasOcupadas[i] == 0) {                              //Se busca la maquina libre

              writeLogMessageConVariable("Máquina de checkin utilizada", i + 1);

              maquinasOcupadas[i] = 1;
              parar = 1;

              pthread_mutex_unlock(&semaforoCheckIn);

              sleep(6);                                                 //Espera 6 segundos al ocupar la maquina de checkin

              pthread_mutex_lock(&semaforoCheckIn);
              
              maquinasOcupadas[i] = 0;                                  //Termina de usar la máquina y se va a los ascensores o las escaleras
              writeLogMessageConVariable("El cliente ha terminado de utilizar la maquina de checkin", identidad);
              pthread_mutex_lock(&semaforoCliente);                                   
              listaClientes[posicionCliente(identidad)].atendido = 1;
              pthread_mutex_unlock(&semaforoCliente); 
              
              pthread_mutex_unlock(&semaforoCheckIn);                              


           }
         }
       
       } else {                                 //Si no hay huecos hay un 50% de que decida irse a recepcionistas

         pthread_mutex_unlock(&semaforoCheckIn);
         aleatorio = calculaAleatorios(1, 10);
 
        
          if (aleatorio < 6) {                                           //El cliente se va con los recepcionistas poprque no hay hueco en las maquinas

          writeLogMessageConVariable("No hay maquinas de checkin, el cliente decide ir a los recepcionistas", identidad);
          irse = 1;
          parar = 1;

          pthread_mutex_lock(&semaforoCliente);  
                                                                          //Se cambia el tipo de cliente dependiedno si antes era VIP o normal

          if (listaClientes[posicionCliente(identidad)].tipo == 3) {       //Antes era normal        

            listaClientes[posicionCliente(identidad)].tipo = 1;  
            
          } else if (listaClientes[posicionCliente(identidad)].tipo == 4) {   //Antes era VIP

            listaClientes[posicionCliente(identidad)].tipo = 2; 
          }
          pthread_mutex_unlock(&semaforoCliente);  

          
          pthread_mutex_lock(&semaforoRecepcionistas);
          ordenRecepcionista[numeroCola] = identidad;                         //Se introduce el cliente en la cola de los recepcionistas
          numeroCola++;
          writeLogMessageConVariable("El cliente se pone en la cola de los recepcionistas", identidad);
          pthread_mutex_unlock(&semaforoRecepcionistas);

           

          
        } else {                                                              //No hay hueco en las maquinas de checkin pero el cliente vuelve a intentar ir a una maquina

          writeLogMessageConVariable("No hay hueco en las maquinas, el cliente vuelve a intentar coger una maquina", identidad);
          pthread_mutex_unlock(&semaforoCheckIn);

          sleep(3);
          
          accionesCheckin(identidad);                                         //Se vuelve a llamar a la funcion tras 3 segundos para que el cliente vuelva a intentar utilizar una maquina 
                              
        }
       }
     }

  
}

int hayHueco() {        //comprueba si hay maquinas de checkin libres

  int hueco = 0;

  for (int i = 0; i < 5; i++) {

    if (maquinasOcupadas[i] == 0) {

      hueco = 1;
    }
  }

  return hueco;
}

//RECEPCIONISTAS 1 MAQUINAS 2

//RECEPCIONISTAS
void *accionesRecepcionista1(void *arg){      
    
  int pos;
  
  
  while(true){

   
   pthread_mutex_lock(&semaforoRecepcionistas);
    

    if(numeroCola>0){

      if(descanso1==5){                                           //El recepcionista descansa cuando atiende a 5 clientes
        writeLogMessageConVariable("El recepcionista ha atendido a 5 clientes y ahora descansa durante 5 segundos", 1);

        pthread_mutex_unlock(&semaforoRecepcionistas);
        sleep(5);
        descanso1=0;
        pthread_mutex_lock(&semaforoRecepcionistas);
      }
      
        
        int i = 0;
        int encontrado = 0;
        
        pthread_mutex_lock(&semaforoCliente);
        while(i<20 && encontrado==0){                                //Selecciamos el primer cliente de la cola si es que lo hay
          
          if (ordenRecepcionista[i] != 0 && listaClientes[posicionCliente(ordenRecepcionista[i])].tipo == 1) {

            encontrado = 1;
            
          }

          i++;
        }  

        pthread_mutex_unlock(&semaforoCliente); 
        i--;

        

        if(encontrado==1 ){

          writeLogMessageConVariable("El recepcionista uno ha escogido un cliente", 1);

   
          

          int copiaID = ordenRecepcionista[i];


          ordenRecepcionista[i]=0;                                  //Se libera un hueco de la cola 


          pthread_mutex_unlock(&semaforoRecepcionistas);
          quitarVacioCola(1);                                       //Se mueven todos los elemenatos de la cola de los clientes a la izquierda para eliminar el hueco libre del medio     
          pthread_mutex_lock(&semaforoRecepcionistas);

          pthread_mutex_lock(&semaforoCliente);
          listaClientes[posicionCliente(copiaID)].atendido = 1;      //Se cambia atendio a verdadero
          pthread_mutex_unlock(&semaforoCliente);

          pthread_mutex_lock(&semaforoCliente);
          while(listaClientes[posicionCliente(copiaID)].atendido != 2 && posicionCliente(copiaID) != -1) {     //Espera a que el cliente haya sido atendido completamente antes de pasar con otro
            
            pthread_mutex_unlock(&semaforoCliente);
            pthread_mutex_lock(&semaforoCliente);
           
          }
          pthread_mutex_unlock(&semaforoCliente);       

         writeLogMessageConVariable("El cliente ha sido atendido por el recepcionista uno", copiaID);


          descanso1++;

          sleep(1);
          pthread_mutex_unlock(&semaforoRecepcionistas);
           
        }else{
                                                                      //No se han encontrado clientes normales en la cola
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
        }
        }else{                                                        //No hay clientes en la cola
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);  

      }
    }
}

void *accionesRecepcionista2(void *arg){


  int pos;
  int i;
  int encontrado;
   


  
  while(true){

   
   pthread_mutex_lock(&semaforoRecepcionistas);

    if(numeroCola>0){

      if(descanso2==5){                                               //El recepcionista descansa cuando atiende a 5 clientes

        writeLogMessageConVariable("El recepcionista ha atendido a 5 clientes y ahora descansa durante 5 segundos", 2);

          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(5);
          descanso2=0;
          pthread_mutex_lock(&semaforoRecepcionistas);
      }
      
       //decidir que cliente escogemos 
        int i = 0;
        int encontrado = 0;
        
        
        pthread_mutex_lock(&semaforoCliente);
        while(i<20 && encontrado==0){                                   //Selecciamos el primer cliente de la cola si es que lo hay
          
          if (ordenRecepcionista[i] != 0 && listaClientes[posicionCliente(ordenRecepcionista[i])].tipo == 1) {

            encontrado = 1;
            
          }

          i++;
        }   
        pthread_mutex_unlock(&semaforoCliente); 

        i--;


        if(encontrado==1 ){

          writeLogMessageConVariable("El recepcionista dos ha escogido un cliente", 1);


         

          int copiaID = ordenRecepcionista[i];

     

          ordenRecepcionista[i]=0;                                        //Se libera un hueco en la cola

        

          pthread_mutex_unlock(&semaforoRecepcionistas);
          quitarVacioCola(1);                                             //Se mueven todos los elemenatos de la cola de los clientes a la izquierda para eliminar el hueco libre del medio
          pthread_mutex_lock(&semaforoRecepcionistas);
          
    
          
          pthread_mutex_lock(&semaforoCliente);
          listaClientes[posicionCliente(copiaID)].atendido = 1;            //Se cambia atendio a verdadero
          pthread_mutex_unlock(&semaforoCliente);

          

          pthread_mutex_lock(&semaforoCliente);
          while(listaClientes[posicionCliente(copiaID)].atendido != 2 && posicionCliente(copiaID) != -1) { //Espera a que el cliente haya sido atendido completamente antes de pasar con otro
            //jiji
            pthread_mutex_unlock(&semaforoCliente);
            pthread_mutex_lock(&semaforoCliente);
           
          }
          pthread_mutex_unlock(&semaforoCliente);     

         writeLogMessageConVariable("El cliente ha sido atendido por el recepcionista dos", copiaID);

          descanso2++;

          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
                     
        }else{                                                                //No se han encontrado clientes normales en la cola
          
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
        }
        }else{                                                                //No hay clientes en la cola
          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);  
      }
    }

}



void *accionesRecepcionistaVip(void *arg){

  int pos;
  int encontrado;
  int i = 0;
  int copiaID;
  

  
  while(true){ 
    
    pthread_mutex_lock(&semaforoRecepcionistas);
    
    if(numeroCola>0){                                                                         //Si hay algun cliente en la cola se compueba si hay alguno VIP

      encontrado=0;
      
     
      pthread_mutex_lock(&semaforoCliente);
      while(i<20 && encontrado==0){                                                           //Busca un cliente VIP en la cola
          
          if (ordenRecepcionista[i] != 0 && listaClientes[posicionCliente(ordenRecepcionista[i])].tipo == 2) {

            encontrado = 1;
            
          }

          i++;
      }

      pthread_mutex_unlock(&semaforoCliente); 

        i--;

      if(encontrado == 1){ 

        writeLogMessageConVariable("El recepcionista VIP ha encontrado a un cliente de su tipo", 3);

        copiaID = ordenRecepcionista[i];

        ordenRecepcionista[i]=0;

        pthread_mutex_unlock(&semaforoRecepcionistas);
        quitarVacioCola(1);                                             //Si se encuentra se quita de la cola y se mueven los demas clientes
        pthread_mutex_lock(&semaforoRecepcionistas);

        pthread_mutex_lock(&semaforoCliente);
        listaClientes[posicionCliente(copiaID)].atendido = 1;           //Se cambia el tipo atendido a verdadero
        pthread_mutex_unlock(&semaforoCliente);

       
        pthread_mutex_lock(&semaforoCliente);
        while(listaClientes[posicionCliente(copiaID)].atendido != 2) {  //Espera a que el cliente haya sido atendido completamente antes de pasar con otro
          
          pthread_mutex_unlock(&semaforoCliente);
          pthread_mutex_lock(&semaforoCliente);
        }
        pthread_mutex_unlock(&semaforoCliente);

        pthread_mutex_unlock(&semaforoRecepcionistas);

        writeLogMessageConVariable("El cliente vip ha sido atendido por el recepcionista", copiaID);
        
      } else {
        
                                                                        //No se encuentra ningun cliente vip
        pthread_mutex_unlock(&semaforoRecepcionistas);
        sleep(1);
      }
    }else{                                                              //No hay clientes en la cola                       
      
      pthread_mutex_unlock(&semaforoRecepcionistas);
      sleep(1);
    }
  }

  
}


void ascensorEscalera(int identidad) {


  srand(time(NULL));

  int aleatorio;
  int encontrado = 0;
  int copiaClientes;

  aleatorio = calculaAleatorios(1, 10);

  
  //toDo probabilidades- cambiado para la ejecucion
  if (aleatorio==1 && aleatorio==2) {                                                  //Se va por escaleras

    writeLogMessageConVariable("El cliente se ha ido por las escaleras", identidad);
    eliminarCliente(identidad);

  } else {                                                                              //Se va por ascensores
      
      writeLogMessageConVariable("El cliente va a ascensores", identidad);
      pthread_mutex_lock(&semaforoAscensor);
      clientesEsperando++;
      pthread_mutex_unlock(&semaforoAscensor);


    while(true){ 
      pthread_mutex_lock(&semaforoAscensor);

      if(ascensor==0){
              
          if (clientesEsperando < 6) {                                                  //Si el ascensor no esta lleno, el cliente accede a ascensores

            
            writeLogMessageConVariable("El cliente entra en el ascensor", identidad);
            pthread_cond_wait(&condicion, &semaforoAscensor);

            pthread_mutex_unlock(&semaforoAscensor);  
            
            writeLogMessageConVariable("El cliente sale del ascensor", identidad);
            if (clientesEsperando > 0) {                                            //Mientras queden clientes en el ascensor, siguen mandando señales para que salgan
              
              pthread_mutex_lock(&semaforoAscensor);
              clientesEsperando--;
              pthread_cond_signal(&condicion);
              pthread_mutex_unlock(&semaforoAscensor);   
            
            }

              
            
          }else{      

            writeLogMessageConVariable("El cliente entra en el ascensor y cierra la puerta", identidad);      //Llega el utlimo cliente en entrar
            ascensor=1;
            pthread_mutex_unlock(&semaforoAscensor);
         
            writeLogMessageConVariable("El ascensor comienza a funcionar", 0);
            sleep(calculaAleatorios(3, 6));
            writeLogMessageConVariable("El cliente sale del ascensor", identidad);
            pthread_mutex_lock(&semaforoAscensor); 
            clientesEsperando--;
            pthread_cond_signal(&condicion);
            pthread_mutex_unlock(&semaforoAscensor);
               
          }

          pthread_mutex_lock(&semaforoAscensor);
          copiaClientes = clientesEsperando;
          pthread_mutex_unlock(&semaforoAscensor);

          if (copiaClientes == 0) {
            writeLogMessageConVariable("El ascensor ha termiando. todos los clientes han salido", 0); 
            ascensor = 0;
          }
          
          eliminarCliente(identidad);                                                           //Eliminamos el cliente


    }else{
        pthread_mutex_unlock(&semaforoAscensor); 
        sleep(3);
    }
    }
  }
}


int posicionCliente(int identidad) {              //Obtiene la posicon del cliente en el array ListaClientes pasandole la identidad

  int numero = 0;
  int encontrado = 0;

  for(int i=0; i<20 && encontrado==0; i++) {
    
   if(listaClientes[i].id==identidad){
     numero = i;
     encontrado = 1;
   }
  }

  if (encontrado == 1) {
    
      return numero;
  } else {

    return -1;
  }

}



int calculaAleatorios(int min, int max) {
  return rand() % (max-min+1) + min;
}

void quitarVacioCola(int cola) {                 
  
  int encontrado = 0;

  if (cola == 1){
    pthread_mutex_lock(&semaforoRecepcionistas);
    for (int i = 0; i < 19; i++) {
      if (ordenRecepcionista[i] == 0) {

        encontrado = 1;  
      }
      if (encontrado == 1) {
       
        ordenRecepcionista[i] = ordenRecepcionista[i+1];
      }
    }
    numeroCola--;
    ordenRecepcionista[19] = 0;
    pthread_mutex_unlock(&semaforoRecepcionistas);

  }else{    
    pthread_mutex_lock(&semaforoCheckIn);
    for (int i = 0; i < 19; i++) {
      if (ordenCheckin[i] == 0) {

        encontrado = 1; 
      }
      if (encontrado == 1) {
        ordenCheckin[i] = ordenCheckin[i+1];
      }
    }

    numeroCheckin--;
    ordenCheckin[19] = 0;

    pthread_mutex_unlock(&semaforoCheckIn);
  }
}


void regulacionClientes(int identidad) {   //decidimos lo que pasa a los clientes antes de irse
  
  srand(time(NULL));
  
  int aleatorio = calculaAleatorios(1, 10);
  
  if(aleatorio==1 || aleatorio==2 ){    //20 %

    if(aleatorio==1){                                                         // El 10% de los clientes estan mal identificados

      writeLogMessageConVariable("El cliente esta mal identificado, tardará más tiempo en ser atendido", identidad);
 

      sleep(calculaAleatorios(2, 6));

      pthread_mutex_lock(&semaforoCliente);
      listaClientes[posicionCliente(identidad)].atendido = 2;                //Se cambia el flag del cliente a atendido
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      pthread_mutex_unlock(&semaforoCliente);
      
                  
    }else if(aleatorio==2){                                                  //10% sin pasaporte covid abandonan el hotel  

      writeLogMessageConVariable("El cliente no dispone del pasaporte covid, se va del hotel", identidad);
      sleep(calculaAleatorios(6, 10));
      
      pthread_mutex_lock(&semaforoCliente);
      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;
      pthread_mutex_unlock(&semaforoCliente);

      //ELIMINAR DE LA COLA DE RECEPCIONISTAS

    }   
  }else{              //80% de los clientes: todo en orden

      writeLogMessageConVariable("El cliente tiene toda la documentación en orden", identidad);
      sleep(calculaAleatorios(1, 4));
      

      pthread_mutex_lock(&semaforoCliente);

      listaClientes[posicionCliente(identidad)].atendido = 2;
      listaClientes[posicionCliente(identidad)].ascensor = 1;

      pthread_mutex_unlock(&semaforoCliente);
  
    }

}

void eliminarCliente(int identidad){                          //Elimina el cliente de la lista de clientes

      pthread_mutex_lock(&semaforoCliente);

      listaClientes[posicionCliente(identidad)].id=0;
      listaClientes[posicionCliente(identidad)].atendido=0;
      listaClientes[posicionCliente(identidad)].tipo=0;
      listaClientes[posicionCliente(identidad)].ascensor=0;

      contadorClientes--;
      
      pthread_mutex_unlock(&semaforoCliente);
      
      pthread_exit(&hiloClientes);
}