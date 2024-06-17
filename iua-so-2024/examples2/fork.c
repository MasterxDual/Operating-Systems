#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (){
	int pid;
	
	pid = fork();
	
	if(pid > 0){
		printf("En padre: %d\n", pid);
	}
	else{
		printf("En hijo: %d\n", pid);
	}
	
	
	return 0;
}
