//APLICAÇÃO PARA GESTOR DE SISTEMA (AGS)

//SOCKET USADO: TPC (WRITE, READ)

//------------------------------------------------------------------Includes--------------------------------------------------------------------------------
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------Defines--------------------------------------------------------------------------------
#define BUF_SIZE 5000
//---------------------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------Struct com atributos do utilizador------------------------------------------------------------------
typedef struct Utilizador{
	char id[10];
	char username[100];
	char password[100];
} Utilizador;
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------Declaração de funções gerais--------------------------------------------------------------
void erro(char *msg);
void header(void);
//---------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------Função Main-------------------------------------------------------------------------------
int main(){


	//-----------------------------------------------------------DEFINIÇÃO DO SERVIDOR--------------------------------------------------------------------
	char endServer[100] = "127.0.0.1";
	int fd, nread;
	//int client;
	struct sockaddr_in addr;
	//client_addr;
	//int client_addr_size;
	struct hostent *hostPtr;

	if ((hostPtr = gethostbyname(endServer)) == 0) 
	{
		printf("Couldn t get host address.\n");
		exit(-1);
	}

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons(9000);

	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	erro("socket");
	if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	erro("Connect");
	//---------------------------------------------------------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------VARIÁVEIS DE MENU--------------------------------------------------------------------------------
	char menu1[1];
	char menu2[1];
	char returnmenu;
	//---------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------VARIÁVEIS DE CREDENCIAIS---------------------------------------------------------------------------
	Utilizador user;
	char registo[250];
	char aps_manage[250][250];
	char aas_manage[250][250];
	char id_accept[100];
	//---------------------------------------------------------------------------------------------------------------------------------------------------------


	//char mensagem[100];
	char help[5000];


	//CICLO WHILE PARA CORRER PROGRAMA SEM FECHAR
	while(1){
		//-----------------------------------------------------------------MENU DE LOGIN----------------------------------------------------------------------
		loginmenu:
		while(1){
			system("clear");
			header();
			printf("\n\n1 - Login\n");
			printf("2 - Registo\n");
			printf("3 - Sair\n");
			printf("\nInsira a sua escolha: ");
			scanf("%s", menu1);
			fflush(stdin);
			
			if(menu1[0] == '1' || menu1[0] == '2' || menu1[0] == '3'){
				break;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------
		

		//OPÇÃO 1 - LOGIN-------------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '1'){
			write(fd, "1", sizeof("1"));
			fflush(stdout);
			
			char confirm[50];
			
			while(1){
				system("clear");
				header();
				printf("\n\nInsira o seu ID de profissional (AGS_xxxx): ");
				scanf("%s", user.id);
				fflush(stdin);
				write(fd, user.id, strlen(user.id));
				fflush(stdout);

				printf("Insira a sua password: ");
				scanf("%s", user.password);
				fflush(stdin);
				
				write(fd, user.password, strlen(user.password));
				fflush(stdout);
				printf("\nA confirmar...\n\n");
				fflush(stdout);

				nread=read(fd, confirm, BUF_SIZE-1); //ler os crimes
				confirm[nread] = '\0'; 
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


		//OPCAO 2 - REGISTO----------------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '2'){
			write(fd, "2", sizeof("2"));
			fflush(stdout);
			
			system("clear");
			header();
			do
			{
				do
				{
					printf("\n\nInsira o seu ID de profissional (AGS_xxxx): ");
					scanf("%s", user.id);
					fflush(stdin);
					if(strlen(user.id) != 8){
						printf("\nID inserido tem um número de caracteres incorreto, tente novamente!");
					}
				}while(strlen(user.id)!=8);
				
				if(strncmp(user.id,"AGS_",4)!=0)
				{
					printf("ID inserido não corresponde ao exemplo, tente novamente!\n");
					fflush(stdout);
				}
			}while(strncmp(user.id,"AGS_",4)!=0);
			
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

			write(fd, registo, strlen(registo));
			fflush(stdout);
			
			char confirm[10];
			
			nread = read(fd, confirm, BUF_SIZE-1);
			confirm[nread] = '\0';
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


		//OPÇÃO 3 - SAIR - WORKING---------------------------------------------------------------------------------------------------------------------------------
		if(menu1[0] == '3'){
			write(fd, "3", sizeof("3"));
			fflush(stdout);
			
			close(fd);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373), Luis Gaspar (2018274645), Telmo Cunha (2018308321), Vitor Ferreira (2017248014).");
			
			exit(1);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//-------------------------------------------------------------MENU APÓS LOGIN-----------------------------------------------------------------------------
		mainmenu:
		while(1){
			system("clear");
			header();
			printf("\n\nSeja bem vindo, %s", user.username);
			printf("\n\n1 - Autorizar registos\n");
			printf("2 - Editar registos\n");
			printf("3 - Apagar registos\n");
			printf("4 - Sair da aplicação\n");
			printf("5 - Help\n");
			printf("\nInsira a sua escolha: ");
			scanf("%s", menu2);
			fflush(stdin);
		
			if(menu2[0] == '1' || menu2[0] == '2' || menu2[0] == '3' || menu2[0] == '4' || menu2[0] == '5'){
				break;
			}
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------

		//OPÇÃO 1 - AUTORIZAR NOVOS REGISTOS - WORKING-------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '1'){
			write(fd, "7", sizeof("7"));
			fflush(stdout);
			
			id_accept[0] = 'a';
			int newaps_count = 0;
			int newaas_count = 0;
			
			system("clear");
			header();
			printf("\n\nLista de novos registos de saúde por aceitar: \n\n");
			while(1){
				nread=read(fd, aps_manage[newaps_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aps_manage[newaps_count], "Done") == 0){
					break;
				}
				aps_manage[newaps_count][nread] = '\0';
				printf("%d - %s\n", newaps_count+1, aps_manage[newaps_count]);
				newaps_count++;
			}
			
			printf("\nLista de novos registos de segurança por aceitar: \n\n");
			while(1){
				nread = read (fd, aas_manage[newaas_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aas_manage[newaas_count], "Done") == 0){
					break;
				}
				aas_manage[newaas_count][nread] = '\0';
				printf("%d - %s\n", newaas_count+1, aas_manage[newaas_count]);
				newaas_count++;
			}
			
			printf("\nSelecione um ID para aceitar (S para sair): ");
			scanf("%s", id_accept);
			fflush(stdin);
			
			while(id_accept[0] != 's' && id_accept[0] != 'S'){
				write(fd, id_accept, sizeof(id_accept));
				fflush(stdout);
				printf("\nInsira outro ID para aceitar (S para sair): ");
				scanf("%s", id_accept);
				fflush(stdin);
			}
			write(fd, id_accept, sizeof(id_accept));
			fflush(stdout);
			goto mainmenu;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------

		//OPÇÃO 2 - EDITAR REGISTOS--------------------------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '2'){
			write(fd, "8", sizeof("8"));
			fflush(stdout);
			
			id_accept[0] = 'a';
			int editaps_count = 0;
			int editaas_count = 0;
			
			system("clear");
			header();
			printf("\n\nLista de registos de saúde existentes: \n\n");
			while(1){
				nread = read(fd, aps_manage[editaps_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aps_manage[editaps_count], "Done") == 0){
					break;
				}
				aps_manage[editaps_count][nread] = '\0';
				printf("%d - %s\n", editaps_count+1, aps_manage[editaps_count]);
				editaps_count++;
			}
			
			printf("\nLista de registos de segurança existentes: \n\n");
			while(1){
				nread = read(fd, aas_manage[editaas_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aas_manage[editaas_count], "Done") == 0){
					break;
				}
				aas_manage[editaas_count][nread] = '\0';
				printf("%d - %s\n", editaas_count+1, aas_manage[editaas_count]);
				editaas_count++;
			}
			
			printf("\nSelecione o ID de um utilizador para editar (S para sair): ");
			scanf("%s", id_accept);
			fflush(stdin);
			
			const char separador[2] = "\\";
			char *id_tok, *user_tok, *pw_tok;
			char aux[250];
			
			if(strncmp(id_accept, "APS", 3) == 0){
				for(int i = 0; i < editaps_count; i++){
					if(strncmp(aps_manage[i], id_accept, 8) == 0){
						strcpy(aux, aps_manage[i]);
						id_tok = strtok(aux, separador);
						user_tok = strtok(NULL, separador);
						pw_tok = strtok(NULL, separador);
						break;
					}
				}
			}
			
			if(strncmp(id_accept, "AAS", 3) == 0){
				for(int i = 0; i < editaas_count; i++){
					if(strncmp(aas_manage[i], id_accept, 8) == 0){
						strcpy(aux, aas_manage[i]);
						id_tok = strtok(aux, separador);
						user_tok = strtok(NULL, separador);
						pw_tok = strtok(NULL, separador);
						break;
					}
				}
			}
			
			Utilizador edit;
			system("clear");
			header();
			printf("\n\nEdição de registo\n\n");
			printf("Dados atuais:\nID - %s\nUsername - %s\nPassword - %s\n\n", id_tok, user_tok, pw_tok);
			
			while(1){
				printf("Insira o novo username do utilizador (0 para manter, máx 100 char): ");
				scanf("%s", edit.username);
				fflush(stdin);
				if(edit.username[0] == '0'){
					strcpy(edit.username, user_tok);
				}
				if(strlen(edit.username) > 0 && strlen(edit.username) <= 100){
					break;
				}
			}
			
			while(1){
				printf("Insira a nova password do utilizador (0 para manter, máx 100 char): ");
				scanf("%s", edit.password);
				fflush(stdin);
				if(edit.password[0] == '0'){
					strcpy(edit.password, pw_tok);
				}
				if(strlen(edit.password) > 0 && strlen(edit.password) <= 100){
					break;
				}
			}
			
			sprintf(registo, "%s\\%s\\%s", id_tok, edit.username, edit.password);
			write(fd, registo, strlen(registo));
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
		//---------------------------------------------------------------------------------------------------------------------------------------------------------

		//OPÇÃO 3 - APAGAR REGISTOS--------------------------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '3'){
			write(fd, "9", sizeof("9"));
			fflush(stdout);
			
			id_accept[0] = 'a';
			int delaps_count = 0;
			int delaas_count = 0;
			
			system("clear");
			header();
			printf("\n\nLista de registos de saúde existentes: \n\n");
			while(1){
				nread = read(fd, aps_manage[delaps_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aps_manage[delaps_count], "Done") == 0){
					break;
				}
				aps_manage[delaps_count][nread] = '\0';
				printf("%d - %s\n", delaps_count+1, aps_manage[delaps_count]);
				delaps_count++;
			}
			
			printf("\nLista de registos de segurança existentes: \n\n");
			while(1){
				nread = read(fd, aas_manage[delaas_count], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(aas_manage[delaas_count], "Done") == 0){
					break;
				}
				aas_manage[delaas_count][nread] = '\0';
				printf("%d - %s\n", delaas_count+1, aas_manage[delaas_count]);
				delaas_count++;
			}
			
			printf("\nSelecione um ID para apagar (S para sair): ");
			scanf("%s", id_accept);
			fflush(stdin);
			
			write(fd, id_accept, strlen(id_accept));
			fflush(stdout);
			
			printf("Registo apagado com sucesso!\n");
			
			returnmenu = 'a';
			while(returnmenu != 's' && returnmenu != 'S'){
				printf("\nPressione S para sair: ");
				scanf(" %c", &returnmenu);
				fflush(stdin);
			}
			goto mainmenu;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------

		//OPÇÃO 4 - SAIR DO PROGRAMA-------------------------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '4'){
			write(fd, "3", sizeof("3"));
			fflush(stdout);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373), Luis Gaspar (2018274645), Telmo Cunha (2018308321), Vitor Ferreira (2017248014).");
			
			close(fd);
			exit(1);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------------


		//OPÇÃO 5 - HELP-------------------------------------------------------------------------------------------------------------------------------------------
		if(menu2[0] == '5'){
			FILE *fhelp;
			
			fhelp = fopen("HELP Files/HELP_AGS.txt", "r");
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
	}
	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------Implementação das funções gerais------------------------------------------------------
void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}

void header(void){
	printf("===============AGS - Aplicação de Gestor de Sistema===============");
	return;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------