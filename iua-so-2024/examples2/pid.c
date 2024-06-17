#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	
	int forkid, pid, ppid;
	
	forkid = fork();
	pid = getpid();
	ppid = getppid();
	
	if(forkid > 0) {
		printf("Este es el ID del proceso padre actual: %d\n", pid);
		printf("Este es el ID del padre del proceso padre: %d\n", ppid);
	}
	else {
		
		printf("Este es el ID del proceso hijo actual: %d\n", pid);
		printf("Este es el ID del padre del proceso hijo: %d\n", ppid);
	}
	
	return 0;
}
