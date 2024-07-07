/*
 * Autor: Ricardo Santos
 * LinkedIn: https://www.linkedin.com/in/ricardos7/
 * GitHub: https://github.com/Ricardo-SS
 * URL da biblioteca: https://github.com/Ricardo-SS/CatFoodSmart
 * Descrição: Este exemplo calcula o volume total e o volume disponível de um reservatório em forma de cone.
*/

#include <FS.h>                // FS: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266FS
#include <ESP8266WiFi.h>       // Biblioteca para utilizar o módulo WiFi
#include <Adafruit_SSD1306.h>  // Para controle do display OLED
#include <Adafruit_GFX.h>      // display
#include <WiFiManager.h>       // Para configurar a conexão WiFi
#include <ArduinoJson.h>  // Para manipulação de arquivos JSON: https://arduinojson.org/ por Benoit Blanchon
#include "pagina.h"       // Pagina web que criamos

//#include <Adafruit_SSD1306.h>  // Para controle do display OLED
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *CONFIG_FILE = "/horario.json";
int hora = 19;  //Definição de hora padrão
int minuto = 30;

// Criando uma variavel para controle de versão,[nome_versao_dataCompilação]
char versaoFirmware[] = "CatFood_v0.0.1_D25032024";

// Definições do display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1

// Criando servidor web local.
ESP8266WebServer server(80);

#define D3 14 //gpio 5, pino d3 para o esp8266 ou gpio 2 para o esp01s/pi 14 = d5 esp8266

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo motor;

const char* ssid = "DeepWeb";
const char* password = "rick1234";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

// Recebe os dados do formulário web
void handleFormSubmit() {
  String hora = server.args("hora");
  String minuto = server.args("minuto");

  hora = horatoString.toInt();
  minut = minutotoString.toInt();

  // Cria arquivo json para gravar as configurações
  DynamicJsonDocument doc(1024);
  doc["hora"] = hora;
  doc["minuto"] = minuto;

  // Abre o arquivo de configurações no modo de escrita
  File configFile = SPIFFS.open(CONFIG_FILE, "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
  }

  // Confirma se os dados foram gravados no arquivo
  server.send(200, "text/html", "<h1>Horario salvo com sucesso!<h1>")
}

// ------- credenciais firebase ------------- //
#define FIREBASE_HOST "seu-projet-default-rtdb.firebaseio.com"   // URL do banco de dados Firebase
#define FIREBASE_AUTH "agagargokjçiojçrgoijijojfewhKhuuW3taAA"  // Sua chave de autenticação do Firebase

// ---------------- wifi -------------------- //
WiFiManager wifiManager;  // Criação de um objeto WiFiManager para configuração do WiFi

// -------------- firebase ------------------ //
FirebaseData firebaseData;  // Criação de um objeto FirebaseData para interação com o Firebase


// ------- função chip id ------------------//
String idChip;
String getIdChip() {
  uint64_t chipid = ESP.getChipId();
  String id = String(chipid);
  return id;
}

int servoAngle = 0;
int targetHour = 0;
int targetMinute = 0;
bool motorActivated = false;

const int EXECUTION_ANGLE = 45;  
const int INITIAL_ANGLE = 0;     
const int DELAY_AFTER_EXECUTION = 5000; 
const int DELAY_BEFORE_RESTART = 3000;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);

  idChip = getIdChip();  // pega o id do chip e atribue a variavel idChip

   // Verificando dados no arquivo, caso não exista, cria arquivo, se existe, leia.
  if (SPIFFS.exists(CONFIG_FILE)) {
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.println("Erro ao ler arquivo de configuração");
    } else {
      formato = doc["hora"].as<int>();
      comprimento = doc["minuto"].as<int>();
    }
    configFile.close();  // fecha arquivo apos leitura
  }

  motor.attach(D3);
  motor.write(INITIAL_ANGLE);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CAT FOOD");
  display.display();

  // nova funcao para conexao wifi
  // Define o nome da rede WiFi como "CATFOOD-<id do chip>"
  char ssid[20];
  sprintf(ssid, "CATFOOD-%d", idChip);

  // Inicia o modo de configuração de WiFi
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 25);
  display.println("Conecte-se a");
  display.setCursor(10, 35);
  display.println(ssid);
  display.display();

  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);  // Mantém o portal de configuração aberto até que uma conexão seja estabelecida
  wifiManager.autoConnect(ssid);
  display.clearDisplay();

  int i = 1;
  // Aguarda a conexão com a rede WiFi
  unsigned long startMillis = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMillis < 1 * 60 * 1000UL) {

    // Barra de progresso --------------------------------------------------------
    i = (i + 2);
    display.setTextSize(1);
    display.setCursor(i, 25);
    display.println("|");
    display.setCursor(10, 45);
    display.println("Reconectando...");

    // Mostra ID do chip ------------------------------------------------------
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("ID: ");
    display.println(ESP.getChipId());  // verificar em teste se pode ser substituida por getIdChip()
    display.display();

    Serial.print(".");
    delay(1000);
  }

  // Se a rede não foi configurada em 5 minutos, reinicia o ESP
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Não foi possível configurar a rede em 5 minutos. Reiniciando...");
    display.clearDisplay();
    display.setCursor(10, 45);
    display.println("Falha na conexao");
    display.display();
    display.clearDisplay();
    display.println("Reiniciando...");
    display.display();
    ESP.restart();
  }

  // Exibe informações de conexão
  display.clearDisplay();
  display.setCursor(2, 2);
  display.println("Conexão WiFi ok!");
  display.println("IP local:");
  display.println(WiFi.localIP());
  display.display();

  // fim nova funcao para conexao wifi

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Sincronizando data e hora...");
  syncTime();
  delay(3000);

    // Enviando dados salvos anteriormente para a pagina web
  server.on("/", []() {
    String html = paginaHTML;
    html.replace("{hora}", String(formato));
    html.replace("{minuto}", String(comprimento));

    server.send(200, "text/html", html);
  });

  server.on("/form", handleFormSubmit);
  server.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  // inicializa a função firebase

  digitalWrite(LED_BUILTIN, HIGH);  // desliga o led interno da placa após iniciar

  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.println("Connected to WiFi");

  // server.begin();

  // timeClient.begin();

  // display.clearDisplay();
  // display.setCursor(0, 0);
  // display.println("CAT FOOD");
  // display.setCursor(0, 20);
  // display.println("Conectado ao WiFi");
  // display.setCursor(0, 40);
  // display.print("IP: ");
  // display.setTextSize(1);
  // display.println(WiFi.localIP());
  // display.display();
  // delay(3000);
}

void loop() {

    // Se a rede não foi configurada em 5 minutos, reinicia o ESP
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("");
    Serial.println("Não foi possível conectarse a rede. Reiniciando...");
    Serial.println("Reiniciando...");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 25);
    display.println("Falha na conexao");
    display.setCursor(10, 45);
    display.println("Reiniciando...");
    display.display();
    delay(3000);
    ESP.restart();
  }

  motor.write(INITIAL_ANGLE);
  timeClient.update();

  // WiFiClient client = server.available();

  // if (client) {
  //   handleClientRequest(client);
  // }

  server.handleClient();  // chama a função do servidor

  updateDisplay();

  executeScheduledAction();

  // novo codigo de envio
#define TEMPO_ENVIO 120000  //  definir o tempo de envio para 2 minutos (em milissegundos)

  // variável para controlar o tempo de envio
  static unsigned long ultima_atualizacao = 0;

  // Obter hora atual e verificar se é hora de atualizar
  static int lastMinute = -1;
  if (lastMinute != timeinfo->tm_min) {
    // atualizar minuto
    lastMinute = timeinfo->tm_min;

    // enviar dados a cada 2 minutos
    if (lastMinute % 2 == 0) {
      Firebase.setString(firebaseData, "/" + idChip + "/ip", WiFi.localIP().toString().c_str());
      Firebase.pushString(firebaseData, "/" + idChip + "/hora", timeString);
      Serial.println("Dados enviados para o Firebase com sucesso");
      ultima_atualizacao = millis();  // atualizar tempo de envio
    }
  }
}

void handleClientRequest(WiFiClient client) {
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/horario") != -1) {
    int index = request.indexOf("?hora=");
    if (index != -1) {
      int index2 = request.indexOf("&", index);
      if (index2 != -1) {
        String horaStr = request.substring(index + 6, index2);
        int hora = horaStr.toInt();
        int minuto = request.substring(index2 + 8).toInt();

        if (hora >= 0 && hora <= 23 && minuto >= 0 && minuto <= 59) {
          targetHour = hora;
          targetMinute = minuto;
          motorActivated = true;
          Serial.print("Motor servo ativado para ");
          Serial.print(targetHour);
          Serial.print(":");
          Serial.println(targetMinute);
        }
      }
    }
  }
  
  // client.println("<html lang=\"pt-br\"><head>");
  // client.println("<title>CAT FOOD</title>");
  // client.println("<meta charset=\"UTF-8\">");
  // client.println("<style>");
  // client.println(".container { width: 400px; margin: 50px auto; background-color: #FFFFFF; border-radius: 10px; padding: 20px; }");
  // client.println(".title { background-color: #006400; color: #FFFFFF; padding: 10px; font-size: 20px; text-align: center; border-radius: 10px 10px 0 0; }");
  // client.println(".description { padding: 20px; margin-bottom: 40px; font-style: italic; color: #006400; }");
  // client.println(".time-input { display: flex; justify-content: center; align-items: center; }");
  // client.println(".time-input label { color: #000; font-weight: bold; margin-right: 10px; }");
  // client.println(".time-input input[type='number'] { width: 45%; padding: 10px; margin-bottom: 10px; border-radius: 5px; }");
  // client.println("input[type='submit'] { width: 100%; padding: 10px; margin-top: 40px; background-color: #006400; color: #FFFFFF; border: none; border-radius: 5px; cursor: pointer; }");
  // client.println("</style>");
  // client.println("</head><body>");
  // client.println("<div class='container'>");
  // client.println("<div class='title'>Alimentador Automático <span class='dog-icon'>.</span></div>");
  // client.println("<div class='description'>O melhor alimentador para seu pet!</div>");

  // client.println("<form action='/horario' method='get'>");
  // client.println("<div class='time-input'>");
  // client.println("<label for='hora'>Hora:</label>");
  // client.print("<input type='number' name='hora' placeholder='00' min='0' max='23' value='");
  // client.print(targetHour);
  // client.print("' required>");
  // client.println("<label for='minuto'>Minutos:</label>");
  // client.print("<input type='number' name='minuto' placeholder='00' min='0' max='59' value='");
  // client.print(targetMinute);
  // client.print("' required>");
  // client.println("</div>");
  // client.println("<br>");
  // client.println("<input type='submit' value='Adicionar Horário'>");
  // client.println("</form>");
  // client.println("</div>");
  // client.println("</body></html>");

  // delay(100);
  // client.stop();
  Serial.println("Client disconnected");
}

void updateDisplay() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CAT FOOD");

  display.setTextSize(2);
  display.setCursor(0, 18);
  display.print(formatDigits(timeClient.getHours()));
  display.print(":");
  display.print(formatDigits(timeClient.getMinutes()));
  display.print(":");
  display.println(formatDigits(timeClient.getSeconds()));

  // Exibir horário definido pelo usuário
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Hora definida: ");
  display.print(formatDigits(targetHour));
  display.print(":");
  display.print(formatDigits(targetMinute));

  display.setCursor(0, 54);
  display.print("IP: ");
  display.println(WiFi.localIP());

  display.display();
}

void executeScheduledAction() {
  if (motorActivated && timeClient.getHours() == targetHour && timeClient.getMinutes() == targetMinute) {
    motor.write(EXECUTION_ANGLE); 
    motorActivated = false;
    delay(DELAY_AFTER_EXECUTION); 
   
    motor.write(INITIAL_ANGLE); 
    delay(DELAY_BEFORE_RESTART); 
  }
}

String formatDigits(int digits) {
  if (digits < 10) {
    return "0" + String(digits);
  }
  return String(digits);
}
