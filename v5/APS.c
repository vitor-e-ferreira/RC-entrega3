//APLICAÇÃO PARA PROFISSIONAL DE SAÚDE (APS)

//SOCKET USADO: UDP (SENDTO, RECVFROM)

//------------------------------------------------------------------Includes--------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
//---------------------------------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------Structs--------------------------------------------------------------------------------
typedef struct Utilizador{
	char id[10];
	char username[100];
	char password[100];
} Utilizador;

struct infomarcao{
    char data[40];
    char local[40];
    char crime[40];
    char nome[40];
    char hora[10];
}info;
//---------------------------------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------Declaração de funções gerais--------------------------------------------------------------
void header(void);
int alarme();
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------Função Main-------------------------------------------------------------------------------
int main(){

	//-----------------------------------------------------------DEFINIÇÃO DO SERVIDOR--------------------------------------------------------------------
	int clientSocket, nBytes;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;
	
	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7000);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	addr_size = sizeof serverAddr;
	//---------------------------------------------------------------------------------------------------------------------------------------------------------
	

	//--------------------------------------------------------VARIÁVEIS DE MENU--------------------------------------------------------------------------------
	char menu1[1];
	char menu2[1];
	char menuregisto[1];
	char menuapagar;
	char returnmenu;
	//---------------------------------------------------------------------------------------------------------------------------------------------------------


	//------------------------------------------------------VARIÁVEIS DE CREDENCIAIS---------------------------------------------------------------------------
	Utilizador user;
	Utilizador edit;
	char registo[250];
	int dia, mes, ano, anon;
	char crime[250];
	char help[5000];
	//---------------------------------------------------------------------------------------------------------------------------------------------------------


	//CICLO WHILE PARA CORRER PROGRAMA SEM FECHAR
	while(1){
		//-----------------------------------------------------------------MENU DE LOGIN & Registo--------------------------------------------------------------
		loginmenu:
		while(1){
			menu1[0] = 'a';
			system("clear");
			header();
			printf("\n\n1 - Login\n");
			printf("2 - Registo\n");
			printf("3 - ⚠️  EMERGÊNCIA ⚠️\n");
			printf("4 - Sair\n");
			printf("\nInsira a sua escolha: ");
			scanf("%s", menu1);
			fflush(stdin);
			
			if(menu1[0] == '1' || menu1[0] == '2' || menu1[0] == '3' || menu1[0] == '4' ){
				break;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 1 - LOGIN------------------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '1'){
			nBytes = strlen("1")+1;
			sendto(clientSocket, "1", nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
			fflush(stdout);
			
			char confirm[50];
			
			while(1){
				system("clear");
				header();
				printf("\n\nInsira o seu ID de profissional (APS_xxxx): ");
				scanf("%s", user.id);
				fflush(stdin);
				nBytes=strlen(user.id)+1;
				sendto(clientSocket,user.id,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);
				fflush(stdout);

				printf("\nInsira a sua password: ");
				scanf("%s", user.password);
				fflush(stdin),
				
				nBytes=strlen(user.password)+1;
				sendto(clientSocket,user.password,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);			
				fflush(stdout);
				printf("\nA confirmar...\n\n");
				fflush(stdout);
				
				nBytes = recvfrom(clientSocket,confirm,50,0,NULL, NULL);
				confirm[nBytes]='\0';
				fflush(stdin);
				
				if(strcmp(confirm, "NEW") == 0){
					printf("ID está pendente de aprovação pelo Gestor de Sistema, tente de novo mais tarde.\n");
					sleep(2);
					goto loginmenu;
				}

				if(strcmp(confirm, "NEW") != 0 && strcmp(confirm, "0") != 0){
					strcpy(user.username, confirm);
					printf("Login com sucesso, a entrar no programa...");
					fflush(stdout);
					sleep(2);
					break;
				}
				
				printf("Credenciais erradas, a voltar ao menu anterior.\n");
				fflush(stdout);
				sleep(2);
				goto loginmenu;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 2 - REGISTO---------------------------------------------------------------------------------------------------------------------------------------- 
		if(menu1[0] == '2'){
			nBytes = strlen("2")+1;
			sendto(clientSocket, "2", nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
			fflush(stdout);
			
			system("clear");
			header();
			
			do
			{
				do
				{
					printf("\n\nInsira o seu ID de profissional (APS_xxxx): ");
					scanf("%s", user.id);
					fflush(stdin);
					if(strlen(user.id) != 8){
						printf("\nID inserido tem um número de caracteres incorreto, tente novamente!");
					}
				}while(strlen(user.id)!=8);
				
				if(strncmp(user.id,"APS_",4)!=0)
				{
					printf("\nID inserido não corresponde ao exemplo, tente novamente!\n");
					fflush(stdout);
				}
			}while(strncmp(user.id,"APS_",4)!=0);
			
			do{
				
				printf("\nInsira o seu username (máx 50 char): ");
				scanf("%s", user.username);
				fflush(stdin);
				
				if(strlen(user.username)>50)
				{
					printf("\nUsername comprido demais, tente novamente!\n.");
					fflush(stdout);
				}
			}while(strlen(user.username)>50);
			
			do
			{
				printf("\nInsira a sua password (máx 50 char): ");
				scanf("%s", user.password);
				fflush(stdin);
				
				if(strlen(user.password)>50)
				{
					printf("\nPassword comprida demais, tente novamente!\n.");
					fflush(stdout);
				}
		    }while(strlen(user.password)>50);

			sprintf(registo, "%s\\%s\\%s", user.id, user.username, user.password);

			nBytes = strlen(registo)+1;
			sendto(clientSocket, registo, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
			fflush(stdout);
			
			char confirm[10];
			
			nBytes = recvfrom(clientSocket, confirm, 10, 0, (struct sockaddr *)&serverAddr, &addr_size);
			fflush(stdin);
			
			if(strcmp(confirm, "1") == 0){
				printf("\nUtilizador registado com sucesso, poderá efetuar login quando for aprovado.\n\n");
				sleep(2);
				goto loginmenu;
			}
			
			if(strcmp(confirm, "NEW") == 0){
				printf("\nUtilizador pendente aprovação pelo Gestor, impossivel fazer login de momento.\n\n");
				sleep(2);
				goto loginmenu;
			}
			
			if(strcmp(confirm, "USE") == 0){
				printf("\nUtilizador já em uso, impossivel fazer login de momento.\n\n");
				sleep(2);
				goto loginmenu;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 3 - EMERGÊNCIA - TBD-------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '3'){
			//MODO DE EMERGÊNCIA
			nBytes = strlen("4")+1;
			sendto(clientSocket,"4",nBytes,0,(struct sockaddr *)&serverAddr,addr_size);
			fflush(stdout);

			system("clear");
			header();

			char localEmergencia[40];

			while (getchar()!='\n'); // para elimniar o \n do buffer
			fflush(stdin);

			while(1){
				printf("\nInsira o local da ocorrência (máx 40 char): ");
				fgets(localEmergencia,40,stdin);
				fflush(stdin);

				localEmergencia[strcspn(localEmergencia, "\n")] = 0; //retira o "\n" do fim da string

				if(strlen(localEmergencia) > 0 && strlen(localEmergencia) < 41){
					break;
				}
			}
			nBytes = strlen(localEmergencia) +1;
			sendto(clientSocket,localEmergencia, nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

			printf("O seu pedido de emergencia foi enviado!\n");

			sleep(2);
			goto loginmenu;
			
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 4 - SAIR - TBD-------------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '4'){
			fflush(stdout);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373) & Vitor Ferreira (2017248014).");
			
			exit(1);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//---------------------------------------------------------MENU APÓS LOGIN---------------------------------------------------------------------------------
		mainmenu:
		while(1){
			system("clear");
			header();
			printf("\n\nSeja bem vindo, %s", user.username);
			printf("\n\n1 - Registar crime\n");
			printf("2 - Gerir registo\n");
			printf("3 - Sair da aplicação\n");
			printf("4 - Help\n");
			printf("5 - chat\n");
			printf("\nInsira a sua escolha: ");
			scanf("%s", menu2);
			fflush(stdin);
			
			if(menu2[0] == '1' || menu2[0] == '2' || menu2[0] == '3' || menu2[0] == '4' || menu2[0]=='5'){
				break;
			}
		}
		
		//OPÇÃO 1 - REGISTAR CRIME - UPDATE COM ANONIMATO---------------------------------------------------------------------------------------------------------
		if(menu2[0] == '1'){
			nBytes = strlen("7")+1;
			sendto(clientSocket, "7", nBytes, 0, (struct sockaddr*)&serverAddr, addr_size);
			fflush(stdout);
			
			returnmenu = 'a';
			
			system("clear");
			header();
			printf("\n\nFormulário de registo de crime:\n");

			while(1){
				printf("\nDeseja fazer um registo anónimo? (0 - Nao | 1 - Sim): ");
				scanf("%d", &anon);
				fflush(stdin);
				
				if(anon >= 0 && anon <= 1){
					break;
				}
			}

			while(1){
				printf("\nInsira o dia (1-31): ");
				scanf("%d", &dia);
				fflush(stdin);
				
				if(dia > 0 && dia < 32){
					break;
				}
			}
			
			while(1){
				printf("\nInsira o mês (1-12): ");
				scanf("%d", &mes);
				
				if(mes > 0 && mes < 13){
					break;
				}
			}
			
			while(1){
				printf("\nInsira o ano (2000-2150): ");
				scanf("%d", &ano);
				while(getchar() != '\n'); //para eliminar o \n do buffer

				if(ano > 1999 && ano < 2151){
					break;
				}
			}
			
			sprintf(info.data, "%d-%d-%d", dia, mes, ano);
			
			printf("Insira a hora da ocorrência (hh:mm): ");
			scanf("%s", info.hora);
			while(getchar() != '\n'); //para eliminar o \n do buffer
			fflush(stdin);
			
			while(1){
				printf("Insira o local da ocorrência (máx 40 char): ");
				fgets(info.local,40,stdin);
				fflush(stdin);

				info.local[strcspn(info.local,"\n")] = 0; //retira o "\n" do fim da string

				if(strlen(info.local) > 0 && strlen(info.local) < 41){
					break;
				}
			}
			
			while(1){
				printf("Insira o tipo de agressão (máx 40 char): ");
				fgets(info.crime,40,stdin);
				fflush(stdin);
				
				info.crime[strcspn(info.crime,"\n")] = 0; //retira o "\n" do fim da string

				if(strlen(info.crime) > 0 && strlen(info.crime) < 41){
					break;
				}
			}
			
			while(1){

				if(anon == 0){
					printf("Insira o nome da vitima (máx 40 char): ");
					fgets(info.nome,40, stdin);
					fflush(stdin);
				
					info.nome[strcspn(info.nome,"\n")] = 0;//retira o "\n" do fim da string

					if(strlen(info.nome) > 0 && strlen(info.nome) < 41){
						break;
					}
				}else{
					sprintf(info.nome,"anonimo");
					break;
				}
			}
			
			sprintf(crime, "\"%s\";\"%s\";\"%s\";\"%s\";\"%s\"", info.data, info.hora, info.local, info.crime, info.nome);
			
			nBytes = strlen(crime) + 1;
			sendto(clientSocket, crime, nBytes,0,(struct sockaddr *)&serverAddr,addr_size);
			printf("Obrigado pela sua colaboracao!\n"); 
			
			while(returnmenu != 's' && returnmenu != 'S'){
				printf("\nPressione S para voltar ao menu anterior: ");
				scanf(" %c", &returnmenu);
				fflush(stdin);
			}
			goto mainmenu;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 2 - GERIR REGISTO - WORKING------------------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '2'){
			while(1){
				system("clear");
				header();
				printf("\n\n1 - Alterar registo\n");
				printf("2 - Apagar registo\n");
				printf("3 - Voltar ao menu anterior\n\n");
				printf("Insira a sua escolha: ");
				scanf("%s", menuregisto);
				fflush(stdin);
				
				if(menuregisto[0] == '1' || menuregisto[0] == '2' || menuregisto[0] == '3'){
					break;
				}
			}
			
			//OPÇÃO 1 - ALTERAR REGISTO - WORKING
			if(menuregisto[0] == '1'){
				nBytes = strlen("8")+1;
				sendto(clientSocket, "8", nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
				fflush(stdout);
				
				system("clear");
				header();
				while(1){
					printf("\n\nInsira o seu novo nome (0 para manter, máx 100 char): ");
					scanf("%s", edit.username);
					fflush(stdin);
					if(edit.username[0] == '0'){
						strcpy(edit.username, user.username);
					}
					
					if(strlen(edit.username) > 0 && strlen(edit.username) <= 100){
						break;
					}
				}
				
				while(1){
					printf("Insira a sua nova password (0 para manter): ");
					scanf("%s", edit.password);
					fflush(stdin);
					if(edit.password[0] == '0'){
						strcpy(edit.password, user.password);
					}
					
					if(strlen(edit.password) > 0 && strlen(edit.password) <= 100){
						break;
					}
				}
				
				sprintf(registo, "%s\\%s\\%s", user.id, edit.username, edit.password);
				nBytes = strlen(registo)+1;
				sendto(clientSocket, registo, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
				fflush(stdout);
				printf("Registo alterado com sucesso!\n");
				
				returnmenu = 'a';
				while(returnmenu != 's' && returnmenu != 'S'){
					printf("\nPressione S para sair: ");
					scanf(" %c", &returnmenu);
					fflush(stdin);
				}
				goto mainmenu;
			}
			
			//OPÇÃO 2 - APAGAR REGISTO - WORKING
			if(menuregisto[0] == '2'){
				menuapagar = 'a';
				while(menuapagar != 'y' && menuapagar != 'Y' && menuapagar != 'n' && menuapagar != 'N'){
					system("clear");
					header();
					printf("\n\nDeseja mesmo apagar o seu registo (Y/N): ");
					scanf(" %c", &menuapagar);
					fflush(stdin);
				}
				
				//OPÇÃO 1 - YES - WORKING
				if(menuapagar == 'y' || menuapagar == 'Y'){
					nBytes = strlen("9")+1;
					sendto(clientSocket, "9", nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
					fflush(stdout);
					
					sprintf(registo, "%s\\%s\\%s", user.id, user.username, user.password);
					nBytes = strlen(registo)+1;
					sendto(clientSocket, registo, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);
					fflush(stdout);
					
					system("clear");
					header();
					printf("\n\nRegisto apagado com sucesso!\n");
					returnmenu = 'a';
					while(returnmenu != 's' && returnmenu != 'S'){
						printf("\nPressione S para voltar ao menu principal: ");
						scanf(" %c", &returnmenu);
						fflush(stdin);
					}
					goto mainmenu;
				}
				
				//OPÇÃO 2 - NO - WORKING
				if(menuapagar == 'n' || menuapagar == 'N'){
					system("clear");
					header();
					printf("\n\nOperação cancelada!\n");

					returnmenu = 'a';
					while(returnmenu != 's' && returnmenu != 'S'){
						printf("\nPressione S para voltar ao menu principal: ");
						scanf(" %c", &returnmenu);
						fflush(stdin);
					}
					goto mainmenu;
				}
			}
			
			//OPÇÃO 3 - VOLTAR AO MENU ANTERIOR - WORKING
			if(menuregisto[0] == '3'){
				goto mainmenu;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 3 - SAIR DO PROGRAMA - WORKING
		if(menu2[0] == '3'){
			fflush(stdout);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373) & Vitor Ferreira (2017248014).");
			
			exit(1);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 4 - HELP
		if(menu2[0] == '4'){
			FILE *fhelp;
			
			fhelp = fopen("HELP Files/HELP_APS.txt", "r");
			system("clear");
			header();
			printf("\n");
			fflush(stdin);
			
			while(fgets(help, sizeof(help), fhelp)){
				printf("%s", help);
			}
			fclose(fhelp);
			
			returnmenu = 'a';
			while(returnmenu != 's' && returnmenu != 'S'){
				printf("\n\nPressione S para voltar ao menu principal: ");
				scanf(" %c", &returnmenu);
			}
			goto mainmenu;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------
		// OPÇÃO 5 - CHAT
		if (menu2[0]=='5'){
			/**
			 * Estabelecer ligação com socket UDP.
			 * 1 socket udp para receber outro para enviar;
			 * Aguardar ligação do lado AAS
			 * Enviar e esperar receber
			 * Ter sinalização para acabar o chat tipo Enter "EXIT" para sair
			 */
			/******************************************
			 **************SOCK_RECEBER**************** 
			 ******************************************/
			int udpChatR;
			int nBytesChatR;
			struct sockaddr_in serverAddr_chatR, clientAddr_chatR;
			struct sockaddr_storage serverStorage_chatR;
			socklen_t addr_size_chatR, clientAddr_size_chatR;
			udpChatR = socket(PF_INET,SOCK_DGRAM,0);
			serverAddr_chatR.sin_family = AF_INET;
			serverAddr_chatR.sin_port = htons(8004);
			serverAddr_chatR.sin_addr.s_addr = htonl(INADDR_ANY);
			memset(serverAddr_chatR.sin_zero, '\0', sizeof(serverAddr_chatR.sin_zero));
			bind (udpChatR, (struct sockaddr *) &serverAddr_chatR, sizeof(serverAddr_chatR));
			addr_size_chatR = sizeof(serverStorage_chatR);
			/*****************************************/

			/******************************************
			 **************SOCK_ENVIAR***************** 
			 ******************************************/
			int udpChatE;
			int nBytesChatE;
			struct sockaddr_in serverAddr_chatE;
			socklen_t addr_size_chatE;
			udpChatE = socket(PF_INET,SOCK_DGRAM,0);
			serverAddr_chatE.sin_family = AF_INET;
			serverAddr_chatE.sin_port = htons(8005);
			serverAddr_chatE.sin_addr.s_addr = htonl(INADDR_ANY);
			memset(serverAddr_chatE.sin_zero, '\0', sizeof(serverAddr_chatE.sin_zero));
			addr_size_chatE = sizeof(serverAddr_chatE);
			//INICIO DA DEFINIÇÃO DO SERVIDOR
			//int udpSocket, nBytes;
			//, nSize;
			printf("\nPara sair, digite EXIT.\n");

			//clientAddr;
			//struct sockaddr_storage serverStorage_chatE;

			/******************************************/
			char receber[100], enviar[100], verificar[100], APS[105];
			int filho_pid;
			do{
				if ((filho_pid=fork())==0){
					// Receber
					fflush(stdin);
				if ((nBytesChatR=recvfrom(udpChatR,receber, sizeof(receber),0,(struct sockaddr *)&serverStorage_chatR, &addr_size_chatR))<0)
					perror("nBysteChatR");
				receber[nBytesChatR]='\0';
				if (strcmp(verificar, receber)!=0 && strcmp(verificar, "\n")!=0)
					printf("%s", receber);
				strcpy(verificar, receber);
				}else{
					// Enviar
					//fflush(stdout);
					fgets(enviar, sizeof(enviar),stdin);
					fflush(stdout);
					strcpy(APS, "APS: ");
					strcat(APS, enviar);
					if (strcmp(enviar, "EXIT\n")==0){
						kill(filho_pid, SIGKILL);
						//break;
						goto mainmenu;
					}
					sendto(udpChatE, APS, strlen(APS),0,(struct sockaddr *)&serverAddr_chatE, addr_size_chatE);
				}
					
			}while (strcmp(enviar, "EXIT\n")!=0);
			wait(NULL);
		}
	}
	return 0;
}

//-------------------------------------------------------------------Implementação das funções gerais------------------------------------------------------
void header(void){
	printf("===============APS - Aplicação para o Profissional de Saúde===============");
	return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------