#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "homefi-IoT";
const char* password = "244466666";

LiquidCrystal_I2C lcd(0x27, 20, 4); // Endereço I2C, colunas, linhas

byte nivelUmC[] = { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 };
byte nivelUmV[] = { B00000, B00000, B00000, B00000, B00000, B11111, B10001, B11111 };
byte nivelDoisC[] = { B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111 };
byte nivelDoisV[] = { B00000, B00000, B00000, B00000, B11111, B10001, B10001, B11111 };
byte nivelTresC[] = { B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111 };
byte nivelTresV[] = { B00000, B00000, B00000, B11111, B10001, B10001, B10001, B11111 };
byte nivelQuatroC[] = { B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111 };
byte nivelQuatroV[] = { B00000, B00000, B11111, B10001, B10001, B10001, B10001, B11111 };
byte nivelCincoC[] = { B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte nivelCincoV[] = { B00000, B11111, B10001, B10001, B10001, B10001, B10001, B11111 };
byte nivelSeisC[] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte nivelSeisV[] = { B11111, B10001, B10001, B10001, B10001, B10001, B10001, B11111 };
byte offChar[] = { B00100, B01010, B01010, B01010, B01010, B01110, B11111, B01110 };
byte onChar[] = { B01110, B11111, B01110, B01110, B01110, B01110, B01110, B00100 };

bool impressao;
bool estadoDaBomba;
int nivelCaixa;
int nivelSisterna;
const int PinReleBomba = D0;
const int PinBotao = D7;

// Função de interrupção para alternar o estado da bomba
void IRAM_ATTR handleButtonPress() {
  estadoDaBomba = !estadoDaBomba; // Alterna o estado da bomba
  digitalWrite(PinReleBomba, estadoDaBomba ? LOW : HIGH); // Liga/desliga imediatamente
}

void limparPosicionarLinha(int linha)
{
  lcd.setCursor(0, linha);
  lcd.print("                ");
  lcd.setCursor(0, linha);
}

void mapNivel(int nivel)
{
  switch (nivel) {
    case 0:
      lcd.createChar(0, nivelUmV);
      lcd.createChar(1, nivelDoisV);
      lcd.createChar(2, nivelTresV);
      lcd.createChar(3, nivelQuatroV);
      lcd.createChar(4, nivelCincoV);
      lcd.createChar(5, nivelSeisV);
      break;
    case 1:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisV);
      lcd.createChar(2, nivelTresV);
      lcd.createChar(3, nivelQuatroV);
      lcd.createChar(4, nivelCincoV);
      lcd.createChar(5, nivelSeisV);
      break;
    case 2:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisC);
      lcd.createChar(2, nivelTresV);
      lcd.createChar(3, nivelQuatroV);
      lcd.createChar(4, nivelCincoV);
      lcd.createChar(5, nivelSeisV);
      break;
    case 3:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisC);
      lcd.createChar(2, nivelTresC);
      lcd.createChar(3, nivelQuatroV);
      lcd.createChar(4, nivelCincoV);
      lcd.createChar(5, nivelSeisV);
      break;
    case 4:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisC);
      lcd.createChar(2, nivelTresC);
      lcd.createChar(3, nivelQuatroC);
      lcd.createChar(4, nivelCincoV);
      lcd.createChar(5, nivelSeisV);
      break;
    case 5:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisC);
      lcd.createChar(2, nivelTresC);
      lcd.createChar(3, nivelQuatroC);
      lcd.createChar(4, nivelCincoC);
      lcd.createChar(5, nivelSeisV);
      break;
    case 6:
      lcd.createChar(0, nivelUmC);
      lcd.createChar(1, nivelDoisC);
      lcd.createChar(2, nivelTresC);
      lcd.createChar(3, nivelQuatroC);
      lcd.createChar(4, nivelCincoC);
      lcd.createChar(5, nivelSeisC);
      break;
  }
}

void mapOnOff(bool on)
{
  if (on)
  {
    lcd.createChar(6, onChar);
  } else {
    lcd.createChar(6, offChar);
  }
}

void imprimeNivel(int linha, String texto, int nivel)
{
  
  mapNivel(nivel);
  limparPosicionarLinha(linha);
  
  for (int i = 0; i <= 5; i++) {
    lcd.write(byte(i));
  }

  lcd.print(" ");
  lcd.print(texto);
}

void imprimeOnOff(int linha, String texto, bool on)
{
  mapOnOff(on);
  limparPosicionarLinha(linha);

  lcd.write(6);

  lcd.print(" ");
  lcd.print(texto);
}

bool ligarBomba(int nivCaixa, int nivSisterna, bool bombaLigada)
{
  bool toReturn = bombaLigada; // Assume o estado atual como padrão

  Serial.println("----------------");
  Serial.print("Nivel Caixa: ");
  Serial.println(nivCaixa);
  Serial.print("Nivel Sisterna: ");
  Serial.println(nivSisterna);
  Serial.print("Bomba estava ligada: ");
  Serial.println(bombaLigada);

  if (bombaLigada) {
    if (nivCaixa >= 6 || nivSisterna <= 0) {
      toReturn = false; 
    }
  } else {
    if (nivCaixa <= 2 && nivSisterna >= 1) {
      toReturn = true;
    }
  }

  Serial.print("Bomba liga/desliga: ");
  Serial.println(toReturn);
  Serial.println("----------------");

  if (toReturn) {
    digitalWrite(PinReleBomba, LOW);  // Liga o relé
    Serial.println("Liga bomba");
  } else {
    digitalWrite(PinReleBomba, HIGH); // Desliga o relé
    Serial.println("Desliga bomba");
  }

  return toReturn;
}

void scrollText(String text, int row)
{
  int len = text.length();
  if (len <= 16) {
    // Se o texto cabe na linha, apenas exibe
    lcd.setCursor(0, row);
    lcd.print(text);
  } else {
    // Caso contrário, faz o scroll
    for (int i = 0; i < len - 15; i++) {
      lcd.setCursor(0, row);
      lcd.print(text.substring(i, i + 16)); // Mostra 12 caracteres por vez
      delay(1000); // Delay para controlar a velocidade do scroll
    }
  }
}

void setup() {
  pinMode(PinReleBomba, OUTPUT);
  digitalWrite(PinReleBomba, HIGH); // Desliga o relé inicialmente

  pinMode(PinBotao, INPUT_PULLUP); // Configura o pino do botão com pull-up interno

  // Configura a interrupção no pino do botão, acionada na borda de descida (pressionado)
  attachInterrupt(digitalPinToInterrupt(PinBotao), handleButtonPress, FALLING);

  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  lcd.begin();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(ssid);

  WiFi.begin(ssid, password);

  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);

    Serial.print("Tentativa ");
    Serial.print(retry_count);
    Serial.print(": Status da conexao: ");
    Serial.println(WiFi.status());
    
    lcd.setCursor(0, 1);
    lcd.print("Conectando ");
    
    int numDots = retry_count % 4;
    switch (numDots) {
      case 0:
        lcd.print("   ");
        break;
      case 1:
        lcd.print(".  ");
        break;
      case 2:
        lcd.print(".. ");
        break;
      case 3:
        lcd.print("...");
        break;
    }
    
    retry_count++;

    if (retry_count > 30) {
      Serial.println("Falha ao conectar. Reiniciando o módulo Wi-Fi...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      retry_count = 0;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Conectado!");
  
  lcd.setCursor(0, 1);
  String ip = "IP: " + WiFi.localIP().toString();
  scrollText(ip, 1);

  Serial.println("");
  Serial.println("WiFi Conectado!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(2000);

  impressao = false;
  nivelCaixa = 0;
  nivelSisterna = 0;
  estadoDaBomba = false;
}

void loop() {
  int numero = random(0, 7); // Ajustado para incluir o nível 6
  String texto;

  if (impressao) {
    texto = "Caixa";
    nivelCaixa = numero;
  } else{
    texto = "Sisterna";
    nivelSisterna = numero;
  }

  imprimeNivel(0, texto.c_str(), numero);
  impressao = !impressao;

  // Verifica os níveis e ajusta o estado da bomba se necessário
  estadoDaBomba = ligarBomba(nivelCaixa, nivelSisterna, estadoDaBomba);
  digitalWrite(PinReleBomba, estadoDaBomba ? LOW : HIGH); // Liga/desliga imediatamente
  imprimeOnOff(1, "Bomba", estadoDaBomba);

  delay(5000);
}
