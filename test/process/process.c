int main() {
	int pid;
	int status;
	int buf;
	char *str;
	str = "Yey";

	pid = fork();

	if(pid == 0) {
		printf(">>>>> This is child process [%d]\n\n", getpid());
		printf("Print str [%.*s]\n\n", 3, str);
		printf("Do command 'ls'\n");
		execl("/bin/ls", "ls", 0);
	} else {
		printf(">>>>> This is parent process [%d]\n\n", getpid());
		printf("Print str [%.*s]\n\n", 3, str);
		waitpid(pid, &status, 0);
		printf("\nChild process [%d]... end\n", pid);
	}	

	return 0;
}

