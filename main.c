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
int contadorClientes=0;
int numClientesAscensor=0;
bool ascensor;
//semaforos
pthread_mutex_t semaforo, semaforoFichero, maquinas;

int recepcionistas[3];
int maquinasCheckIn[5];
FILE * logFile;

void writeLogMessage(char *id, char *msg);

struct Cliente{
  int  id;
  bool atendido; 
  char tipo;
  bool ascensor;
} listaClientes[20];


int main(int argc, char *argv[]){

  system("rm logsPractica");
  system("touch logsPractica");   //PREGUNTAR A MIGUELITO ANGELITO SI ESTA CORRECTITO
  

  printf("hola que tal ");

  pthread_mutex_init( &semaforo, NULL);

  writeLogMessage("1", "buenas ASDFASDFASDFtardes");

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

//       ---------------YA SABEMOS QUE TE DUELE MUCHO LA OREJA :(------------
//--------------------------------------------------------------------------------
//      -----------------------ESTAMOS CONTIGO DAVID------------------------


