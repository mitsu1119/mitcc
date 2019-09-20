int main() {
	int pid;
	int status;
	int buf;
	char *str;
	str = "Yey";

	pid = fork();

	if(pid == 0) printf(">>>>> This is child process [%d]\n\n", getpid());
	if(pid == 0) printf("Print str [%.*s]\n\n", 3, str);
	if(pid == 0) printf("Do command 'ls'\n");
	if(pid == 0) execl("/bin/ls", "ls", 0);

	if(pid == 0) buf = 10;
	else printf(">>>>> This is parent process [%d]\n\n", getpid());
	if(pid == 0) buf = 10;
	else printf("Print str [%.*s]\n\n", 3, str);
	if(pid == 0) buf = 10;
	else waitpid(pid, &status, 0);
	if(pid == 0) buf = 10;
	else printf("\nChild process [%d]... end\n", pid);

	return 0;
}

