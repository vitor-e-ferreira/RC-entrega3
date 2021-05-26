//APLICAÇÃO PARA AGENTE DE SEGURANÇA (AAS)

//SOCKET USADO: TPC (WRITE, READ)

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

typedef struct Utilizador{
	char id[10];
	char username[100];
	char password[100];
} Utilizador;

#define BUF_SIZE 5000

void erro(char *msg);
void header(void);

int main(){
	//INICIO DA DEFINICAO DO SERVIDOR
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
	addr.sin_port = htons(8000);

	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	erro("socket");
	if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	erro("Connect");
	//FIM DA DEFINIÇÃO DO SERVIDOR

	//VARIÀVEIS DE MENU
	char menu1[1];
	char menu2[1];
	char menuregisto[1];
	char menuapagar;
	char returnmenu;
	
	//VARIAVEIS DE CREDENCIAIS
	Utilizador user;
	Utilizador edit;
	char registo[250];
	
	char help[5000];

	//CICLO WHILE PARA CORRER PROGRAMA SEM FECHAR
	while(1){
		//MENU DE LOGIN/REGISTO
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
		
		//OPCAO 1 = LOGIN
		if(menu1[0] == '1'){
			write(fd, "1", sizeof("1"));
			fflush(stdout);
			
			char confirm[50];
			
			while(1){
				system("clear");
				header();
				printf("\n\nInsira o seu ID de profissional (AAS_xxxx): ");
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
		
		//OPCAO 2 = REGISTO - WORKING
		if(menu1[0] == '2'){
			write(fd, "2", sizeof("2"));
			fflush(stdout);
			
			system("clear");
			header();

			do{
				do{
					printf("\n\nInsira o seu ID de profissional (AAS_xxxx): ");
					scanf("%s", user.id);
					fflush(stdin);
					if(strlen(user.id) != 8){
						printf("\nID inserido tem um número de caracteres incorreto, tente novamente!");
					}
			    }while(strlen(user.id)!=8);
			    
				if(strncmp(user.id,"AAS_",4)!=0)
				{
					printf("\nID inserido não corresponde ao exemplo, tente novamente!\n");
				}
		    }while(strncmp(user.id,"AAS_",4)!=0);
			
			
			do{
				printf("\nInsira o seu username (máx 50 char): ");
				scanf("%s", user.username);
				fflush(stdin);
				
				if(strlen(user.username)>50){
					printf("\nUsername comprido demais, tente novamente!\n");
				}
			}while(strlen(user.username)>50);
			
			do
			{
				printf("\nInsira a sua password (máx 50 char): ");
				scanf("%s", user.password);
				fflush(stdin);
				
				if(strlen(user.password)>50){
					printf("\nPassword comprida demais, tente novamente!\n");
				}
		    }while(strlen(user.password)>50);

			sprintf(registo, "%s\\%s\\%s", user.id, user.username, user.password);

			write(fd, registo, strlen(registo));
			fflush(stdout);
			
			char confirm[10];
			
			nread = read(fd, confirm, BUF_SIZE-1);
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
		
		//OPÇÃO 3 - SAIR - WORKING
		if(menu1[0] == '3'){
			write(fd, "3", sizeof("3"));
			fflush(stdout);
			
			close(fd);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373), Luis Gaspar (2018274645), Telmo Cunha (2018308321), Vitor Ferreira (2017248014).");
			
			exit(1);
		}
		
		//MENU APÓS LOGIN
		mainmenu:
		while(1){
			system("clear");
			header();
			printf("\n\nSeja bem vindo, %s", user.username);
			printf("\n\n1 - Ver crimes\n");
			printf("2 - Gerir registo\n");
			printf("3 - Sair da aplicação\n");
			printf("4 - Help\n");
			printf("\nInsira a sua escolha: ");
			scanf("%s", menu2);
			fflush(stdin);
			
			if(menu2[0] == '1' || menu2[0] == '2' || menu2[0] == '3' || menu2[0] == '4' || menu2[0] == '5'){
				break;
			}
		}
		
		//OPCAO 1 - VER CRIMES - MODIFICAR PARA LINHA BY LINHA
		if(menu2[0] == '1'){
			write(fd, "7", sizeof("7"));
			fflush(stdout);
			
			char crimes[1000][200];
			
			int num_crimes = 0;
			char crime_id[5];
			char filter_data[50];
			char filter_local[50];
			char filter_nome[50];
			
			//int dia, mes, ano;
			
			returnmenu = 'a';
			system("clear");
			while(1){
				nread=read(fd, crimes[num_crimes], BUF_SIZE-1);
				fflush(stdin);
				if(strcmp(crimes[num_crimes], "Done") == 0){
					break;
				}
				crimes[num_crimes][nread] = '\0';
				num_crimes++;
			}
			
			while(1){
				header();
				printf("\n\nOs crimes registados são: \n\n");
				for(int i = 0; i < num_crimes; i++){
					printf("%d - %s\n", i+1, crimes[i]);
					fflush(stdout);
					
				}
				printf("\n\n1 - Marcar como assistido\n");
				printf("2 - Filtrar por data\n");
				printf("3 - Filtrar por local\n");
				printf("4 - Filtrar por nome\n");
				printf("5 - Voltar ao menu anterior\n\n");
				printf("Insira a sua escolha: ");
				scanf(" %c", &returnmenu);
				
				//Send ID
				if(returnmenu == '1'){
					printf("\nInsira o ID do crime: ");
					scanf("%s", crime_id);
					
					/*crime_id[0]=crime_id[0]-1;
					write(fd, crime_id, strlen(crime_id));
				    fflush(stdout);
				    while(1){
						nread=read(fd, crimes[num_crimes], BUF_SIZE-1);
						fflush(stdin);
						if(strcmp(crimes[num_crimes], "Done") == 0){
							break;
						}
						crimes[num_crimes][nread] = '\0';
						num_crimes++;
			       }
					printf("\nCrime inserido como resolvido! Bom trabalho Sr. Agente %s\n", );
					printf("\n\nOs crimes registados são: \n\n");
					for(int i = 0; i < num_crimes; i++)
					{
						printf("%d - %s\n", i+1, crimes[i]);
						fflush(stdout);
					
					}*/	
				}
				
				//Pesquisa por data
				if(returnmenu == '2'){
					system("clear");
				    header();
				    
				    printf("\n\nLista total de crimes:\n");
				    for(int i = 0; i < num_crimes; i++){
						printf("%d - %s\n", i+1, crimes[i]);
						fflush(stdout);
					}
					/*
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
						
						if(ano > 1999 && ano < 2151){
							break;
						}
					}
					*/
					//sprintf(filter_data, "%d-%d-%d", dia, mes, ano);
					
					
					printf("\nEsreva a data que deseja: ");
					fflush(stdout);
					scanf("%s", filter_data);
					
					printf("\n\nCrimes encontrados:\n");
					for(int i = 0; i < num_crimes; i++)
					{
						char lim[2]=";";
						char auxiliar[20];
						char *word;
						char aux_string[1000][200];
						
						strcpy(aux_string[i],crimes[i]);
						sprintf(auxiliar,"\"%s""\"", filter_data);
						
						word=strtok(aux_string[i],lim);//fica com a primeira palavra->"00-00-0000"
						if(strcmp(word,auxiliar)==0)
						{
							printf("%d - %s\n", i+1, crimes[i]);
							fflush(stdout);
						}
				    }
				    returnmenu = 'a';
					while(returnmenu != 's' && returnmenu != 'S'){
						printf("\n\nPressione S para sair: ");
						scanf(" %c", &returnmenu);
						fflush(stdin);
					}
					goto mainmenu;
				}
				
				//Pesquisa por local
				if(returnmenu == '3'){
					system("clear");
				    header();
					
					printf("\n\nLista total de crimes:\n");
					for(int i = 0; i < num_crimes; i++){
					printf("%d - %s\n", i+1, crimes[i]);
					fflush(stdout);
					
					}
					
					do{
					printf("\nInsira o local desejado: ");
					scanf("%s", filter_local);
					fflush(stdin);
					if(strlen(user.username)>40)
						{
							printf("\nLocal comprido demais, tente novamente!\n.");
							fflush(stdout);
						}
				    }while(strlen(filter_local)>40);
				    
				    printf("\n\nCrimes encontrados:\n");
					for(int i = 0; i < num_crimes; i++)
					{
						
						char lim[2]=";";
						char auxiliar[20];
						char *word;
						char aux_string[1000][200];
						
						strcpy(aux_string[i],crimes[i]);
						sprintf(auxiliar,"\"%s""\"", filter_local);
						
						word=strtok(aux_string[i],lim);
						while( word != NULL ) 
						{
							  if(strcmp(word,auxiliar)==0)
								{
									printf("%d - %s\n", i+1, crimes[i]);
									fflush(stdout);
								}
							
							  word = strtok(NULL, lim);
					    }
				    }
				    returnmenu = 'a';
					while(returnmenu != 's' && returnmenu != 'S'){
						printf("\n\nPressione S para sair: ");
						scanf(" %c", &returnmenu);
						fflush(stdin);
					}
					goto mainmenu;
				}
				
				//Pesquisa por nome
				if(returnmenu == '4'){
					
					system("clear");
				    header();
					
					printf("\n\nLista total de crimes:\n");
					for(int i = 0; i < num_crimes; i++){
					printf("%d - %s\n", i+1, crimes[i]);
					fflush(stdout);
					
					}
					
					do{
					printf("\nInsira o Nome desejado: ");
					scanf("%s", filter_nome);
					fflush(stdin);
					if(strlen(user.username)>40)
						{
							printf("\nNome comprido demais, tente novamente!\n.");
							fflush(stdout);
						}
				    }while(strlen(filter_nome)>40);
					
					printf("\n\nCrimes encontrados:\n");
					for(int i = 0; i < num_crimes; i++)
					{
						
						char lim[2]=";";
						char auxiliar[20];
						char *word;
						char aux_string[1000][200];
						
						strcpy(aux_string[i],crimes[i]);
						sprintf(auxiliar,"\"%s""\"", filter_nome);
						
						word=strtok(aux_string[i],lim);
						while( word != NULL ) 
						{
							if(strcmp(word,auxiliar)==0)
								{
									printf("%d - %s\n", i+1, crimes[i]);
									fflush(stdout);
								}
							  word = strtok(NULL, lim);
					    }
				    }
				    returnmenu = 'a';
					while(returnmenu != 's' && returnmenu != 'S'){
						printf("\n\nPressione S para sair: ");
						scanf(" %c", &returnmenu);
						fflush(stdin);
					}
					goto mainmenu;
				}
				
				if(returnmenu == '5'){
					goto mainmenu;
				}
			}
		}
		
		//OPCAO 2 - GERIR REGISTO - WORKING
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
				
				if(menuregisto[0] == '1' || menuregisto[0] == '2' || menuregisto[0] == '3' || menuregisto[0] == '4' || menuregisto[0] == '5'){
					break;
				}
			}
			 
			//OPCAO 1 - ALTERAR REGISTO - WORKING
			if(menuregisto[0] == '1'){
				write(fd, "8", sizeof("8"));
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
				write(fd, registo, strlen(registo));
				fflush(stdout);
				printf("\nRegisto alterado com sucesso!\n");
				
				returnmenu = 'a';
				while(returnmenu != 's' && returnmenu != 'S'){
					printf("\nPressione S para sair: ");
					scanf(" %c", &returnmenu);
					fflush(stdin);
				}
				goto mainmenu;
			}
			
			//OPCAO 2 - APAGAR REGISTO - FIX ID SEND
			if(menuregisto[0] == '2'){
				menuapagar = 'a';
				while(menuapagar != 'y' && menuapagar != 'Y' && menuapagar != 'n' && menuapagar != 'N'){
					system("clear");
					header();
					printf("\n\nDeseja mesmo apagar o seu registo (Y/N): ");
					scanf(" %c", &menuapagar);
					fflush(stdin);
				}
				
				//OPÇÃO 1 - YES
				if(menuapagar == 'y' || menuapagar == 'Y'){
					write(fd, "9", sizeof("9"));
					fflush(stdout);
					sleep(0.5);
					
					sprintf(registo, "%s\\%s\\%s", user.id, user.username, user.password);
					write(fd, registo, strlen(registo));
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
				
				//OPÇÃO 2 - NO
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
			
			//OPCAO 3 - VOLTAR AO MENU ANTERIOR
			if(menuregisto[0] == '3'){
				goto mainmenu;
			}
		}
		
		//OPCAO 3 - SAIR DO PROGRAMA - WORKING
		if(menu2[0] == '3'){
			write(fd, "3", sizeof("3"));
			fflush(stdout);
			
			printf("\nCopyright (c) [2021] João Ruivo (2018289373), Luis Gaspar (2018274645), Telmo Cunha (2018308321), Vitor Ferreira (2017248014).");
			
			close(fd);
			exit(1);
		}
		
		//OPÇÃO 4 - HELP
		if(menu2[0] == '4'){
			FILE *fhelp;
			
			fhelp = fopen("HELP Files/HELP_AAS.txt", "r");
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
	}
	return 0;
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}

void header(void){
	printf("===============AAS - Aplicação para o Agente de Segurança===============");
	return;
}
