#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#define DRIVERS_NUMBER 3
#define INTERFACE_NUMBER 3
char drivers_name[DRIVERS_NUMBER][16] = {"rtl8822be","rt2800pci","rtw89_8852be"};
char interfaces_name[INTERFACE_NUMBER][8] = {"wlan0","wlo1","wlp2s0"};
int exec_command_with_arg_and_return_int(char* command,char* arg){
  FILE* f;
  int i,status;
  char* command_with_arg;
  command_with_arg = malloc(strlen(command)+strlen(arg)+1);
  sprintf(command_with_arg,command,arg);
  f = popen(command_with_arg,"r");
  fscanf(f,"%d",&status);
  pclose(f);
  free(command_with_arg);
  return status;
}
char* detect_driver(){
  int i,status;
  for(i=0;i<DRIVERS_NUMBER;i++){
      status = exec_command_with_arg_and_return_int("lsmod | grep %s | wc -l",drivers_name[i]);
      if(status>0){
	printf("Driver detected:%s\n",drivers_name[i]);
        return drivers_name[i];
      }
  }
  return 0;
}
char* detect_interface(){
  int i,status;
  for(i=0;i<INTERFACE_NUMBER;i++){
    status = exec_command_with_arg_and_return_int("ifconfig | grep %s | wc -l",interfaces_name[i]);
    if(status>0){
      printf("Interface detected:%s\n",interfaces_name[i]);
      return interfaces_name[i];
    }
  }
}
int test(char* interface_name){
    return exec_command_with_arg_and_return_int("arping -I %s 192.168.1.1 -c 1 -f -w 2 | wc -l",interface_name);
}
int worker(char* driver_name,char* interface_name){
  int errores=0;
  int umbral;
  umbral=2;
  char driver_stop_command[64];
  char driver_start_command[64];
  sprintf(driver_stop_command,"modprobe -r %s",driver_name);
  sprintf(driver_start_command,"modprobe %s",driver_name);
  while(1){
    sleep(10);
    printf("Performing test");
    int lines = test(interface_name);
    printf("Lines obtained:%d\n",lines);
    if(lines!=4){
      errores++;
    }else{
      umbral=2;
      errores=0;
    }
    printf("%d\n",errores);
    if(errores==umbral){
      errores=0;
      umbral++;
      if(umbral==5){
        umbral=2;
      }
      printf("Restart triggered\n");
      system("systemctl stop NetworkManager");
      system(driver_stop_command);//rtl8822be
      system(driver_start_command);
      system("systemctl start NetworkManager");
    }
  }
}
int main(){
  FILE* archivo = NULL;
  char* driver;
  char* interface;
  archivo = fopen("/var/run/networkwatchdogd.pid","w");
  if(archivo!=NULL){
    fprintf(archivo,"%d\n",getpid());
    fclose(archivo);
  }
  sleep(60);
  driver = detect_driver();
  interface = detect_interface();
  worker(driver,interface);
}
