#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>	
#include <unistd.h>

int main(){
	
	int forkid, pid, ppid, status;
	
	forkid = fork();
	pid = getpid();
	ppid = getppid();
	
	if(forkid > 0) {
		wait(&status);
		printf("Este es el ID del proceso padre actual: %d\n", pid);
		printf("Este es el ID del padre del proceso padre: %d\n", ppid);
		printf("Status de salida del hijo: %d\n", status);5
		printf("Analisis del status: %
	}
	else {
		
		printf("Este es el ID del proceso hijo actual: %d\n", pid);
		printf("Este es el ID del padre del proceso hijo: %d\n", ppid);
	}
	
	return 0;
}
