#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "homenet";
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
  Serial.println("----------------");
  Serial.print("Nivel Caixa: ");
  Serial.println(nivCaixa);
  Serial.print("Nivel Sisterna: ");
  Serial.println(nivSisterna);
  Serial.print("Bomba ligada: ");
  Serial.println(bombaLigada);

  if (bombaLigada) {
    if (nivCaixa == 6) {
      return false;
    }
    if (nivSisterna == 0) {
      return false;
    }
    return true;
  } else {
    if (nivCaixa <= 2 && nivSisterna >= 1) {
      return true;
    } else {
      return false;
    }
  }
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

  Serial.begin(115200); // Inicia a comunicação serial
  while (!Serial) {
    ; // Espera até que a porta serial esteja pronta, necessário em alguns boards como o Leonardo
  }
  lcd.begin();
  lcd.clear();
  lcd.backlight();

  // Exibe a mensagem de conexão no LCD
  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");

  // Inicia a conexão Wi-Fi
  WiFi.begin(ssid, password);

  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    
    // Exibe "..." no LCD enquanto tenta conectar
    lcd.setCursor(0, 1);
    lcd.print("Tentando ");
    for (int i = 0; i <= retry_count % 3; i++) {
      lcd.print(".");
    }
    retry_count++;
  }

  // Limpa a linha e exibe uma mensagem de sucesso
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Conectado!");
  
  lcd.setCursor(0, 1);
  String ip = "IP: " + WiFi.localIP().toString();
  scrollText(ip, 1); // Faz scroll na linha 1 (segunda linha) com o IP

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
  int numero = random(0, 6);
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

  estadoDaBomba = ligarBomba(nivelCaixa, nivelSisterna, estadoDaBomba);
  imprimeOnOff(1, "Bomba", estadoDaBomba);

  delay(5000);
}
