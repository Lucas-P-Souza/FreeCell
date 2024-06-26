// Inclus�o das bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

// Defini��o do tamanho de cada espa�o
#define TAM_MESA 8
#define TAM_NAIPE 4
#define TAM_TEMP 4

// Defini��o das cores
#define RED 4
#define GREEN 2
#define WHITE 15
#define BLACK 0

// Movimentos
int moves = 0;

// Defini��o da estrutura das cartas
typedef struct no
{
    short num;
    char naipe;
    struct no *prox;
} tCarta;

// Inicializa��o das pilhas
tCarta *primMonte = NULL;
tCarta *primMesa[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
tCarta *primNaipe[] = {NULL, NULL, NULL, NULL};
tCarta *primTemp[] = {NULL, NULL, NULL, NULL};

// Fun��o para mudar a cor do texto do terminal
void textcolor(int iColor)
{
    HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    bool b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
    bufferInfo.wAttributes &= 0x00F0;
    SetConsoleTextAttribute(hl, bufferInfo.wAttributes |= iColor);

} // end textcolor()

// Fun��o para mudar a cor de fundo do terminal
void backcolor(int iColor)
{
    HANDLE hl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    bool b = GetConsoleScreenBufferInfo(hl, &bufferInfo);
    bufferInfo.wAttributes &= 0x000F;
    SetConsoleTextAttribute(hl, bufferInfo.wAttributes |= (iColor << 4));

} // end backcolor()

// Fun��o para exibir uma mensagem de erro
void mensagem(char *textIn)
{
    system("cls");
    printf("%s\n\n", textIn);
    system("pause");

} // end mensagem()

// Fun��o para salvar a pontua��o
void save(int scoreIn)
{
    FILE *arq;

    // Abre o arquivo
    arq = fopen("score_record", "w");

    if (arq != NULL)
    {
        // Salva o valor
        fprintf(arq, "%d\n", scoreIn);
        fclose(arq);
    } // end if(arq != NULL)
    else
    {
        // Caso n�o consiga abrir
        mensagem("Erro no arquivo!");
    } // end else

} // end save()

// Fun��o para carregar a pontua��o
int load_score()
{
    FILE *arq;
    int score = 0;

    // Abre o arquivo
    arq = fopen("score_record", "r");

    if (arq != NULL)
    {
        // Carrega o valor
        fscanf(arq, "%d", &score);
        fclose(arq);
    } // end if(arq != NULL)
    else
    {
        save(score);
    } // end else

    return score;

} // end load_score()

// Fun��o para verificar se uma carta � vermelha
bool cartaVermelha(tCarta *carta)
{
    return ((carta->naipe >= 3) && (carta->naipe <= 4));
} // end cartaVermelha()

// Fun��o para verificar se uma carta � preta
bool cartaPreta(tCarta *carta)
{
    return ((carta->naipe >= 5) && (carta->naipe <= 6));
} // end cartaPreta()

// Fun��o para verificar se as cores de duas cartas s�o diferentes
bool coresDiferentes(tCarta *carta1, tCarta *carta2)
{
    return ((cartaVermelha(carta1) && cartaPreta(carta2)) || (cartaPreta(carta1) && cartaVermelha(carta2)));
} // end coresDiferentes()

// Fun��o para verificar se uma sequ�ncia de cartas est� em ordem
bool sequenciaCorreta(tCarta *carta)
{
    while (carta->prox != NULL)
    {
        // Percorre a pilha a partir da carta passada comparando se s�o de cores diferentes e est�o em ordem decrescente
        if ((coresDiferentes(carta, carta->prox)) && (carta->num - 1 == carta->prox->num))
        {
            carta = carta->prox;
        } // end if((coresDiferentes(carta, carta->prox)) && (carta->num - 1 == carta->prox->num))
        else
        {
            return false;
        } // end else
    }     // end while(carta->prox != NULL)

    return true;

} // end sequenciaCorreta()

// Fun��o para gerar o monte
void gerarBaralho()
{
    tCarta *novo;
    int i, j;

    // Cria��o dos 4 naipes com 13 cartas cada
    for (i = 0; i < 4; i++)
    {
        for (j = 1; j <= 13; j++)
        {
            // Aloca��o de mem�ria para a nova carta
            novo = (tCarta *)malloc(sizeof(tCarta));

            // A nova carta recebe um valor (0 a 13) e um naipe (copas, ouros, paus e espadas)
            novo->num = j;
            novo->naipe = i + 3;

            // A nova carta aponta para a que est� no topo da pilha
            novo->prox = primMonte;

            // O primeiro elemento da pilha passa a ser a nova carta
            primMonte = novo;
        } // end for(j = 1; j <= 13; j++)
    }     // end for(i = 0; i < 4; i++)

} // end gerarBaralho()

// Fun��o para embaralhar o monte de cartas
void embaralhaBaralho()
{
    tCarta *atual, *ant;
    int i, j, sort;

    // Realiza uma randomiza��o quatro vezes maior que o n�mero de cartas
    for (i = 0; i < 208; i++)
    {
        // Sorteia uma posi��o do monte
        sort = rand() % 52;
        ant = NULL;

        atual = primMonte;

        // Encontra a carta nessa posi��o
        for (j = 0; j < sort; j++)
        {
            ant = atual;
            atual = atual->prox;
        } // end for(j = 0; j < sort; j++)

        // Troca a carta de lugar com a primeira do monte
        if (sort > 0)
        {
            ant->prox = atual->prox;
            atual->prox = primMonte;
            primMonte = atual;
        } // end if(sort > 0)
    }     // end for(i = 0; i < 208; i++)

} // end embaralhaBaralho()

// Fun��o para distribuir as cartas na mesa
void distribuirMesa()
{
    tCarta *auxMonte, *auxMesa;
    int i = 0;

    while (primMonte != NULL)
    {
        auxMonte = primMonte;
        primMonte = primMonte->prox;

        // Se for a primeira carta do monte da mesa
        if (primMesa[i] == NULL)
        {
            primMesa[i] = auxMonte;
            primMesa[i]->prox = NULL;
        } // end if(primMesa[i] == NULL)
        else
        {
            auxMesa = primMesa[i];
            primMesa[i] = auxMonte;
            primMesa[i]->prox = auxMesa;
        } // end else

        if (primMonte == NULL)
        {
            break;
        } // end if(primMonte == NULL)

        // Passa para o pr�ximo monte da mesa, se chegar ao final volta ao primeiro e continua assim at� acabar as cartas
        i = (i + 1) % 8;
    } // end while(primMonte != NULL)

} // end distribuirMesa()

void telaInicial()
{
    // Limpa o terminal
    system("cls");

    textcolor(WHITE); // Muda a cor do texto para branca

    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::FREE CELL::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    printf("\nOBJETIVO:\n\n- Preencher, com a menor quantidade de movimentos possiveis, os 4 naipes disponiveis com as cartas em ordem crescente e do mesmo naipe.\n");
    printf("\nREGRAS:\n");
    printf("\n- As cartas das pilhas da mesa que serao movidas estao mais a direita.");
    printf("\n- Para as cartas serem movidas para as pilhas da mesa elas devem ser de uma cor diferente da que esta na base e na sequencia.");
    printf("\n- Apenas o rei (13) pode ser movido para uma das pilhas vazias da mesa.");
    printf("\n- Cada pilha do temporario comporta apenas uma carta.");
    printf("\n- E possivel mover um bloco de cartas entre as pilhas da mesa desde que todas esteja na sequencia e em cores alternadas.");
    printf("\n- As cartas a serem colocadas nas pilhas dos naipes devem ser da menor para a maior e do mesmo naipe.");
    printf("\n\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::");

    printf("\n\n");
    system("pause");

} // end telaInicial()

// Fun��o para imprimir o TEMP, o NAIPE e a MESA
void imprime()
{
    tCarta *atual;
    int i;

    // Limpa o terminal
    system("cls");

    textcolor(WHITE); // Muda a cor do texto para branca

    printf("::::::::::::::::::::::FREE CELL::::::::::::::::::::::\n");

    printf("[TEMP] = ");
    // Percorre as posi��es de TEMP (0 a 3)
    for (i = 0; i < TAM_TEMP; i++)
    {
        if (primTemp[i] != NULL)
        {
            printf("%d-[%02d/", i + 1, primTemp[i]->num);
            // Imprime o texto do naipe com a cor correspondente
            if (cartaVermelha(primTemp[i]))
            {
                textcolor(RED);
            }
            else if (cartaPreta(primTemp[i]))
            {
                textcolor(BLACK);
            }
            printf("%c", primTemp[i]->naipe);
            textcolor(WHITE);
            printf("] ");
        } // end if(primTemp[i] != NULL)
        else
        {
            printf("%d-[    ] ", i + 1);
        } // end else
    }     // end for(i = 0; i < TAM_TEMP; i++)
    printf("\n\n");

    // Percorre as posi��es de NAIPE (0 a 3)
    for (i = 0; i < TAM_NAIPE; i++)
    {
        atual = primNaipe[i];
        printf("[NAIPE %d] = ", i + 1);
        while (atual != NULL)
        {
            printf("[%02d/", atual->num);
            if (cartaVermelha(atual))
            {
                textcolor(RED);
            }
            else if (cartaPreta(atual))
            {
                textcolor(BLACK);
            }
            printf("%c", atual->naipe);
            textcolor(WHITE);
            printf("]");

            atual = atual->prox;
        } // end while(atual != NULL)
        printf("\n");
    } // end for(i = 0; i < TAM_NAIPE; i++)
    printf("\n");

    // Percorre as posi��es de MESA (0 a 7)
    for (i = 0; i < TAM_MESA; i++)
    {
        atual = primMesa[i];
        printf("[MESA %d] = ", i + 1);
        while (atual != NULL)
        {
            printf("[%02d/", atual->num);
            if (cartaVermelha(atual))
            {
                textcolor(RED);
            }
            else if (cartaPreta(atual))
            {
                textcolor(BLACK);
            }
            printf("%c", atual->naipe);
            textcolor(WHITE);
            printf("]");

            atual = atual->prox;
        } // end while(atual != NULL)
        printf("\n");
    } // end for(i = 0; i < TAM_MESA; i++)
    printf("\n");

} // end imprime()

void telaFinal()
{
    int score = 0;

    score = load_score();

    if ((moves < score) || (score == 0))
    {
        save(moves);
    } // end if((moves < score) || (score == 0))

    score = load_score();

    // Limpa o terminal
    system("cls");

    textcolor(WHITE); // Muda a cor do texto para branca

    printf("::::::::::::::::::::::FREE CELL::::::::::::::::::::::\n");
    printf("\n\n\n\n                    VOCE VENCEU!\n                      PARABENS!\n\n             Movimentos realizados: %04d\n\n                    Recorde: %04d\n\n\n\n", moves, score);
    printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::\n\n");

} // telaFinal()

// Fun��o para mover da MESA para o TEMP
void moveMesaTemp()
{
    tCarta *ant, *atual;
    int posMesa, posTemp;

    // Inser��o da posi��o da mesa pelo usu�rio
    printf("Digite a posicao da MESA na qual esta a carta que deseja mover (1 - 8): ");
    scanf("%d", &posMesa);
    posMesa--;

    // Verifica se a posi��o da mesa � v�lida
    if ((posMesa >= 0) && (posMesa <= 7))
    {
        // Verifica se h� carta para mover
        if (primMesa[posMesa] != NULL)
        {
            // Inser��o da posi��o de temp pelo usu�rio
            printf("Digite a posicao de TEMP para qual deseja mover a carta (1 - 4): ");
            scanf("%d", &posTemp);
            posTemp--;

            // Percorre a mesa at� o �ltimo elemento
            ant = NULL;
            atual = primMesa[posMesa];

            while (atual->prox != NULL)
            {
                ant = atual;
                atual = atual->prox;
            } // end while(atual->prox != NULL)

            // Verifica se a posi��o de temp � v�lida
            if ((posTemp >= 0) && (posTemp <= 3))
            {
                // Verifica se a posi��o de temp est� vazia
                if (primTemp[posTemp] == NULL)
                {
                    primTemp[posTemp] = atual;

                    if (ant == NULL)
                    {
                        primMesa[posMesa] = NULL;
                        moves++;
                    } // end if(ant == NULL)
                    else
                    {
                        ant->prox = NULL;
                        moves++;
                    } // end else
                }     // end if(primTemp[posTemp] == NULL)
                else
                {
                    system("cls");
                    printf(":::Posicao %d de TEMP nao disponivel!:::\n\n", posTemp + 1);
                    system("pause");
                } // end else
            }     // end if((posTemp >= 0) && (posTemp <= 3))
            else
            {
                mensagem(":::Posicao de TEMP invalida!:::");
            } // end else
        }     // end if(primMesa[posMesa] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d de MESA:::\n\n", posMesa + 1);
            system("pause");
        } // end else
    }     // end if((posMesa >= 0) && (posMesa <= 7))
    else
    {
        mensagem(":::Posicao da MESA invalida!:::");
    } // end else

} // end moveMesaTemp()

// Fun��o para mover de TEMP para a MESA
void moveTempMesa()
{
    tCarta *atual;
    int posMesa, posTemp;

    // Inser��o da posi��o de temp pelo usu�rio
    printf("Digite a posicao de TEMP na qual esta a carta que deseja mover (1 - 4): ");
    scanf("%d", &posTemp);
    posTemp--;

    // Verifica se a posi��o de temp � v�lida
    if ((posTemp >= 0) && (posTemp <= 3))
    {
        // Verifica se h� uma carta para mover
        if (primTemp[posTemp] != NULL)
        {
            // Inser��o da posi��o da mesa pelo usu�rio
            printf("Digite a posicao da MESA para qual deseja mover a carta (1 - 8): ");
            scanf("%d", &posMesa);
            posMesa--;

            // Verifica se a posi��o da mesa � v�lida
            if ((posMesa >= 0) && (posMesa <= 7))
            {
                // Se o destino da carta na mesa � uma pilha vazia
                if (primMesa[posMesa] == NULL)
                {
                    // Verifica se a carta que est� sendo movida � a 13 (rei)
                    if (primTemp[posTemp]->num == 13)
                    {
                        // Atualiza a pilha de temp e coloca a carta na pilha da mesa
                        primMesa[posMesa] = primTemp[posTemp];
                        primTemp[posTemp] = NULL;
                        moves++;
                    } // end if(primTemp[posTemp]->num == 13)
                    else
                    {
                        mensagem(":::Mesas vazias so aceitam a carta 13 (rei)!:::");
                    } // end else
                }     // end if(primMesa[posMesa] == NULL)
                else
                {
                    // Percorre a mesa at� achar o �ltimo elemento
                    atual = primMesa[posMesa];

                    while (atual->prox != NULL)
                    {
                        atual = atual->prox;
                    } // end while(atual->prox != NULL)

                    // Verifica se a carta que se deseja mover est� na sequ�ncia da mesa
                    if ((coresDiferentes(primTemp[posTemp], atual)) && (atual->num - 1 == primTemp[posTemp]->num))
                    {
                        atual->prox = primTemp[posTemp];
                        primTemp[posTemp] = NULL;
                        moves++;
                    } // end if()
                    else
                    {
                        mensagem(":::A carta a ser movida deve ser de uma cor diferente e na sequencia da que esta no topo do destino!:::");
                    } // end else
                }     // end else
            }         // end if((posMesa >= 0) && (posMesa <= 7))
            else
            {
                mensagem(":::Posicao invalida da MESA!:::");
            } // end else
        }     // end if(primTemp[posTemp] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na posicao %d de TEMP!:::\n\n", posTemp + 1);
            system("pause");
        } // end else
    }     // end if((posTemp >= 0) && (posTemp <= 3))
    else
    {
        mensagem(":::Posicao invalida de TEMP!:::");
    } // end else

} // end moveTempMesa()

// Fun��o para mover da MESA para o NAIPE
void moveMesaNaipe()
{
    tCarta *ant, *atual;
    int posMesa, posNaipe;

    // Inser��o da posi��o da mesa pelo usu�rio
    printf("Digite a posicao da MESA na qual esta a carta que deseja mover (1 - 8): ");
    scanf("%d", &posMesa);
    posMesa--;

    // Verifica se a posi��o da mesa � v�lida
    if ((posMesa >= 0) && (posMesa <= 7))
    {
        // Verifica se h� uma carta para mover
        if (primMesa[posMesa] != NULL)
        {
            // Inser��o da posi��o de naipe pelo usu�rio
            printf("Digite a posicao de NAIPE para qual deseja mover a carta (1 - 4): ");
            scanf("%d", &posNaipe);
            posNaipe--;

            // Percorre a mesa at� achar o �ltimo elemento
            ant = NULL;
            atual = primMesa[posMesa];

            while (atual->prox != NULL)
            {
                ant = atual;
                atual = atual->prox;
            } // end while(atual->prox != NULL)

            // Verifica se a posi��o do naipe � v�lida
            if ((posNaipe >= 0) && (posNaipe <= 3))
            {
                // Verifica se a carta que se deseja mover est� na sequ�ncia do naipe
                if (((atual->num == 1) && (primNaipe[posNaipe] == NULL)) ||                                                                            // Se a pilha estiver vazia a primeira carta deve ser o �s
                    (((primNaipe[posNaipe] != NULL) && (atual->naipe == primNaipe[posNaipe]->naipe) && (atual->num - 1 == primNaipe[posNaipe]->num)))) // Caso contr�rio o naipe deve ser o mesmo e o n�mero o pr�ximo da sequ�ncia
                {
                    // Se a mesa tiver somente um elemento
                    if (ant == NULL)
                    {
                        primMesa[posMesa] = NULL;
                    } // end if(ant == NULL)
                    else
                    {
                        ant->prox = NULL;
                    } // end else

                    // Se o naipe estiver vazio
                    if (primNaipe[posNaipe] == NULL)
                    {
                        // A carta passa a ser a primeira do naipe
                        primNaipe[posNaipe] = atual;
                        moves++;
                    } // end if(primNaipe[posNaipe] == NULL)
                    else
                    {
                        // Move a carta
                        atual->prox = primNaipe[posNaipe];
                        primNaipe[posNaipe] = atual;
                        moves++;
                    } // end else()
                }     // end if()
                else
                {
                    mensagem(":::A carta a ser movida deve ser do mesmo naipe e a proxima da sequencia!:::");
                } // end else
            }     // end if((posNaipe >= 0) && (posNaipe <= 3))
            else
            {
                mensagem(":::Posicao de NAIPE invalida!:::");
            } // end else
        }     // end if(primMesa[posMesa] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d de MESA!:::\n\n", posMesa + 1);
            system("pause");
        } // end else
    }     // end if((posMesa >= 0) && (posMesa <= 7))
    else
    {
        mensagem(":::Posicao da MESA invalida!:::");
    } // end else

} // end moveMesaNaipe()

// Fun��o para mover de NAIPE para o TEMP
void moveNaipeTemp()
{
    int posNaipe, posTemp;

    // Inser��o da posi��o do naipe pelo usu�rio
    printf("Digite a posicao de NAIPE na qual esta a carta que deseja mover (1 - 4): ");
    scanf("%d", &posNaipe);
    posNaipe--;

    // Verifica se a posi��o do naipe � v�lida
    if ((posNaipe >= 0) && (posNaipe <= 3))
    {
        // Verifica se h� carta para mover
        if (primNaipe[posNaipe] != NULL)
        {
            // Inser��o da posi��o de temp pelo usu�rio
            printf("Digite a posicao de TEMP para qual deseja mover a carta (1 - 4): ");
            scanf("%d", &posTemp);
            posTemp--;

            // Verifica se a posi��o de temp � v�lida
            if ((posTemp >= 0) && (posTemp <= 3))
            {
                // Verifica se a posi��o de temp est� vazia
                if (primTemp[posTemp] == NULL)
                {
                    // Move a carta
                    primTemp[posTemp] = primNaipe[posNaipe];
                    primNaipe[posNaipe] = primNaipe[posNaipe]->prox;
                    primTemp[posTemp]->prox = NULL;
                    moves++;
                } // end if(primTemp[posTemp] == NULL)
                else
                {
                    system("cls");
                    printf(":::Posicao %d de TEMP nao disponivel!:::\n\n", posTemp + 1);
                    system("pause");
                } // end else
            }     // end if((posTemp >= 0) && (posTemp <= 3))
            else
            {
                mensagem(":::Posicao de TEMP invalida!:::");
            } // end else
        }     // end if(primNaipe[posNaipe] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d de NAIPE!:::\n\n", posNaipe + 1);
            system("pause");
        } // end else
    }     // end if((posNaipe >= 0) && (posNaipe <= 3))
    else
    {
        mensagem(":::Posicao de NAIPE invalida!:::");
    } // end else

} // end moveNaipeTemp()

// Fun��o para mover do TEMP para o NAIPE
void moveTempNaipe()
{
    int posTemp, posNaipe;

    // Inser��o da posi��o de temp pelo usu�rio
    printf("Digite a posicao da TEMP na qual esta a carta que deseja mover (1 - 4): ");
    scanf("%d", &posTemp);
    posTemp--;

    // Verifica se a posi��o de temp � v�lida
    if ((posTemp >= 0) && (posTemp <= 3))
    {
        // Verifica se h� carta para mover
        if (primTemp[posTemp] != NULL)
        {
            // Inser��o da posi��o de naipe pelo usu�rio
            printf("Digite a posicao de NAIPE para qual deseja mover a carta (1 - 4): ");
            scanf("%d", &posNaipe);
            posNaipe--;

            // Verifica se a posi��o de naipe � v�lida
            if ((posNaipe >= 0) && (posNaipe <= 3))
            {
                // Verifica se a carta que se deseja mover est� na sequ�ncia do naipe
                if (((primTemp[posTemp]->num == 1) && (primNaipe[posNaipe] == NULL)) ||                                                                                        // Se a pilha estiver vazia a primeira carta deve ser o �s
                    (((primNaipe[posNaipe] != NULL) && (primTemp[posTemp]->naipe == primNaipe[posNaipe]->naipe) && (primTemp[posTemp]->num - 1 == primNaipe[posNaipe]->num)))) // Caso contr�rio o naipe deve ser o mesmo e o n�mero o pr�ximo da sequ�ncia
                {
                    // Move a carta
                    primTemp[posTemp]->prox = primNaipe[posNaipe];
                    primNaipe[posNaipe] = primTemp[posTemp];
                    primTemp[posTemp] = NULL;
                    moves++;
                } // end if()
                else
                {
                    mensagem(":::A carta a ser movida deve ser do mesmo naipe e a proxima da sequencia!:::");
                } // end else
            }     // end if((posNaipe >= 0) && (posNaipe <= 3))
            else
            {
                mensagem(":::Posicao de NAIPE invalida!:::");
            } // end else
        }     // end if(primTemp[posTemp] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d de TEMP!:::\n\n", posTemp + 1);
            system("pause");
        } // end else
    }     // end if((posTemp >= 0) && (posTemp <= 3))
    else
    {
        mensagem(":::Posicao de TEMP invalida!:::");
    } // end else

} // end moveTempNaipe()

// Fun��o para mover do NAIPE para a MESA
void moveNaipeMesa()
{
    tCarta *atual;
    int posMesa, posNaipe;

    // Inser��o da posi��o do naipe pelo usu�rio
    printf("Digite a posicao do NAIPE na qual esta a carta que deseja mover (1 - 4): ");
    scanf("%d", &posNaipe);
    posNaipe--;

    // Verifica se a posi��o do naipe � v�lida
    if ((posNaipe >= 0) && (posNaipe <= 3))
    {
        // Verifica se h� carta para mover
        if (primNaipe[posNaipe] != NULL)
        {
            // Inser��o da posi��o da mesa pelo usu�rio
            printf("Digite a posicao da MESA para qual deseja mover a carta (1 - 8): ");
            scanf("%d", &posMesa);
            posMesa--;

            // Verifica se a posi��o da mesa � v�lida
            if ((posMesa >= 0) && (posMesa <= 7))
            {
                // Percorre a mesa at� achar o �ltimo elemento
                atual = primMesa[posMesa];

                // Se a mesa n�o est� vazia
                if (atual != NULL)
                {
                    while (atual->prox != NULL)
                    {
                        atual = atual->prox;
                    } // end while(atual->prox != NULL)
                }     // end if(atual != NULL)

                // Se a mesa estiver vazia
                if (primMesa[posMesa] == NULL)
                {
                    // Verifica se a carta que est� sendo movida � a 13 (rei)
                    if (primNaipe[posNaipe]->num == 13)
                    {
                        // A carta passa a ser a primeira da mesa
                        primMesa[posMesa] = primNaipe[posNaipe];
                        primNaipe[posNaipe] = primNaipe[posNaipe]->prox;
                        primMesa[posMesa]->prox = NULL;
                        moves++;
                    } // end if(primNaipe[posNaipe]->num == 13)
                    else
                    {
                        mensagem(":::Mesas vazias so aceitam a carta 13 (rei)!:::");
                    } // end else
                }     // end if(primMesa[posMesa] == NULL)
                // Verifica se a carta que se deseja mover est� na sequ�ncia da mesa
                else if ((coresDiferentes(primNaipe[posNaipe], atual)) && (atual->num - 1 == primNaipe[posNaipe]->num))
                {
                    // Move a carta
                    atual->prox = primNaipe[posNaipe];
                    primNaipe[posNaipe] = primNaipe[posNaipe]->prox;
                    atual->prox->prox = NULL;
                    moves++;
                } // end else if()
                else
                {
                    mensagem(":::A carta a ser movida deve ser de uma cor diferente e na sequencia da que esta no destino!:::");
                } // end else
            }     // end if((posMesa >= 0) && (posMesa <= 7))
            else
            {
                mensagem(":::Posicao da MESA invalida!:::");
            } // end else
        }     // end if(primNaipe[posNaipe] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d de NAIPE!:::\n\n", posNaipe + 1);
            system("pause");
        } // end else
    }     // end if((posNaipe >= 0) && (posNaipe <= 3))
    else
    {
        mensagem(":::Posicao de NAIPE invalida!:::");
    } // end else

} // end moveNaipeMesa()

// Fun��o para mover de uma MESA para outra
void moveMesaMesa()
{
    tCarta *atualOrigem, *antOrigem, *atualDestino;
    int posOrigemMesa, posDestinoMesa, movimento, quant = 0, indice, i = 0;

    // Inser��o da posi��o da mesa de origem pelo usu�rio
    printf("Digite a posicao da MESA de origem (1 - 8): ");
    scanf("%d", &posOrigemMesa);
    posOrigemMesa--;

    // Verifica se a posi��o da mesa de origem � v�lida
    if ((posOrigemMesa >= 0) && (posOrigemMesa <= 7))
    {
        // Verifica se h� carta para mover
        if (primMesa[posOrigemMesa] != NULL)
        {
            // Inser��o da posi��o da mesa de destino pelo usu�rio
            printf("Digite a posicao da MESA de destino (1 - 8): ");
            scanf("%d", &posDestinoMesa);
            posDestinoMesa--;

            // Verifica se a posi��o da mesa de destino � v�lida
            if ((posDestinoMesa >= 0) && (posDestinoMesa <= 7))
            {
                // Percorre a mesa de destino at� achar o �ltimo elemento
                atualDestino = primMesa[posDestinoMesa];

                // Se a mesa de destino n�o est� vazia
                if (primMesa[posDestinoMesa] != NULL)
                {
                    while (atualDestino->prox != NULL)
                    {
                        atualDestino = atualDestino->prox;
                    } // end while(atualDestino->prox != NULL)
                }     // end if(primMesa[posDestinoMesa] != NULL)

                // Inser��o do tipo de movimento pelo usu�rio
                printf("(1) - Mover uma carta\n(2) - Mover um bloco\nEscolha uma opcao: ");
                scanf("%d", &movimento);

                // Verifica se a op��o � v�lida
                if (movimento == 1)
                {
                    // Percorre a mesa de origem at� achar o �ltimo elemento
                    antOrigem = NULL;
                    atualOrigem = primMesa[posOrigemMesa];

                    while (atualOrigem->prox != NULL)
                    {
                        antOrigem = atualOrigem;
                        atualOrigem = atualOrigem->prox;
                    } // end while(atualOrigem->prox != NULL)

                    // Verifica a mesa de destino est� vazia
                    if (primMesa[posDestinoMesa] == NULL)
                    {
                        // Verifica se a carta que est� sendo movida � a 13 (rei)
                        if (atualOrigem->num == 13)
                        {
                            // A carta passa a ser a primeira da mesa
                            primMesa[posDestinoMesa] = atualOrigem;
                            antOrigem->prox = NULL;
                            moves++;
                        } // end if(atualOrigem->num == 13)
                        else
                        {
                            mensagem(":::Mesas vazias so aceitam a carta 13 (rei)!:::");
                        } // end else
                    }     // end if(primMesa[posDestinoMesa] == NULL)
                    // Verifica se a carta que se deseja mover est� na sequ�ncia da mesa de destino
                    else if ((coresDiferentes(atualOrigem, atualDestino)) && (atualDestino->num - 1 == atualOrigem->num))
                    {
                        // Se h� apenas uma carta na origem
                        if (antOrigem == NULL)
                        {
                            // Move a carta
                            atualDestino->prox = atualOrigem;
                            primMesa[posOrigemMesa] = NULL;
                            moves++;
                        } // end if(antOrigem == NULL)
                        else
                        {
                            // Move a carta
                            atualDestino->prox = atualOrigem;
                            antOrigem->prox = NULL;
                            moves++;
                        } // end else
                    }     // end else if()
                    else
                    {
                        mensagem(":::A carta a ser movida deve ser de uma cor diferente e na sequencia da que esta no destino!:::");
                    } // end else
                }     // end if(movimento == 1)
                else if (movimento == 2)
                {
                    // Percorre a origem verificando a quantidade de elementos dispon�veis
                    atualOrigem = primMesa[posOrigemMesa];

                    while (atualOrigem->prox != NULL)
                    {
                        quant++;
                        atualOrigem = atualOrigem->prox;
                    } // end while(atualOrigem->prox != NULL)

                    // Inser��o da posi��o que inicia o bloco que o usu�rio quer mover
                    printf("Digite o indice da carta que inicia o bloco que deseja mover (1 - %d): ", quant + 1);
                    scanf("%d", &indice);
                    indice--;

                    // Verifica se o �ndice � v�lido
                    if ((indice >= 0) && (indice <= quant))
                    {
                        // Percorre a mesa at� o �ndice passado pelo usu�rio
                        antOrigem = NULL;
                        atualOrigem = primMesa[posOrigemMesa];

                        while (i != indice)
                        {
                            antOrigem = atualOrigem;
                            atualOrigem = atualOrigem->prox;
                            i++;
                        } // end while(i != indice)

                        // Verifica se as cartas do bloco est�o na sequ�ncia correta
                        if (sequenciaCorreta(atualOrigem))
                        {
                            // Se a mesa de destino est� vazia
                            if (primMesa[posDestinoMesa] == NULL)
                            {
                                // Verifica se a carta do topo que est� sendo movida � a 13 (rei)
                                if (atualOrigem->num == 13)
                                {
                                    // Se a mesa de origem possu�a somente um elemento
                                    if (antOrigem == NULL)
                                    {
                                        // Move o bloco
                                        primMesa[posDestinoMesa] = atualOrigem;
                                        primMesa[posOrigemMesa] = NULL;
                                        moves++;
                                    } // end if(antOrigem == NULL)
                                    else
                                    {
                                        // Move o bloco
                                        primMesa[posDestinoMesa] = atualOrigem;
                                        antOrigem->prox = NULL;
                                        moves++;
                                    } // end else
                                }     // end if(atualOrigem->num == 13)
                                else
                                {
                                    mensagem(":::Mesas vazias so aceitam a carta 13 (rei)!:::");
                                } // end else
                            }     // end if(primMesa[posDestinoMesa] == NULL)
                            // Verifica se a carta do in�cio do bloco que se deseja mover est� na sequ�ncia da mesa de destino
                            else if ((coresDiferentes(atualOrigem, atualDestino)) && (atualDestino->num - 1 == atualOrigem->num))
                            {
                                // Se h� apenas uma carta na origem
                                if (antOrigem == NULL)
                                {
                                    // Move a carta
                                    atualDestino->prox = atualOrigem;
                                    primMesa[posOrigemMesa] = NULL;
                                    moves++;
                                } // end if(antOrigem == NULL)
                                else
                                {
                                    // Move a carta
                                    atualDestino->prox = atualOrigem;
                                    antOrigem->prox = NULL;
                                    moves++;
                                } // end else
                            }     // end else if()
                            else
                            {
                                mensagem(":::A carta do topo do bloco a ser movido deve ser de uma cor diferente e na sequencia da que esta no destino!:::");
                            } // end else
                        }     // end if(sequenciaCorreta(atualOrigem))
                        else
                        {
                            mensagem(":::As cartas devem estar na sequencia correta para serem movidas!:::");
                        } // end else
                    }     // end else if((indice >= 0) && (indice <= quant))
                    else
                    {
                        mensagem(":::Indice invalido!:::");
                    } // end else
                }     // end else if(movimento == 2)
                else
                {
                    mensagem(":::Insira uma opcao valida!:::");
                } // end else
            }     // end if((posDestinoMesa >= 0) && (posDestinoMesa <= 7))
            else
            {
                mensagem(":::Posicao da MESA de destino invalida!:::");
            } // end else
        }     // end if(primMesa[posOrigemMesa] != NULL)
        else
        {
            system("cls");
            printf(":::Sem carta na pilha %d da MESA de origem!:::\n\n", posOrigemMesa + 1);
            system("pause");
        } // end else
    }     // end if((posOrigemMesa >= 0) && (posOrigemMesa <= 7))
    else
    {
        mensagem(":::Posicao da MESA de origem invalida!:::");
    } // end else

} // end moveMesaMesa()

// Fun��o para destruir as pilhas
void destroiTudo()
{
    tCarta *atual;
    tCarta *apaga;
    int i;

    // Percorre as posi��es de MESA (0 a 7)
    for (i = 0; i < TAM_MESA; i++)
    {
        // Come�a pelo topo da pilha
        atual = primMesa[i];
        while (atual != NULL)
        {
            // Apaga recebe o ponteiro que ter� a mem�ria liberada
            apaga = atual;
            // Passa para a pr�xima carta da pilha
            atual = atual->prox;
            // Libera a mem�ria
            free(apaga);
        } // end while(atual != NULL)
    }     // end for(i = 0; i < TAM_MESA; i++)

    // Percorre as posi��es de NAIPE (0 a 3)
    for (i = 0; i < TAM_NAIPE; i++)
    {
        atual = primNaipe[i];
        while (atual != NULL)
        {
            apaga = atual;
            atual = atual->prox;
            free(apaga);
        } // end while(atual != NULL)
    }     // end for(i = 0; i < TAM_NAIPE; i++)

    // Percorre as posi��es de TEMP (0 a 3)
    for (i = 0; i < TAM_TEMP; i++)
    {
        atual = primTemp[i];
        while (atual != NULL)
        {
            apaga = atual;
            atual = atual->prox;
            free(apaga);
        } // end while(atual != NULL)
    }     // end for(i = 0; i < TAM_TEMP; i++)

} // end destroiTudo()

// Fun��o para reiniciar o jogo
void reiniciar()
{
    int i;

    // Libera a mem�ria das cartas criadas
    destroiTudo();

    // Inicializa os montes
    primMonte = NULL;

    // Reseta o valor dos movimentos
    moves = 0;

    // Percorre as posi��es de MESA (0 a 7)
    for (i = 0; i < TAM_MESA; i++)
    {
        primMesa[i] = NULL;
    } // end for(i = 0; i < TAM_MESA; i++)

    // Percorre as posi��es de NAIPE (0 a 3)
    for (i = 0; i < TAM_NAIPE; i++)
    {
        primNaipe[i] = NULL;
    } // end for(i = 0; i < TAM_NAIPE; i++)

    // Percorre as posi��es de TEMP (0 a 3)
    for (i = 0; i < TAM_TEMP; i++)
    {
        primTemp[i] = NULL;
    } // end for(i = 0; i < TAM_TEMP; i++)

    // Gera o baralho, embaralha ele e distribui nas mesas
    gerarBaralho();
    embaralhaBaralho();
    distribuirMesa();

} // end reiniciar()

bool playerWin()
{
    int i;

    // Percorre a pilha de Naipes verificando se as condi��es de vit�ria est�o atendidas
    for (i = 0; i < TAM_NAIPE; i++)
    {
        if (primNaipe[i] != NULL)
        {
            if (primNaipe[i]->num != 13)
            {
                return false;
            } // end if(primNaipe[i]->num != 13)
        }     // end if(primNaipe[i] != NULL)
        else
        {
            return false;
        } // end else

    } // end for(i = 0; i < TAM_NAIPE; i++)
    return true;

} // end playerWin()

// Fun��o principal
int main(int argc, char **argv)
{
    int op = 0;
    char confirm = 'n';

    backcolor(GREEN); // Cor de fundo do terminal muda para verde

    // Inicia uma semente aleat�ria para a fun��o rand()
    srand(time(NULL));

    // Gera o baralho
    gerarBaralho();

    // Embaralha o baralho
    embaralhaBaralho();

    // Distribui as cartas na mesa
    distribuirMesa();

    // Imprime a tela inicial do jogo
    telaInicial();

    while (op != 9)
    {
        // Imprime TEMP, NAIPE e MESA
        imprime();

        textcolor(WHITE); // Cor do texto branca

        // Menu
        printf("Movimentos: %04d\n", moves);
        printf("(1) - Mover: MESA --> TEMP\n");
        printf("(2) - Mover: MESA --> NAIPE\n");
        printf("(3) - Mover: TEMP --> MESA\n");
        printf("(4) - Mover: NAIPE --> TEMP\n");
        printf("(5) - Mover: TEMP --> NAIPE\n");
        printf("(6) - Mover: NAIPE --> MESA\n");
        printf("(7) - Mover: MESA --> MESA\n");
        printf("(8) - Reiniciar\n");
        printf("(9) - Sair\n");

        // Inser��o da op��o pelo usu�rio
        printf("Opcao: ");
        scanf("%d", &op);

        getchar();

        // Dependendo da op��o realiza uma a��o
        switch (op)
        {
        case 1:
            // Mover de MESA para TEMP
            moveMesaTemp();
            break;
        case 2:
            // Mover de MESA para NAIPE
            moveMesaNaipe();
            break;
        case 3:
            // Mover de TEMP para MESA
            moveTempMesa();
            break;
        case 4:
            // Mover de NAIPE para TEMP
            moveNaipeTemp();
            break;
        case 5:
            // Mover de TEMP para NAIPE
            moveTempNaipe();
            break;
        case 6:
            // Mover de NAIPE para MESA
            moveNaipeMesa();
            break;
        case 7:
            // Mover de Mesa para MESA
            moveMesaMesa();
            break;
        case 8:
            // Reiniciar
            printf("Tem certeza?(s/n) ");
            scanf("%c", &confirm);

            if (confirm == 's')
            {
                reiniciar();
            } // end if(confirm != 's')
            break;
        case 9:
            // Sair
            printf("Tem certeza?(s/n) ");
            scanf("%c", &confirm);

            if (confirm != 's')
            {
                op = 0;
            } // end if(confirm != 's')

            printf("\n");
            break;
        default:
            // Op��o fora do menu
            system("cls");
            printf(":::Opcao Invalida!:::\n\n");
            system("pause");
            break;
        } // end switch(op)

        if (playerWin())
        {
            op = 9;
            // Imprime a tela final do jogo
            telaFinal();
        }

    } // end while(op != 4)

    // Libera��o da mem�ria
    destroiTudo();

    system("pause");
    return 0;

} // end main()
