#include <string.h>
#include <stdio.h>

//para server UDP
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ctype.h>

//para server TCP
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

//void process_client_AGS(int fd);
void process_client_AAS(int fd);
void process_client_AGS(int fd);
void erro(char *msg);
void header(char client[3], char type[50]);

#define BUF_SIZE 5000

int main(){
	int aps = fork();   

	while (1){
		if(aps == 0){
		//INICIO DA DEFINIÇÃO DO SERVIDOR
		int udpSocket, nBytes;
		//, nSize;

		struct sockaddr_in serverAddr;
		//clientAddr;
		struct sockaddr_storage serverStorage;

		socklen_t addr_size;
		//client_addr_size;	

		udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7000);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

		bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

		addr_size = sizeof serverStorage;
		//FIM DA DEFINIÇÃO DO SERVIDOR

		char operacao[1];
		char id_login[10];
		char password[100];
		char registo[250];
		char crime[1000];
		FILE *credenciais_aps;
		FILE *fcrimes;
		FILE *reg_edit;
		FILE *reg_del;

		while(1){
			inicio:
			nBytes = recvfrom(udpSocket, operacao, 2, 0, (struct sockaddr *)&serverStorage, &addr_size);
			fflush(stdin);
			printf("OPÇÃO RECEBIDA DEBUG APS: %s\n", operacao);
			sleep(1);

			//OPERAÇÃO 1 - LOGIN
			if(operacao[0] == '1'){
				FILE *checks;
				char checkaux[250];
				
				nBytes = recvfrom(udpSocket, id_login, 100, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				header("APS", "LOGIN");
				printf("\n\nID: %s\n", id_login);
				fflush(stdout);

				nBytes = recvfrom(udpSocket, password, 100, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				printf("Password: %s\n", password);
				fflush(stdout);
				
				checks = fopen("DATABASE Files/saude_new.txt", "r"); //VERIFICA SE EXISTE NO X_NEW
				while(fscanf(checks, "%s\n", checkaux) != EOF){
					fflush(stdout);
					fflush(stdin);
					if(strstr(checkaux, id_login)){
						printf("\nID pendente aprovação, a informar cliente.\n");
						char new[4] = "NEW";
						nBytes = strlen(new)+1;
						sendto(udpSocket, new, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
						fflush(stdout);
						goto inicio;
					}
				}
				fclose(checks);
				
				checks = fopen("DATABASE Files/credenciais_saude.txt", "r");
				while(fscanf(checks, "%s\n", checkaux) != EOF){
					fflush(stdout);
					fflush(stdin);
					if(strstr(checkaux, id_login)){ //SE ENCONTRAR ID
						const char separador[2] = "\\";
						char *pw_tok;
						char *user_tok;
						
						pw_tok = strtok(checkaux, separador);
						user_tok = strtok(NULL, separador);
						pw_tok = strtok(NULL, separador);
						
						if(strstr(pw_tok, password)){
							nBytes = strlen(user_tok)+1;
							sendto(udpSocket, user_tok, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
							printf("\nCredenciais corretas, a permitir ligação.\n");
							fflush(stdout);
							goto inicio;
						}
					}
				}

				char isnt[1] = "0";
				nBytes = strlen(isnt)+1;
				sendto(udpSocket, isnt, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
				printf("\nCredenciais incorretas, a informar o cliente.\n");
				fflush(stdout);
				//FALTA COMPLETAR
			}

			//OPERAÇÃO 2 - REGISTO
			if(operacao[0] == '2'){
				nBytes = recvfrom(udpSocket, registo, 250, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				
				const char separador[2] = "\\";
				char *id_tok;
				char aux[250];
				strcpy(aux, registo);
				id_tok = strtok(aux, separador);
				
				FILE *checks;
				char checkaux[250];
				
				header("APS", "REGISTO");
				printf("\n\nNovo registo: %s", registo);
				
				checks = fopen("DATABASE Files/credenciais_saude.txt", "r");
				while(fscanf(checks, "%s\n", checkaux) != EOF){
					fflush(stdin);
					if(strstr(checkaux, id_tok)){
						printf("\n\nID já em uso, a informar cliente.\n");
						nBytes = strlen("USE")+1;
						sendto(udpSocket, "USE", nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
						fflush(stdout);
						goto inicio;
					}
				}
				fclose(checks);
				
				checks = fopen("DATABASE Files/saude_new.txt", "r");
				while(fscanf(checks, "%s\n", checkaux) != EOF){
					fflush(stdin);
					if(strstr(checkaux, id_tok)){
						printf("\n\nID pendente aprovação, a informar cliente.\n");
						nBytes = strlen("NEW")+1;
						sendto(udpSocket, "NEW", nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
						fflush(stdout);
						goto inicio;
					}
				}
				fclose(checks);

				credenciais_aps = fopen("DATABASE Files/saude_new.txt", "a");
				fprintf(credenciais_aps, "%s\n", registo);
				fflush(stdout);
				printf("\n\nUtilizador registado com sucesso!\n\n");
				fclose(credenciais_aps);
				
				nBytes = strlen("1")+1;
				sendto(udpSocket, "1", nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
				fflush(stdout);
				sleep(3);
			}

			//OPERAÇÃO 3 - Botão de emergencia
			if(operacao[0] == '3'){
				char localEmergencia[40];
				nBytes = recvfrom(udpSocket, localEmergencia, 40, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				
				printf("Local da ocorrencia: %s\n",localEmergencia);

				//Enviar a informação para o AAS
				//nBytes = strlen("1")+1;
				//sendto(udpSocket, "1", nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
				//fflush(stdout);
				sleep(3);
			}


			//OPERACAO 7 - SUBMETER CRIME
			if(operacao[0] == '7'){
				nBytes = recvfrom(udpSocket, crime, 1000, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				header("APS", "CRIME");
				printf("\n\nNovo crime recebido: %s", crime);

				fcrimes = fopen("DATABASE Files/crimes.txt", "a");
				fprintf(fcrimes, "%s\n", crime);
				fflush(stdout);
				printf("\n\nCrime registado com sucesso!\n");
				fclose(fcrimes);
				sleep(3);				
			}

			//OPERAÇAO 8 - EDITAR REGISTO - EDIT
			if(operacao[0] == '8'){
				nBytes = recvfrom(udpSocket, registo, 250, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				header("APS", "REG EDIT");
				printf("\n\nNova edição de registo recebida: %s", registo);
				reg_edit = fopen("DATABASE Files/credenciais_saude.txt", "r");
				
				char reg_aps[1000][250];
				char aux[250];
				int regaps_count = 0;
				
				strcpy(aux, registo);
				const char separador[2] = "\\";
				char *id_tok;
						
				id_tok = strtok(aux, separador);
				
				while(fscanf(reg_edit, "%s\n", reg_aps[regaps_count]) != EOF){
					fflush(stdin);
					regaps_count++;
				}
				fclose(reg_edit);
				
				for(int i = 0; i < regaps_count; i++){
					if(strncmp(reg_aps[i], id_tok, 8) == 0){
						strcpy(reg_aps[i], registo);
						break;
					}
				}
				
				reg_edit = fopen("DATABASE Files/credenciais_saude.txt", "w");
				for(int i = 0; i < regaps_count; i++){
					fprintf(reg_edit, "%s\n", reg_aps[i]);
					fflush(stdout);
				}
				printf("\n\nEdição registada com sucesso!\n");
				fclose(reg_edit);
				sleep(2);
			}

			//OPERÇÃO 9 - APAGAR REGISTO - EDIT
			if(operacao[0] == '9'){
				nBytes = recvfrom(udpSocket, registo, 250, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fflush(stdin);
				header("APS", "REG DEL");
				printf("\n\nNovo apagamento de registo recebido: %s\n", registo);
				reg_del = fopen("DATABASE Files/credenciais_saude.txt", "r");
				
				char reg_aps[1000][250];
				char aux[250];
				int regaps_count = 0;
				
				strcpy(aux, registo);
				const char separador[2] = "\\";
				char *id_tok;
				
				id_tok = strtok(aux, separador);
				
				while(fscanf(reg_del, "%s\n", reg_aps[regaps_count]) != EOF){
					fflush(stdin);
					regaps_count++;
				}
				fclose(reg_del);
				
				for(int i = 0; i < regaps_count; i++){
					if(strncmp(reg_aps[i], id_tok, 8) == 0){
						strcpy(reg_aps[i], "DEL");
						break;
					}
				}
				
				reg_del = fopen("DATABASE Files/credenciais_saude.txt", "w");
				for(int i = 0; i < regaps_count; i++){
					if(strcmp(reg_aps[i], "DEL") == 0){
						continue;
					}
					fprintf(reg_del, "%s\n", reg_aps[i]);
					fflush(stdout);
				}
				printf("n\nApagamento registado com sucesso!\n");
				fclose(reg_del);
				sleep(2);
				}
			}
		}

		else{
			int aas=fork();
			
			//FORK AAS (TCP)
			if(aas == 0){
				//INICIO DA DEFINIÇÃO DO SERVIDOR
				#define SERVER_Port  8000

				int fd, client;
				struct sockaddr_in addr, client_addr;
				int client_addr_size;

				//bzero((void *) &addr, sizeof(addr));
				addr.sin_family      = AF_INET;
				addr.sin_addr.s_addr = htonl(INADDR_ANY);
				addr.sin_port        = htons(SERVER_Port);

				if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
				erro("na funcao socket");
				if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
				erro("na funcao bind");
				if( listen(fd, 5) < 0) 
				erro("na funcao listen");

				int nclientes=0;

				while (1){
					client_addr_size = sizeof(client_addr);
					client = accept(fd,(struct sockaddr *)&client_addr, &client_addr_size);
					nclientes++;
					if (client > 0){
						if (fork() == 0){
							close(fd);
							process_client_AAS(client);
							exit(0);
						}
						close(client);
					}
				}
				return 0;  
			}//FIM DA DEFINIÇÃO DO SERVIDOR
			
			//FORK AGS (TCP)
			else{
				//INICIO DA DEFINIÇÃO DO SERVIDOR			
				#define SERVER  9000

				int fd, client;
				struct sockaddr_in addr, client_addr;
				int client_addr_size;

				//bzero((void *) &addr, sizeof(addr));
				addr.sin_family      = AF_INET;
				addr.sin_addr.s_addr = htonl(INADDR_ANY);
				addr.sin_port        = htons(SERVER);

				if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
				erro("na funcao socket");
				if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
				erro("na funcao bind");
				if( listen(fd, 5) < 0) 
				erro("na funcao listen");

				int nclientes=0;

				while (1){
					client_addr_size = sizeof(client_addr);
					client = accept(fd,(struct sockaddr *)&client_addr, &client_addr_size);
					nclientes++;
					if (client > 0){
						if (fork() == 0){
							close(fd);
							process_client_AGS(client);
							exit(0);
						}
						close(client);
					}
				}
				return 0;  
			}//FIM DA DEFINIÇÂO DO SERVIDOR			
		}        
	}	
	return 0;
}

 void process_client_AAS(int client_fd){
	 char operacao[1];
	 char id_login[10];
	 char password[100];
	 char registo[250];
	 FILE *credenciais_aas;
	 FILE *fcrimes;
	 FILE *reg_edit;
	 FILE *reg_del;
	 
	 int nread = 0;
	 
	 while(1){
		 inicio_aas:
		 operacao[0] = '0';
		 nread = read(client_fd, operacao, BUF_SIZE-1);
		 operacao[nread] = '\0';
		 fflush(stdin);
		 printf("OPÇÃO RECEBIDA DEBUG AAS: %s\n", operacao);
		 sleep(1);
		 
		 //OPERAÇÃO 1 - LOGIN
		 if(operacao[0] == '1'){
			 FILE *checks;
			 char checkaux[250];
			 
			 nread = read(client_fd, id_login, BUF_SIZE-1);
			 id_login[nread] = '\0';
			 fflush(stdin);
			 header("AAS", "LOGIN");
			 printf("\n\nID: %s\n", id_login);
			 fflush(stdout);
			 
			 nread = read(client_fd, password, BUF_SIZE-1);
			 password[nread] = '\0';
			 fflush(stdin);
			 printf("Password: %s\n", password);
			 fflush(stdout);
			 
			 checks = fopen("DATABASE Files/psp_new.txt", "r"); //VERIFICA SE EXISTE NO X_NEW
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				fflush(stdout);
				fflush(stdin);
				if(strstr(checkaux, id_login)){
					printf("\n\nID pendente aprovação, a informar cliente.\n");
					write(client_fd, "NEW", strlen("NEW"));
					fflush(stdout);
					goto inicio_aas;
				}
			}
			fclose(checks);
			
			checks = fopen("DATABASE Files/credenciais_psp.txt", "r");
			while(fscanf(checks, "%s\n", checkaux) != EOF){
				fflush(stdout);
				fflush(stdin);
				if(strstr(checkaux, id_login)){ //SE ENCONTRAR ID
					const char separador[2] = "\\";
					char *pw_tok;
					char *user_tok;
					
					pw_tok = strtok(checkaux, separador);
					user_tok = strtok(NULL, separador);
					pw_tok = strtok(NULL, separador);
					
					if(strstr(pw_tok, password)){
						write(client_fd, user_tok, strlen(user_tok));
						printf("\nCredenciais corretas, a permitir ligação.\n");
						fflush(stdout);
						goto inicio_aas;
					}
				}
			}
			write(client_fd, "0", strlen("0"));
			printf("\nCredenciais incorretas, a informar o cliente.\n");
			fflush(stdout);
			//FALTA COMPLETAR
		 }
		 
		 //OPERAÇÃO 2 - REGISTO
		 if(operacao[0] == '2'){
			 nread = read(client_fd, registo, BUF_SIZE-1);
			 registo[nread] = '\0';
			 fflush(stdin);
			 
			 const char separador[2] = "\\";
			 char *id_tok;
			 char aux[250];
			 strcpy(aux, registo);
			 id_tok = strtok(aux, separador);
			 
			 FILE *checks;
			 char checkaux[250];
			 
			 header("AAS", "REGISTO");
			 printf("\n\nNovo registo: %s", registo);
			 
			 checks = fopen("DATABASE Files/credenciais_psp.txt", "r");
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				 fflush(stdin);
				 if(strstr(checkaux, id_tok)){
					 printf("\n\nID já em uso, a informar cliente.\n");
					 write(client_fd, "USE", strlen("USE"));
					 fflush(stdout);
					 goto inicio_aas;
				 }
			 }
			 fclose(checks);
			 
			 checks = fopen("DATABASE Files/psp_new.txt", "r");
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				 fflush(stdin);
				 if(strstr(checkaux, id_tok)){
					 printf("\n\nID pendente aprovação, a informar cliente.\n");
					 write(client_fd, "NEW", strlen("NEW"));
					 fflush(stdout);
					 goto inicio_aas;
				 }
			 }
			 fclose(checks);
			 
			 credenciais_aas = fopen("DATABASE Files/psp_new.txt", "a");
			 fprintf(credenciais_aas, "%s\n", registo);
			 fflush(stdout);
			 printf("\n\nUtilizador registado com sucesso!\n");
			 fclose(credenciais_aas);
			 
			 write(client_fd, "1", sizeof("1"));
			 fflush(stdout);
			 sleep(3);
		 }
		 
		 //OPERAÇAO 3 - SAIR
		 if(operacao[0] == '3'){
			 header("AAS", "EXIT");
			 printf("\n\nUtilizador desconectado\n");
			 
			 fflush(stdin);
			 fflush(stdout);
			 close(client_fd);
			 exit(1);
		}
		 
		 //OPERAÇÃO 7 - VER CRIMES
		 if(operacao[0] == '7'){
			 header("AAS", "CRIMES");
			 fcrimes = fopen("DATABASE Files/crimes.txt", "r");
			 fflush(stdin);
			 
			 char crimes[1000][200];
			 
			 printf("\n\nLista de crimes:\n");
			 int crime_count = 0; 
			 while(fscanf(fcrimes, "%s\n", crimes[crime_count]) != EOF){
				 fflush(stdin);
				 printf("%d - %s\n", crime_count, crimes[crime_count]);
				 write(client_fd, crimes[crime_count], strlen(crimes[crime_count]));
				 fflush(stdout);
				 sleep(0.5);
				 crime_count++;
			 }
			 write(client_fd, "Done", strlen("Done"));
			 fflush(stdout);	 
			 printf("\nCrimes enviados com sucesso!\n");
			 fclose(fcrimes);
			 sleep(1);
		 }
		 
		 //OPERAÇÃO 8 - EDITAR REGISTO
		 if(operacao[0] == '8'){
			 nread = read(client_fd, registo, BUF_SIZE-2);
			 registo[nread] = '\0';
			 fflush(stdin);
			 header("AAS", "REG EDIT");
			 printf("\n\nNova edição de registo recebida: %s", registo);
			 reg_edit = fopen("DATABASE Files/credenciais_psp.txt", "r");
			 
			 char reg_aas[1000][250];
			 char aux[250];
			 int regaas_count = 0;
			 
			 strcpy(aux, registo);
			 const char separador[2] = "\\";
			 char *id_tok;
			 
			 id_tok = strtok(aux, separador);
			 
			 while(fscanf(reg_edit, "%s\n", reg_aas[regaas_count]) != EOF){
				 fflush(stdin);
				 regaas_count++;
			 }
			 fclose(reg_edit);
			 
			 for(int i = 0; i < regaas_count; i++){
				 if(strncmp(reg_aas[i], id_tok, 8) == 0){
					 strcpy(reg_aas[i], registo);
					 break;
				 }
			 }
			 
			 reg_edit = fopen("DATABASE Files/credenciais_psp.txt", "w");
			 for(int i = 0; i < regaas_count; i++){
				 fprintf(reg_edit, "%s\n", reg_aas[i]);
				 fflush(stdout);
			 }
			 printf("\n\nEdição registada com sucesso!\n");
			 fclose(reg_edit);
			 sleep(2);
		 }
		 
		 //OPERÇÃO 9 - APAGAR REGISTO
		 if(operacao[0] == '9'){
			 nread = read(client_fd, registo, BUF_SIZE-1);
			 registo[nread] = '\0';
			 fflush(stdin);
			 header("AAS", "REG DEL");
			 printf("\n\nNovo apagamento de registo recebido: %s", registo);
			 reg_del = fopen("DATABASE Files/credenciais_psp.txt", "r");
			 
			 char reg_aas[1000][250];
			 char aux[250];
			 int regaas_count = 0;
			 
			 strcpy(aux, registo);
			 const char separador[2] = "\\";
			 char *id_tok;
			 
			 id_tok = strtok(aux, separador);
			 
			 while(fscanf(reg_del, "%s\n", reg_aas[regaas_count]) != EOF){
				 fflush(stdin);
				 regaas_count++;
			 }
			 fclose(reg_del);
			 
			 for(int i = 0; i < regaas_count; i++){
				 if(strncmp(reg_aas[i], id_tok, 8) == 0){
					 strcpy(reg_aas[i], "DEL");
					 break;
				 }
			 }
			 
			 reg_del = fopen("DATABASE Files/credenciais_psp.txt", "w");
			 for(int i = 0; i < regaas_count; i++){
				 if(strcmp(reg_aas[i], "DEL") == 0){
					 continue;
				 }
				 fprintf(reg_del, "%s\n", reg_aas[i]);
				 fflush(stdout);
			 }
			 printf("\n\nApagamento registado com sucesso!\n");
			 fclose(reg_del);
			 sleep(2);
		 }
	 }
 }

void process_client_AGS(int client_fd){
	 char operacao[1];
	 char id_login[10];
	 char password[100];
	 char registo[250];
	
	 char id_accept[10];
	 FILE *credenciais_ags;
	 FILE *newreg_aas;
	 FILE *newreg_aps;
	 
	 //CREDENCIAIS EXISTENTES
	 char regexist_aas[1000][250];
	 char regexist_aps[1000][250];
	 
	 int nread = 0;
	 
	 while(1){
		 inicio_ags:
		 operacao[0] = '0';
		 nread = read(client_fd, operacao, BUF_SIZE-1);
		 operacao[nread] = '\0';
		 fflush(stdin);
		 printf("OPÇÃO RECEBIDA DEBUG AGS: %s\n", operacao);
		 sleep(1);
		 
		 //OPERAÇÃO 1 - LOGIN
		 if(operacao[0] == '1'){
			 FILE *checks;
			 char checkaux[250];
			 
			 nread = read(client_fd, id_login, BUF_SIZE-1);
			 id_login[nread] = '\0';
			 fflush(stdin);
			 header("AGS", "LOGIN");
			 printf("\n\nID: %s\n", id_login);
			 fflush(stdout);
			 
			 nread = read(client_fd, password, BUF_SIZE-1);
			 password[nread] = '\0';
			 fflush(stdin);
			 printf("Password: %s\n", password);
			 fflush(stdout);
			 
			 checks = fopen("DATABASE Files/gestor_new.txt", "r"); //VERIFICA SE EXISTE NO X_NEW
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				 fflush(stdout);
				fflush(stdin);
				if(strstr(checkaux, id_login)){
					printf("\n\nID pendente aprovação, a informar cliente.\n");
					write(client_fd, "NEW", strlen("NEW"));
					fflush(stdout);
					goto inicio_ags;
				}
			}
			fclose(checks);
			
			checks = fopen("DATABASE Files/credenciais_gestor.txt", "r");
			while(fscanf(checks, "%s\n", checkaux) != EOF){
				fflush(stdout);
				fflush(stdin);
				if(strstr(checkaux, id_login)){ //SE ENCONTRAR ID
					const char separador[2] = "\\";
					char *pw_tok;
					char *user_tok;
					
					pw_tok = strtok(checkaux, separador); //1º Tok: ID
					user_tok = strtok(NULL, separador); //2º Tok: User
					pw_tok = strtok(NULL, separador); //3º Tok: Pass
						
					if(strstr(pw_tok, password)){
						write(client_fd, user_tok, strlen(user_tok));
						printf("\nCredenciais corretas, a permitir ligação.\n");
						fflush(stdout);
						goto inicio_ags;
					}
				}
			}
			
			write(client_fd, "0", strlen("0"));
			printf("\nCredenciais incorretas, a informar o cliente.\n");
			fflush(stdout);
			//FALTA COMPLETAR
		 }
		 
		 //OPERAÇÃO 2 - REGISTO - WORKING
		 if(operacao[0] == '2'){
			 nread = read(client_fd, registo, BUF_SIZE-1);
			 registo[nread] = '\0';
			 fflush(stdin);
			 
			 const char separador[2] = "\\";
			 char *id_tok;
			 char aux[250];
			 strcpy(aux, registo);
			 id_tok = strtok(aux, separador);
			 
			 FILE *checks;
			 char checkaux[250];
			 
			 header("AGS", "REGISTO");
			 printf("\n\nNovo registo: %s", registo);

			 checks = fopen("DATABASE Files/credenciais_gestor.txt", "r");
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				 fflush(stdin);
				 if(strstr(checkaux, id_tok)){
					 printf("\n\nID já em uso, a informar cliente.\n");
					 write(client_fd, "USE", strlen("USE"));
					 fflush(stdout);
					 goto inicio_ags;
				 }
			 }
			 fclose(checks);
			 
			 checks = fopen("DATABASE Files/gestor_new.txt", "r");
			 while(fscanf(checks, "%s\n", checkaux) != EOF){
				 fflush(stdin);
				 if(strstr(checkaux, id_tok)){
					 printf("\n\nID pendente aprovação, a informar cliente.\n");
					 write(client_fd, "NEW", strlen("NEW"));
					 fflush(stdout);
					 goto inicio_ags;
				 }
			 }
			 fclose(checks);
			 
			 credenciais_ags = fopen("DATABASE Files/gestor_new.txt", "a");
			 fprintf(credenciais_ags, "%s\n", registo);
			 fflush(stdout);
			 printf("\n\nUtilizador registado com sucesso!\n");
			 fclose(credenciais_ags);
			 
			 write(client_fd, "1", sizeof("1"));
			 fflush(stdout);
			 sleep(3);
		 }
		 
		 //OPERAÇAO 3 - SAIR - WORKING
		 if(operacao[0] == '3'){
			 header("AGS", "EXIT");
			 printf("\n\nUtilizador desconectado\n");
			 
			 fflush(stdin);
			 fflush(stdout);
			 close(client_fd);
			 exit(1);
		}
		 
		 //OPERAÇÃO 7 - AUTORIZAR NOVOS REGISTOS
		 if(operacao[0] == '7'){
			 header("AGS", "REG NEW AUTH");
			 newreg_aps = fopen("DATABASE Files/saude_new.txt", "r");
			 newreg_aas = fopen("DATABASE Files/psp_new.txt", "r");
			 
			 char new_aps[250][250];
			 char new_aas[250][250];
			 
			 int count = 0;
			 while(fscanf(newreg_aps, "%s\n", new_aps[count]) != EOF){
				 fflush(stdin);
				 write(client_fd, new_aps[count], sizeof(new_aps[count]));
				 fflush(stdout);
				 sleep(0.5);
				 count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 printf("\n\nNovos registos de saúde enviados!\n");
			 fclose(newreg_aps);
			 sleep(1);
			 
			 count = 0;
			 while(fscanf(newreg_aas, "%s\n", new_aas[count]) != EOF){
				 fflush(stdin);
				 write(client_fd, new_aas[count], sizeof(new_aas[count]));
				 sleep(0.5);
				 fflush(stdout);
				 count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 printf("\nNovos registos de segurança enviados!\n");
			 fclose(newreg_aas);
			 sleep(1);
			 
			 newreg_aps = fopen("DATABASE Files/credenciais_saude.txt", "a");
			 newreg_aas = fopen("DATABASE Files/credenciais_psp.txt", "a");
			 
			 while(1){
				 nread = read(client_fd, id_accept, BUF_SIZE-1);
				 id_accept[nread] = '\0';
				 fflush(stdin);
				 
				 if(id_accept[0] == 's' || id_accept[0] == 'S'){
					 printf("\nSaida do accept ID\n");
					 break;
				 }
				 
				 for(int i = 0; i < 250; i++){
					 if(strstr(new_aps[i], id_accept)){
						 fprintf(newreg_aps, "%s\n", new_aps[i]);
						 fflush(stdout);
						 strcpy(new_aps[i], "DEL");
						 printf("\nID aceite: %s\n", new_aps[i]);
						 break;
					 }
					 
					 else if(strstr(new_aas[i], id_accept)){
						 fprintf(newreg_aas, "%s\n", new_aas[i]);
						 fflush(stdout);
						 strcpy(new_aas[i], "DEL");
						 printf("\nID aceite: %s\n", new_aas[i]);
						 break;
					 }
				 }
			 }
			 fclose(newreg_aps);
			 fclose(newreg_aas);
			 
			 newreg_aps = fopen("DATABASE Files/saude_new.txt", "w");
			 newreg_aas = fopen("DATABASE Files/psp_new.txt", "w");
			 
			 for(int i = 0; i < 250; i++){
				 if(strcmp(new_aps[i], "\0") == 0){
					 break;
				 }
				 if(strcmp(new_aps[i], "DEL") == 0){
					 continue;
				 }
				 fprintf(newreg_aps, "%s\n", new_aps[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aps);
			 
			 for(int i = 0; i < 250; i++){
				 if(strcmp(new_aas[i], "\0") == 0){
					 break;
				 }
				 if(strcmp(new_aas[i], "DEL") == 0){
					 continue;
				 }
				 fprintf(newreg_aas, "%s\n", new_aas[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aas);
			 
			 printf("\nNovos registos aceites com sucesso!\n");
			 sleep(3);
		 }
		 
		 //OPERAÇÃO 8 - EDITAR REGISTOS
		 if(operacao[0] == '8'){
			 header("AGS", "REG EDIT AUTH");
			 newreg_aps = fopen("DATABASE Files/credenciais_saude.txt", "r");
			 newreg_aas = fopen("DATABASE Files/credenciais_psp.txt", "r");
			 
			 int regaps_count = 0;
			 while(fscanf(newreg_aps, "%s\n", regexist_aps[regaps_count]) != EOF){
				 fflush(stdin);
				 write(client_fd, regexist_aps[regaps_count], sizeof(regexist_aps[regaps_count]));
				 fflush(stdout);
				 sleep(0.5);
				 regaps_count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 
			 printf("\nLista de registos de saúde (%d quant): \n", regaps_count);
			 for(int i = 0; i < regaps_count; i++){
				 printf("\n%d - %s", i, regexist_aps[i]);
			 }
			 
			 printf("\n\nRegistos de saúde enviados!\n");
			 fclose(newreg_aps);
			 sleep(1);
			 
			 int regaas_count = 0;
			  while(fscanf(newreg_aas, "%s\n", regexist_aas[regaas_count]) != EOF){
				 fflush(stdin);
				 write(client_fd, regexist_aas[regaas_count], sizeof(regexist_aas[regaas_count]));
				 fflush(stdout);
				 sleep(0.5);
				 regaas_count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 
			 printf("\nLista de registos de segurança (%d quant): \n", regaas_count);
			 for(int i = 0; i < regaas_count; i++){
				 printf("\n%d - %s", i, regexist_aas[i]);
			 }
			 
			 printf("\n\nRegistos de segurança enviados!\n");
			 fclose(newreg_aas);
			 sleep(1);
			 
			nread = read(client_fd, registo, BUF_SIZE-1);
			fflush(stdin);
			registo[nread] = '\0';
			
			const char separador[2] = "\\";
			char *id_tok;
			char aux[250];
			
			strcpy(aux, registo);
			id_tok = strtok(aux, separador);
			
			for(int i = 0; i < regaps_count; i++){
				if(strncmp(regexist_aps[i], id_tok, 8) == 0){ //SE ID ESTIVER NA LISTA DE EDIÇÕES ENTRA NO IF
					strcpy(regexist_aps[i], registo); //TROCA REG EXISTENTE PARA O NOVO
					printf("Registo aceite: %s\n", registo);
					break;
				}
			}
			 
			for(int i = 0; i < regaas_count; i++){
				if(strncmp(regexist_aas[i], id_tok, 8) == 0){
					 strcpy(regexist_aas[i], registo);
					 printf("Registo aceite: %s\n", registo);
					 break;
				}
			}
		 
			 for(int i = 0; i < regaps_count; i++){
				printf("Regexist_aps %d: %s\n", i, regexist_aps[i]);
			}
			 
			 for(int i = 0; i < regaas_count; i++){
				printf("Regexist_aas %d: %s\n", i, regexist_aas[i]);
			}
			
			 newreg_aps = fopen("DATABASE Files/credenciais_saude.txt", "w");
			 newreg_aas = fopen("DATABASE Files/credenciais_psp.txt", "w");
			 
			 for(int i = 0; i < regaps_count; i++){
				 printf("\n\nEntrou fprintf 1");
				 fprintf(newreg_aps, "%s\n", regexist_aps[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aps);
			 
			 for(int i = 0; i < regaas_count; i++){
				 printf("\n\nEntrou fprintf 2");
				 fprintf(newreg_aas, "%s\n", regexist_aas[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aas);
			 
			 printf("\n\nEdição de registos aceites com sucesso!\n");
			 sleep(2);
		 }
		 
		 //OPERÇÃO 9 - APAGAR REGISTO
		 if(operacao[0] == '9'){
			 header("AGS", "REG DEL AUTH");
			 newreg_aps = fopen("DATABASE Files/credenciais_saude.txt", "r");
			 newreg_aas = fopen("DATABASE Files/credenciais_psp.txt", "r");
			 
			 int regaps_count = 0;
			 while(fscanf(newreg_aps, "%s\n", regexist_aps[regaps_count]) != EOF){
				fflush(stdin);
				write(client_fd, regexist_aps[regaps_count], sizeof(regexist_aps[regaps_count]));
				fflush(stdout);
				sleep(0.5);
				regaps_count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 
			 printf("\nLista de registos de saúde (%d quant): \n", regaps_count);
			 for(int i = 0; i < regaps_count; i++){
				 printf("\n%d - %s", i, regexist_aps[i]);
			 }
			 
			 printf("\n\nRegistos de saúde enviados!\n");
			 fclose(newreg_aps);
			 sleep(1);
			 
			 int regaas_count = 0;
			 while(fscanf(newreg_aas, "%s\n", regexist_aas[regaas_count]) != EOF){
				 fflush(stdin);
				 write(client_fd, regexist_aas[regaas_count], sizeof(regexist_aas[regaas_count]));
				 fflush(stdout);
				 sleep(0.5);
				 regaas_count++;
			 }
			 write(client_fd, "Done", sizeof("Done"));
			 fflush(stdout);
			 
			 printf("\nLista de registos de segurança (%d quant): \n", regaas_count);
			 for(int i = 0; i < regaas_count; i++){
				 printf("\n%d - %s", i, regexist_aas[regaas_count]);
			 }
			 
			 printf("\n\nRegistos de segurança enviados!\n");
			 fclose(newreg_aas);
			 sleep(1);
			 
			 nread = read(client_fd, id_accept, BUF_SIZE-1);
			 fflush(stdin);
			 id_accept[nread] = '\0';
			 
			for(int i = 0; i < regaps_count; i++){
				if(strncmp(regexist_aps[i], id_accept, 8) == 0){
					printf("Registo apagado: %s\n", regexist_aps[i]);
					fflush(stdout);
					strcpy(regexist_aps[i], "DEL");
					break;
				}
			}
			
			for(int i = 0; i < regaas_count; i++){
				if(strncmp(regexist_aas[i], id_accept, 8) == 0){
					printf("Registo apagado: %s\n", regexist_aas[i]);
					fflush(stdout);
					strcpy(regexist_aas[i], "DEL");
					break;
				}
			}	
					
			 for(int i = 0; i < regaps_count; i++){
				 printf("Regexist_aps %d: %s\n", i, regexist_aps[i]);
			 }
			 
			 for(int i = 0; i < regaas_count; i++){
				 printf("Regexist_aas %d: %s\n", i, regexist_aas[i]);
			 }
			 
			 newreg_aps = fopen("DATABASE Files/credenciais_saude.txt", "w");
			 newreg_aas = fopen("DATABASE Files/credenciais_psp.txt", "w");
			 
			 for(int i = 0; i < regaps_count; i++){
				 if(strcmp(regexist_aps[i], "DEL") == 0){
					 continue;
				 }
				 printf("\n\nEntrou fprintf 1");
				 fprintf(newreg_aps, "%s\n", regexist_aps[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aps);
			 
			 for(int i = 0; i < regaas_count; i++){
				 if(strcmp(regexist_aas[i], "DEL") == 0){
					 continue;
				 }
				 printf("\n\nEntrou fprintf 2");
				 fprintf(newreg_aas, "%s\n", regexist_aas[i]);
				 fflush(stdout);
			 }
			 fclose(newreg_aas);
	
			 printf("\n\Eliminação de registos aceites com sucesso!\n");
			 sleep(2);
		 }
	 }
 }

void header(char client[3], char type[5]){
	printf("===============CONEXÃO %s - %s===============", client, type);
	return;
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}
