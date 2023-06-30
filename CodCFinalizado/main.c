#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// função para configuração da porta serial
void configurarPortaSerial(HANDLE *hSerial)
{
    char portaArduino[] = "COM1"; // Inserir aqui a porta em que o arduino esta configurado.

    /*Aqui vamos abir a porta serial passada acima os parâmetros GENERIC_WRITE e GENERIC_READ
    indicam que a porta será aberta para escrita e leitura.*/
    *hSerial = CreateFile(portaArduino, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    // Verificação se não for possível abrir a porta serial.
    if (*hSerial == INVALID_HANDLE_VALUE)
    {
        printf("Falha ao abrir a porta serial!\n");
        exit(1); // encerra o programa e retorna um "erro".
    }

    // Configuração das propriedades da porta serial que será usada
    DCB dcbSerialParams = {0};

    /*Abaixo iremos definir o tamanho da estrutura DCB utilizando a função sizeof
    isso garante que a estrutura esteja corretamente dimensionada para o uso posterior. */
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    // Verificação se tive falha na comunicação serial
    if (!GetCommState(*hSerial, &dcbSerialParams))
    {
        printf("Falha ao obter as configurações da porta serial.\n");
        CloseHandle(*hSerial);
        exit(1); // encerra o programa e retorna um "erro".
    }

    dcbSerialParams.BaudRate = CBR_9600;   // Configurar a taxa de transmissão para 9600 bps
    dcbSerialParams.ByteSize = 8;          // Configurar byte para 8 que é o padrão para enviar e receber cada caractere é representado por 8 bits de dados.
    dcbSerialParams.StopBits = ONESTOPBIT; // Configurar 1 bit de parada que são usados para indicar o final de um caractere transmitido na comunicação serial.
    dcbSerialParams.Parity = NOPARITY;     // Desativar paridade da comunicação Serial

    // Verificação se tiver falha na configuração da porta serial.
    if (!SetCommState(*hSerial, &dcbSerialParams))
    {
        printf("Falha ao configurar as propriedades da porta serial.\n");
        CloseHandle(*hSerial);
        exit(1); // encerra o programa e retorna um "erro".
    }

    // Configurar os timeouts da porta serial
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;         // Define o tempo máximo de intervalo entre leituras consecutivas na porta serial em milessegundos.
    timeouts.ReadTotalTimeoutConstant = 50;    // Define o tempo máximo de espera para a conclusão de uma operação de leitura
    timeouts.ReadTotalTimeoutMultiplier = 10;  // Aplicado ao número de bytes esperados para leitura, que influencia o tempo máximo de espera para a conclusão de leitura.
    timeouts.WriteTotalTimeoutConstant = 50;   // Define o tempo máximo de espera para a conclusão de uma operação de escrita.
    timeouts.WriteTotalTimeoutMultiplier = 10; // Aplicado ao número de bytes esperados para escrita, que influencia o tempo máximo de espera para a conclusão da escrita.

    // Verificação se tiver falha na configuração dos timeouts.
    if (!SetCommTimeouts(*hSerial, &timeouts))
    {
        printf("Falha ao configurar os timeouts da porta serial.\n");
        CloseHandle(*hSerial);
        exit(1); // encerra o programa e retorna um "erro".
    }
}

// Função para limpar o terminal.
void limparTerminal()
{
    system("cls");
}

void exibirMenu(HANDLE hSerial)
{
    // Menu principal
    int input;
    int continuar = 1;
    int servoAngle;
    int numero = 8;
    char escrebe[10];

    do
    {
        limparTerminal();

        printf("Digite 0 para enviar comando digital\n");
        printf("Digite 9 para enviar comando analogico\n");
        printf("Digite 10 opcoes do potenciometro\n");
        printf("Digite 4 para sair\n");
        scanf("%d", &input);

        // Envia os comandos ao Arduino
        DWORD bytesWritten; // (Double Word) tipo de dados utilizado para representar um valor não negativo de 32 bits.
        char Escreve[255];

        switch (input)
        {
        case 4:
            continuar = 0; // Define a variável de controle como 0 para sair do loop
            break;

            // Case para entrar no comando digital
        case 0:
        {
            int ledStatus = 0;
            int buzzerStatus = 0;
            int command;

            do
            {
                // limparTerminal();

                printf("Digite 4 para acender o LED\n");
                printf("Digite 3 para apagar o LED\n");
                printf("Digite 2 para ligar o buzzer\n");
                printf("Digite 1 para desligar o buzzer\n");
                printf("Digite 5 para voltar ao menu principal\n");
                scanf("%d", &command);

                switch (command)
                {
                case 4:
                    if (ledStatus == 0)
                    {
                        Escreve[0] = 'D';
                        Escreve[1] = '4';
                        Escreve[2] = '\0';

                        /* comando hSerial escreve no arquivo de comunicação
                        Escreve é os dados que desejamos escrever ao serial do arduino
                        &bytesWritten recebe o numero de bytes escritos no serial
                        */
                       //Aqui escrevemos os dados para a porta serial do arduino utilizando o char Escreve
                        WriteFile(hSerial, Escreve, 2, &bytesWritten, NULL);
                        printf("Enviando comando para acender o LED...\n");

                        // Ler a resposta do Arduino
                        DWORD bytesRead;
                        char Resposta[255];

                        /* sizeof é o tamanho do buffer de leitura é a área de memória temporária utilizada*/
                        if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                        {
                            Resposta[bytesRead] = '\0';
                            printf("Resposta do Arduino: %s\n", Resposta);
                        }
                        else
                        {
                            printf("Falha ao ler dados da porta serial.\n");
                        }

                        ledStatus = 1;
                    }
                    else
                    {
                        printf("O LED ja esta aceso.\n");
                    }
                    break;
                case 3:
                    if (ledStatus == 1)
                    {
                        Escreve[0] = 'D';
                        Escreve[1] = '3';
                        Escreve[2] = '\0';
                        WriteFile(hSerial, Escreve, 2, &bytesWritten, NULL);
                        printf("Enviando comando para apagar o LED...\n");

                        // Ler a resposta do Arduino
                        DWORD bytesRead;
                        char Resposta[255];
                        if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                        {
                            Resposta[bytesRead] = '\0';
                            printf("Resposta do Arduino: %s\n", Resposta);
                        }
                        else
                        {
                            printf("Falha ao ler dados da porta serial.\n");
                        }

                        ledStatus = 0;
                    }
                    else
                    {
                        printf("O LED ja esta apagado.\n");
                    }
                    break;
                case 2:
                    if (buzzerStatus == 0)
                    {
                        Escreve[0] = 'C';
                        Escreve[1] = '2';
                        Escreve[2] = '\0';
                        WriteFile(hSerial, Escreve, 2, &bytesWritten, NULL);
                        printf("Enviando comando para ligar o buzzer...\n");

                        // Ler a resposta do Arduino
                        DWORD bytesRead;
                        char Resposta[255];
                        if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                        {
                            Resposta[bytesRead] = '\0';
                            printf("Resposta do Arduino: %s\n", Resposta);
                        }
                        else
                        {
                            printf("Falha ao ler dados da porta serial.\n");
                        }

                        buzzerStatus = 1;
                    }
                    else
                    {
                        printf("O buzzer ja esta ligado.\n");
                    }
                    break;
                case 1:
                    if (buzzerStatus == 1)
                    {
                        Escreve[0] = 'C';
                        Escreve[1] = '1';
                        Escreve[2] = '\0';
                        WriteFile(hSerial, Escreve, 2, &bytesWritten, NULL);
                        printf("Enviando comando para desligar o buzzer...\n");

                        // Ler a resposta do Arduino
                        DWORD bytesRead;
                        char Resposta[255];
                        if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                        {
                            Resposta[bytesRead] = '\0';
                            printf("Resposta do Arduino: %s\n", Resposta);
                        }
                        else
                        {
                            printf("Falha ao ler dados da porta serial.\n");
                        }

                        buzzerStatus = 0;
                    }
                    else
                    {
                        printf("O buzzer ja esta desligado.\n");
                    }
                    break;
                case 5:
                    break;
                default:
                    printf("Entrada invalida! Tente novamente.\n");
                    break;
                }

            } while (command != 5);

            break;
        }
        case 9:
        {
            // Variáveis para entrar no case e ajustar os valores analogicos
            int escolhaDoCase, Ajuste;
            // Menu de comando analógico
            do
            {
                //limparTerminal();

                printf("Digite 8 para controlar o servo motor\n");
                printf("Digite 7 para ajustar o brilho do LED\n");
                printf("Digite 3 para voltar ao menu principal\n");
                scanf("%d", &input);

                switch (input)
                {
                case 8:

                    printf("Digite o numero 8 aperte espaco e um valor de 0 a 180 para definir angulo do servo motor ");
                    scanf("%d %d", &escolhaDoCase, &Ajuste);

                    // Formata os números em uma string separados por espaço
                    char comando[32];
                    sprintf(comando, "%d %d", escolhaDoCase, Ajuste);

                    // Envia a string formatada para o Arduino
                    WriteFile(hSerial, comando, strlen(comando), &bytesWritten, NULL);

                    DWORD bytesRead;
                    char Resposta[255];
                    if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                    {
                        Resposta[bytesRead] = '\0';
                        printf("Resposta do Arduino: %s\n", Resposta);
                    }
                    else
                    {
                        printf("Falha ao ler dados da porta serial.\n");
                    }

                    break;

                case 7:
                {
                    // limparTerminal();
                    printf("Digite o numero 7 aperte espaco e um valor de 0 a 100 para definir o brilho: ");
                    scanf("%d %d", &escolhaDoCase, &Ajuste);

                    // Formata os números em uma string separados por espaço
                    char comando[32];
                    sprintf(comando, "%d %d", escolhaDoCase, Ajuste);

                    // Envia a string formatada para o Arduino
                    WriteFile(hSerial, comando, strlen(comando), &bytesWritten, NULL);

                    DWORD bytesRead;
                    char Resposta[255];
                    if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                    {
                        Resposta[bytesRead] = '\0';
                        printf("Resposta do Arduino: %s\n", Resposta);
                    }
                    else
                    {
                        printf("Falha ao ler dados da porta serial.\n");
                    }

                    break;
                }

                case 3:
                    break;
                default:
                    printf("Entrada inválida. Tente novamente.\n");
                    break;
                }
            } while (input != 3);

            break;
        }

        case 10:
        {
            // Menu do potenciometro
            do
            {

                printf("Digite 5 para ler o valor do potenciometro 1 vez \n");
                printf("Digite 2 para ler o valor do potenciometro em um intervalo de 5 segundos\n");
                printf("Digite 3 para voltar ao menu principal\n");
                scanf("%d", &input);

                switch (input)
                {
                case 5:
                {
                    limparTerminal();
                    Escreve[0] = 'P';
                    Escreve[1] = '5';
                    Escreve[2] = '\0';
                    WriteFile(hSerial, Escreve, 2, &bytesWritten, NULL);
                    printf("Lendo valor do potenciometro...\n");

                    // Ler a resposta do Arduino
                    DWORD bytesRead;
                    char Resposta[255];
                    if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                    {
                        Resposta[bytesRead] = '\0';
                        float valorPotenciometro = atof(Resposta); // Converter string para float
                        printf("Valor em volts: %.2f\n\n", valorPotenciometro);
                    }
                    else
                    {
                        printf("Falha ao ler dados da porta serial.\n");
                    }
                    break;
                }
                case 2:
                {
                    // Envia o comando para o Arduino
                    char Escreve[255];
                    sprintf(Escreve, "6\n");
                    DWORD bytesWritten;
                    WriteFile(hSerial, Escreve, strlen(Escreve), &bytesWritten, NULL);

                    // Lê as leituras do potenciometro enviadas pelo Arduino
                    for (int i = 0; i < 5; i++)
                    {
                        // Ler a resposta do Arduino
                        DWORD bytesRead;
                        char Resposta[255];
                        if (ReadFile(hSerial, Resposta, sizeof(Resposta) - 1, &bytesRead, NULL))
                        {
                            Resposta[bytesRead] = '\0';
                            printf("Valor do potenciometro: %s\n", Resposta);
                        }
                        else
                        {
                            printf("Falha ao ler dados da porta serial.\n");
                        }

                        Sleep(1000); // Aguarda 1 segundo entre as leituras
                    }

                    break;
                }

                case 3:
                    break;
                default:
                    printf("Entrada invalida! Tente novamente.\n");
                    break;
                }
            } while (input != 3);

            break;
        }
        default:
            printf("Entrada invalida! Tente novamente.\n");
            break;
        }
    } while (continuar);
}


// Na função Main vamos utilizar as funçõe e menu criado acima para fazer a comunicação com o arduino.
int main()
{
    HANDLE hSerial;
    configurarPortaSerial(&hSerial);
    exibirMenu(hSerial);
    CloseHandle(hSerial);

    return 0;
}
