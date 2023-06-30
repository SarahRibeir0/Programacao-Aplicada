#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Fonts/TomThumb.h>
#include <Fonts/FreeMono9pt7b.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET A7     //8 // Reset pin # (or -1 if sharing Arduino reset pin)
#define ONE_WIRE_BUS 4    // Defina o pino digital ao qual o sensor está conectado
#define AO2 3             // Saída do LED digital


Servo AO1;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Declaração de pinos

#define DI1 A1     //2
#define DI2 A2     //3
#define ByPass A0  //4
#define DO1 9      //saida do buzzer
#define DO2 7      //saida de led digital


//Variáveis globais
float valor = 0;
float tensao = 0;
float tensao_grafico = 0;
int porcentagem = 0;
int outServo = 90;
int lastState_ByP = 0;
int outState_DO1 = 0;
int outState_DO2 = 0;
int outState_AO1 = 0;
int outState_AO2 = 0;

void controlServo(int angle) {
  angle = constrain(angle, 0, 180);  // Limita o ângulo entre 0 e 180
  AO1.write(angle);                  // Move o servo para a posição desejada
}

void setup() {
  Serial.begin(9600);

  AO1.attach(4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // inicia a comunicação com o display OLED
  display.clearDisplay();                     //Limpa o display
  //Definição de Pinagem
  pinMode(DI1, INPUT);
  pinMode(DI2, INPUT);
  pinMode(ByPass, INPUT);
  pinMode(DO1, OUTPUT);
  pinMode(DO2, OUTPUT);

  digitalWrite(DO1, LOW);
  digitalWrite(DO2, LOW);
}

void loop() {

  int brilhoLed;
  int servoAngle;
  int buttonState_DI1 = digitalRead(DI1);
  int buttonState_DI2 = digitalRead(DI2);
  int buttonState_ByP = digitalRead(ByPass);

  if (Serial.available() > 0) {
    int comando = Serial.parseInt();

    if (buttonState_ByP == LOW) {  // Executa quando o botão By-Pass foi pressionado
      while (digitalRead(ByPass) == LOW) {
      }
      lastState_ByP = !lastState_ByP;

      Serial.println("Botão By-Pass foi pressionado.");
    } else {
      // Verifica qual comando foi recebido e aciona o botão correspondente
      switch (comando) {
        case 1:
          outState_DO1 = LOW;
          Serial.println("Buzzer Desligado");
          break;

        case 2:
          outState_DO1 = HIGH;
          Serial.println("Buzzer Ligado");
          break;

        case 3:
          outState_DO2 = LOW;
          Serial.println("Led desligado");
          break;

        case 4:
          outState_DO2 = HIGH;
          Serial.println("Led ligado");
          break;

        // Para mostrar o valor do potenciômetro na tela 1 vez
        case 5:
          Serial.println(tensao);
          break;

        // Para mostrar o valor do potenciômetro na tela 5 vez
        case 6:
          for (int i = 0; i < 5; i++) {
            valor = analogRead(A3);         // Leitura do potenciômetro em A0
            tensao = valor * (5.0 / 1023);  // Cálculo para valor de tensão
            Serial.print("Valor do potenciometro: ");
            Serial.println(tensao);
            delay(1000);  // Aguarda 1 segundo entre as leituras
          }
          break;


        case 7:

          brilhoLed = Serial.parseInt();                    // Lê o valor do brilho do LED
          brilhoLed = constrain(brilhoLed, 0, 100);         // Limita o valor do brilho entre 0 e 100
          porcentagem = brilhoLed;                          // Atualiza a variável porcentagem
          outState_AO2 = map(porcentagem, 0, 100, 0, 255);  // Calcula o valor correspondente de brilho
          analogWrite(AO2, outState_AO2);                   // Define o brilho do LED
          Serial.print("Brilho do LED ajustado para: ");
          Serial.println(porcentagem);
          break;

        case 8:
          int servoAngle;
          servoAngle = Serial.parseInt();  // Lê o valor digitado pelo usuário

          controlServo(servoAngle);  // Chama a função para mover o servo para o ângulo desejado
          Serial.print("Servo movido para o ângulo: ");
          Serial.println(servoAngle);
          break;

        default:
          Serial.println("Comando inválido.");
          break;
      }
    }
  }

  if (buttonState_ByP == LOW) {  //executa quando o botao bypass foi pressionado
    while (digitalRead(ByPass) == LOW) {}
    lastState_ByP = !lastState_ByP;
  }

  if (lastState_ByP == HIGH) {
    outState_DO1 = !buttonState_DI1;
    outState_DO2 = !buttonState_DI2;
    outState_AO1 = outServo;
    outState_AO2 = porcentagem;
    AO1.write(outServo);
  }

  digitalWrite(DO1, outState_DO1);
  digitalWrite(DO2, outState_DO2);


  main_screen(buttonState_DI1, buttonState_DI2, buttonState_ByP, porcentagem);


  valor = analogRead(A3);         //leitura do potenciômetro em A0
  tensao = valor * (5.0 / 1023);  //cálculo para valor de tensão

  //tensao_grafico=map(valor,0,1023,10,128); //dimensiona o valor analógico para um pixel imprimível no limite da tela
  tensao_grafico = map(valor, 0, 1023, 1, 128);  //dimensiona o valor analógico para um pixel imprimível no limite da tela
  outServo = map(valor, 0, 1023, 180, 0);

  porcentagem = map(valor, 0, 1023, 0, 1000);  //dimensiona o valor analógico para um pixel imprimível no limite da tela
  porcentagem = (porcentagem / 10.0);
}
// fim do Loop

//===========================================================================

void main_screen(int buttonState_DI1, int buttonState_DI2, int buttonState_ByP, int porcentagem) {


  display.clearDisplay();  //Limpa o display

  display.setRotation(1);  //pode ser valor 1 ou 3
  display.drawRoundRect(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, 6, SSD1306_WHITE);

  display.drawLine(0, 15, SCREEN_WIDTH, 15, SSD1306_WHITE);
  display.drawLine(0, 30, SCREEN_WIDTH, 30, SSD1306_WHITE);
  display.drawLine(0, 40, SCREEN_WIDTH, 40, SSD1306_WHITE);
  display.drawLine(0, 50, SCREEN_WIDTH, 50, SSD1306_WHITE);
  display.drawLine(0, 60, SCREEN_WIDTH, 60, SSD1306_WHITE);
  display.drawLine(0, 72, SCREEN_WIDTH, 72, SSD1306_WHITE);
  display.drawLine(0, 87, SCREEN_WIDTH, 87, SSD1306_WHITE);
  display.drawLine(0, 97, SCREEN_WIDTH, 97, SSD1306_WHITE);
  display.drawLine(0, 107, SCREEN_WIDTH, 107, SSD1306_WHITE);
  display.drawLine(0, 117, SCREEN_WIDTH, 117, SSD1306_WHITE);

  display.drawLine(display.width() - 15, 30, display.width() - 15, 60, SSD1306_WHITE);
  display.drawLine(display.width() - 15, 87, display.width() - 15, 128, SSD1306_WHITE);

  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text


  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setFont(&TomThumb);
  display.setCursor(8, 10);  // Start at top-left corner
  display.println(F("SUPERVISORIO"));
  display.setCursor(16, 25);  // Start at top-left corner
  display.println(F("ENTRADAS"));
  display.setCursor(2, 38);  // Start at top-left corner
  display.println(F("DI1: Botao"));
  display.setCursor(2, 48);  // Start at top-left corner
  display.println(F("DI2: Hall"));
  display.setCursor(2, 58);  // Start at top-left corner
  display.println(F("AI1: Pot."));

  display.setCursor(52, 58);
  display.print(porcentagem);

  display.setCursor(20, 82);  // Start at top-left corner
  display.println(F("SAIDAS"));
  display.setCursor(2, 95);  // Start at top-left corner
  display.println(F("DO1: Buzzer"));
  display.setCursor(2, 105);  // Start at top-left corner
  display.println(F("DO2: Fan"));
  display.setCursor(2, 115);  // Start at top-left corner
  display.println(F("AO1: Servo"));
  display.setCursor(2, 125);  // Start at top-left corner
  display.println(F("AO2: Led"));

  display.setCursor(52, 115);
  display.print(outState_AO1);
  display.setCursor(52, 125);
  display.print(outState_AO2);

  if (buttonState_DI1 == LOW) {
    display.fillRect(51, 32, 11, 7, SSD1306_WHITE);
  }
  if (buttonState_DI2 == LOW) {
    display.fillRect(51, 42, 11, 7, SSD1306_WHITE);
  }
  if (lastState_ByP == HIGH) {
    display.fillRoundRect(2, 62, 60, 9, 3, SSD1306_WHITE);
    //display.fillRect(2, 62, 60, 9, SSD1306_WHITE);
    display.setTextColor(SSD1306_INVERSE);
    display.setCursor(20, 69);
    display.println(F("BY-PASS"));
  }
  if (outState_DO1 == HIGH) {
    display.fillRect(51, 89, 11, 7, SSD1306_WHITE);
  }
  if (outState_DO2 == HIGH) {
    display.fillRect(51, 99, 11, 7, SSD1306_WHITE);
  }

  display.display();
}