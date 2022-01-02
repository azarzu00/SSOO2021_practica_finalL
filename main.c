/*   COMANDOS PARA MANDAR LAS SEÑALES AL PROGRAMA CUANDO

      a.out

      ps -ef

      kill -s SIGUSR1 pid
      kill -s SIGUSR2 pid
      kill -s SITERM pid   
*/

//    TIPO:    1 NORMAL       2 VIP         3 MAQUINA

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
bool recepcionistaVipFuncionando=false;
int logs=0;
//semaforos
pthread_mutex_t semaforoCliente, semaforoFichero, maquinas, semaforoRecepcionistas;
pthread_t hiloClientes;
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
  bool atendido; 
  int tipo;
  bool ascensor;
} listaClientes[20];

bool maquinasOcupadas[5] = {false, false, false, false, false};

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
bool hayHueco();
void accionesCheckin(int identidad);
void quitarVacioCola(int cola);
void regulacionClientes();
void eliminarCliente(int identidad);
void ascensorEscalera(int identidad);





int main(int argc, char *argv[]){
    
  system("rm logsPractica");
  system("touch logsPractica");

  
  //HILOS
  pthread_t recepcionista1,recepcionista2,recepcionistaVip;
  pthread_attr_t tattr;
    
  pthread_mutex_init(&semaforoCliente, NULL); 

  pthread_create(&recepcionista1, NULL, accionesRecepcionista1, "Ejecuta recepcionista1?");
  pthread_create(&recepcionista2, NULL, accionesRecepcionista2, "Ejecuta recepcionista2?");
  pthread_create(&recepcionistaVip, NULL, accionesRecepcionistaVip, "Ejecuta recepcionistavip?");
  
 
  signal(SIGUSR1, &creacionClienteNormal );
  signal(SIGUSR2, &creacionClienteVip );
  signal(SIGTERM, &finalPrograma);
  
  while(!terminado){
    pause();
  }

  pthread_mutex_destroy(&semaforoCliente); 
  writeLogMessage("1", "Fin del programa.");

  

  //recepcionistas 

    for (int i = 0; i<20; i++) {

      printf("%d", ordenRecepcionista[i]);

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

          printf("ESTAMOS EN CREACION DE UN NUEVO CLIENTE NORMAL  %d", contadorIdClientes);
          writeLogMessage("1", "hola todo bien estoy en creacion del cliente ok");
          printf("\n");
          
          contadorIdClientes++;
          
          listaClientes[i].atendido = false;
          listaClientes[i].tipo= 1;       
          listaClientes[i].ascensor = false;
          contadorClientes++;
          writeLogMessage("1", "Un cliente NORMAL ha llegado.");

          pthread_mutex_unlock(&semaforoCliente);

          accionesCliente(listaClientes[i].id);

        }else{                    //1 = CLIENTE VIP

          listaClientes[i].id = contadorIdClientes;

          printf("ID jaja miguel  %d", contadorIdClientes);
          printf("\n");

          contadorIdClientes++;
            
          listaClientes[i].atendido = false;
          listaClientes[i].tipo= 2;
          listaClientes[i].ascensor = false;
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
  

  if (eleccion == 1) {                  //El cliente va directamente a maquinas de checking    
                                        //metodo para ir a maquinas de checkin
    writeLogMessage("1", "me voy a chec");                                    //Si llegan y no quedan maquinas libres se van a recepcionistas
                                        //Habria que añadirlo a la cola de clientes (ordenEntradaClientes)
  } else {
    
    eleccion = calculaAleatorios(1, 10);
    
    if(eleccion==2 || eleccion == 3){   //El cliente se cansan de esperar en cola y se van a maquinas de checkIn

                                        //metodo para ir a maquinas de checkin
                                        //En las maquinas checking el cliente compreba si hay maquinas libres, si no las hay espera 3s en los que hay un 50% de irse 
    

    } else if (eleccion==4){            //El cliente se cansa de esperar en cola y se marcha del hotel

    
      writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE CANSA DE ESPERAR");
      eliminarCliente(identidad);


    } else {    

        eleccion = calculaAleatorios( 1, 20);

        if(eleccion == 1) {             //El cliente se cansa de la cola, van al baño y se marcha del hotel

          writeLogMessage("1", "CLIENTE ELIMINADO PORQUE SE VA A LAVARSE LAS MANOS Y PIERDE SU TURNO");
          eliminarCliente(identidad);
          
        } else {                        //El cliente se queda definitivamente y va con los recepcionistas

          writeLogMessage("1", "EL CLIENTE SE VA A LA COLA A ESPERAR QUE LOS RECEPCIONISTAS LE LLAMEN");       //( ͡👁️ ͜ʖ ͡👁️)
          ordenRecepcionista[numeroCola] = identidad;
          numeroCola++; 
         
           writeLogMessage("1", "NUMEROCOLA SE HA AUMENTADO");
        }
    } 
  }

  

}

void accionesCheckin(int identidad) {

    bool parar = false;
    bool irse = false;
    int aleatorio;
       
     while (!parar) {

       if (hayHueco()) {

         for (int i = 0; i < 5; i++) {

           if (maquinasOcupadas[i] == false) {

              maquinasOcupadas[i] = true;
              parar = true;

              sleep(6);                             //Espera 6 segundos al ocupar la maquina de checkin

              maquinasOcupadas[i] = false;          //Termina de usar la máquina y debería irse al ascensor
                                                    //TO DO ascensor o escalera
           }
         }

       } else {             //Si no hay huecos hay un 50% de que decida irse a recepcionistas

        aleatorio = calculaAleatorios(1, 10);

        if (aleatorio < 6) {

          irse = true;
          parar = true;
        } else {

          sleep(3);
                              //EL CLIENTE VA AL METODO QUE VUELVE A LLAMAR A ESTE MISMO METODO PARA QUE REPITA EL PROCESO
        }

       }
     }

  if (!irse){


  } else {                  // El cliente se va de maquinas y se añade a la cola de espera de recepcionistas

    ordenRecepcionista[numeroCola] = identidad;
    numeroCola++; 

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

//RECEPCIONISTAS
void *accionesRecepcionista1(void *arg){      //utilizamos el mutex solamente para cuando estamos escogiendo el cliente en cada recepcionista

  //escoge sus propio clientes utilizando el mutex para no acceder a la vez al array de clientes
  // IMPORTATE: SI EL SIGUIENTE CLIENTE ES VIP, LLAMAREMOS A UN METODO QUE COMPRUEBA SI EL RECEPCIONISTA VIP ESTA OCUPADO, SI ESTA OCUPADO, EL RECEPCIONISTA NORMAL SE ENCARRGARIA DEL VIP
  
  bool tenemosCliente;
  int pos;
  
   

  writeLogMessage("1", "inicio recepcionista 1");
  
  while(true){

    //1 NORMAL, 2 VIP, 3 MAQUINAS
    //RECEPCIONISTAS 1 MAQUINAS 2

   // pthread_mutex_lock(&semaforoRecepcionistas); 
    writeLogMessage("1", "inicio rece");

    if(numeroCola>0){
      
      writeLogMessage("1", "ENTRA EN IF ");
       //decidir que cliente escogemos
      

      int i;
      bool encontrado = false;
      for(i=0; i<20 && !encontrado; i++){
        pos=posicionCliente(ordenRecepcionista[0]);
        if(listaClientes[i].tipo==1){
          encontrado=true;
        }
      }

      

      ordenRecepcionista[i]=0;
      quitarVacioCola(1);
      //mutex
      regulacionClientes();
        
      }else{
        sleep(1);
      }
      
      ///////////////////////////////////
   

        if(listaClientes[pos].tipo==2 && !recepcionistaVipFuncionando ){    //si el siguiente cliente en la cola es VIP tendremos que controlar si el recepcionistaVip est ocupado  
          
          pos=posicionCliente(ordenRecepcionista[1]); //Si no podemos coger el primero cogeremos el segundo cliente sea vip o no
          ordenRecepcionista[1]=0;
          quitarVacioCola(1);

          //mutex si es vip
          pthread_mutex_unlock(&semaforoRecepcionistas);
          //IMPORTANTE, HACER METODO PARA MOVER LOS ELEMENTOS DE LA COLA A LA IZQUIERDA Y ELIMINAR EL PRIMERO
          
          
        }

      }else{    //cliente normal

        if ((!(listaClientes[posicionCliente(ordenRecepcionista[0])].tipo == 2) || (ordenRecepcionista[1]!=0 && listaClientes[posicionCliente(ordenRecepcionista[0])].tipo==2 && recepcionistaVipFuncionando)) {

          printf("AQUI ESTAMOS CON EL CLIENTE %d",ordenRecepcionista[0] );
          writeLogMessage("1", "AQUI ESTAMOS CON EL CLIENTE NORMAL");
          ordenRecepcionista[0]=0;      //MIRAR POR QUE NO QUITA CORRECTAMENTE AL CLIENTE
          quitarVacioCola(1);

          //mutex si es normal
          pthread_mutex_unlock(&semaforoRecepcionistas);


          regulacionClientes();

        } else {

            pthread_mutex_unlock(&semaforoRecepcionistas);
            sleep(1);
          }

          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);

      }
        
    } else {


      writeLogMessage("1", "HACEMOS UN SLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP        1");
      //pthread_mutex_unlock(&semaforoRecepcionistas);
      sleep(1);

    }

      
    }
  
  // // // // // // if(tenemosCliente==true){
  // // // // // //   listaClientes[pos].atendido=true;
  // // // // // // }


  //pthread_exit(NULL);
}

void *accionesRecepcionista2(void *arg){

  //escoge sus propio clientes utilizando el mutex para no acceder a la vez al array de clientes
  // IMPORTATE: SI EL SIGUIENTE CLIENTE ES VIP, LLAMAREMOS A UN METODO QUE COMPRUEBA SI EL RECEPCIONISTA VIP ESTA OCUPADO, SI ESTA OCUPADO, EL RECEPCIONISTA NORMAL SE ENCARRGARIA DEL VIP
  
  bool tenemosCliente;
  int pos;
  
   

  writeLogMessage("1", "inicio recepcionista 2");
  
  while(true){


    //pthread_mutex_lock(&semaforoRecepcionistas); 
    writeLogMessage("1", "inicio rece");

    if(numeroCola>0){
      
      writeLogMessage("1", "ENTRA EN IF ");
       //decidir que cliente escogemos
      pos=posicionCliente(ordenRecepcionista[0]);


  if(ordenRecepcionista[1]!=0 && listaClientes[posicionCliente(ordenRecepcionista[0])].tipo==2  && !recepcionistaVipFuncionando){

        if(listaClientes[pos].tipo==2 && !recepcionistaVipFuncionando ){    //si el siguiente cliente en la cola es VIP tendremos que controlar si el recepcionistaVip est ocupado  
          
          pos=posicionCliente(ordenRecepcionista[1]); //Si no podemos coger el primero cogeremos el segundo cliente sea vip o no
          ordenRecepcionista[1]=0;
          quitarVacioCola(1);

          //mutex si es vip
          pthread_mutex_unlock(&semaforoRecepcionistas);
          //IMPORTANTE, HACER METODO PARA MOVER LOS ELEMENTOS DE LA COLA A LA IZQUIERDA Y ELIMINAR EL PRIMERO
          
          regulacionClientes();
        }

      }else{    //cliente normal

        if ((!(listaClientes[posicionCliente(ordenRecepcionista[0])].tipo==2) || (ordenRecepcionista[1]!=0 && listaClientes[posicionCliente(ordenRecepcionista[0])].tipo==2 && recepcionistaVipFuncionando)) {

          printf("AQUI ESTAMOS CON EL CLIENTE %d",ordenRecepcionista[0] );
          writeLogMessage("1", "AQUI ESTAMOS CON EL CLIENTE");
          ordenRecepcionista[0]=0;      //MIRAR POR QUE NO QUITA CORRECTAMENTE AL CLIENTE
          quitarVacioCola(1);

          //mutex si es normal
          pthread_mutex_unlock(&semaforoRecepcionistas);


          regulacionClientes();

        } else {

            pthread_mutex_unlock(&semaforoRecepcionistas);
            sleep(1);
          }

          pthread_mutex_unlock(&semaforoRecepcionistas);
          sleep(1);
      }
        
    } else {
      writeLogMessage("1", "HACEMOS UN SLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP         2 ");
      pthread_mutex_unlock(&semaforoRecepcionistas);
      sleep(1);

    }

      
    }
  
}

void *accionesRecepcionistaVip(void *arg){

  //recordar cambiar la variable para saber si esta funcionando o no 
  //escoge sus propio clientes utilizando el mutex para no acceder a la vez al array de clientes
  // while()
  bool tenemosCliente;
  int pos;
  
 

  writeLogMessage("1", "inicio recepcionista VIP");
  
  while(true){

   // pthread_mutex_lock(&semaforoRecepcionistas); 

    writeLogMessage("1", "inicio rece BIP");

    if(numeroCola>0){
      
      writeLogMessage("1", "ENTRA EN IF ");
       //decidir que cliente escogemos

      bool encontrado=false;

      int i;
      for(i=0; i<numeroCola; i++){

        pos=posicionCliente(ordenRecepcionista[i]);
        if(listaClientes[pos].tipo==2){
          encontrado=true;
        }

      }


      if(encontrado ){    //si el siguiente cliente en la cola es VIP tendremos que controlar si el recepcionistaVip est ocupado  
        

        ordenRecepcionista[i]=0;
        quitarVacioCola(1);

        pthread_mutex_unlock(&semaforoRecepcionistas);
       
        
        regulacionClientes();

      } else {
        
        pthread_mutex_unlock(&semaforoRecepcionistas);
        sleep(1);
      }

    }else{
      writeLogMessage("1", "HACEMOS UN SLEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP   VIP");
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


  } else {    //lista de los checkin
    
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
  
  int aleatorio = calculaAleatorios(1, 10);
  writeLogMessage("1", "PASAMOS A REGULACIONES, a ver si hay suerte");
  
  
  if(aleatorio==1 || aleatorio==2 ){    //20 %

     aleatorio = calculaAleatorios(1, 10);

    if(aleatorio==1){ //10% mal identificados

      writeLogMessage("1", "EL CLIENTE ESTA MAL IDENTIFICADO 10%, EN UN TIEMPO SE VA AL ASCENSOR AL ASCS ESCALERAS");
      sleep(calculaAleatorios(2, 6));
      ascensorEscalera(identidad);
      //llamada para saber si va a ascensores o no
          
            
    }else if(aleatorio==2){ //10% sin pasaporte covid abandonan el hotel  
      writeLogMessage("1", "EL CLIENTE NO TIENE EL PASAPORTE COVID, SE MARCHA DEL HOTEL");
      
      sleep(calculaAleatorios(6, 10)); 
                                  //SE VAN DEL HOTEL, PODRIAMOS DEJAR ESTO TAL CUAL
      //COGER EL ID DEL CLIENTE Y CAMBIAR A 0 TODOS SUS PARAMETROS

      eliminarCliente(identidad);

      //cliente eliminado

    }   
  }else{              //80% de los clientes: todo en orden
          
      writeLogMessage("1", "EL CLIENTE TIENE TODO EN ORDEN, SE VA AL ASCENSOR O A LAS ESCALERAS");
      sleep(calculaAleatorios(1, 4));
      ascensorEscalera(identidad);
      //llamada para saber si va a ascensores o no

    }

}



void eliminarCliente(int identidad){

      listaClientes[posicionCliente(identidad)].id=0;
      listaClientes[posicionCliente(identidad)].atendido=false;
      listaClientes[posicionCliente(identidad)].tipo=0;
      listaClientes[posicionCliente(identidad)].ascensor=false;
      
}