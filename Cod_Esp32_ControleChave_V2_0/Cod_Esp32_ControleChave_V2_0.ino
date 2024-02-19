/*iG Automacao - Código para enviar mensagem via Whats App com ESP32 usando API CallMeBot

1.Adicione o número de telefone +34 644 81 58 78 aos seus Contatos Telefônicos.(Nomeie como desejar);
2.Envie a seguinte mensagem: ( I allow callmebot to send me messages ) para o novo Contato criado (utilizando o WhatsApp é claro);
3.Aguarde até receber a mensagem “ API ativada para o seu número de telefone. Sua APIKEY é XXXXXX ” do bot.
*/

#include <WiFi.h>    
#include <HTTPClient.h>
#include <UrlEncode.h>
#define RXp2 16
#define TXp2 17
//========================  EDITAR APENAS ESTES CAMPOS COM SUAS CREDENCIAIS =====================================================
const char* ssid = "nomde wifi";
const char* password = "senha-wifi";

/*O número de telefone deve estar em formato internacional (incluindo o+sinal). EX: +553173539925
o numero nao deve conter o 9 na frente */
String phoneNumber = "+553177777777";
String apiKey = "1234567";  //chave API recebida no whatsApp

//================================================================================================================================

// Esta função aceita como argumento a mensagem que você deseja enviar.
void enviaMensagem(String message){
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);   
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpResponseCode = http.POST(url);
  delay(800);  //aguarda a API retornar com o codigo de resposta
  if (httpResponseCode == 200){
    Serial.print("WhatsApp - Mensagem enviada com sucesso!");
  }
  else{
    Serial.println("WhatsApp - Erro ao enviar mensagem");
    Serial.print("WhatsApp - HTTP resposta codigo: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1,RXp2,TXp2);

//Conecte-se à sua rede local e imprima o endereço IP da placa.
  WiFi.begin(ssid, password);
  Serial.println("Conectando");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado ao Wifi com o Endereço IP: "); 
  Serial.println(WiFi.localIP());

  /*Então, podemos enviar uma mensagem para o WhatsApp simplesmente chamando a função sendMessage(). Neste caso, estamos enviando a mensagem Olá do ESP32! */
  enviaMensagem("🤖   iG Automacao   🤖\n\n🔏Controle de Acesso das chaves\n\n🔑Chave 1 :Sala  de Documentos\n🔑Chave 2 :Sala Reunião\n🔑Chave 3 :Depósito dos Materiais");
  //enviaMensagem("ESP32 Ligado...");
}

void loop() {
  String mensagem ="";
  if (Serial2.available() > 0) {
    //Serial.print("\nESP32 - Mensagem recebida do Arduino: ");
    mensagem = Serial2.readString();

    String nomeProcurado = "Dados:";
    int posicaoInicial = mensagem.indexOf(nomeProcurado);
    if(posicaoInicial != -1){
      //Extrai a partir da posicao Dados:
      String mensagemFormatada = mensagem.substring(posicaoInicial + nomeProcurado.length());
      //Serial.println("Texto apos o nome: ");
      //Serial.println(mensagemFormatada);
      enviaMensagem(mensagemFormatada);
    }else{
      Serial.println("Nome nao encontrado na mensagem.");
    }
  }
  delay(100);
}