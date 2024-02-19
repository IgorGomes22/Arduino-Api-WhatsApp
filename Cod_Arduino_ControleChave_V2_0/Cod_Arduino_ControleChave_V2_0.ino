//Programa : RFID - Controle de Acesso leitor RFID
//Autor : Igor Gomes Pereira

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 53                       
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN); 
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);    //Pinagem do Display LCD arduino MEGA

unsigned long tempoAtual = 0;
unsigned long tempoAnterior = 0;
const unsigned long intervaloEspera = 500;

//Pinagem do reles arduino MEGA
const int rele1Pin = 22,rele2Pin = 23,rele3Pin = 24;

bool estadoch1 = false,estadoch2 = false,estadoch3 = false; 

//Pinagem dos botoes arduino MEGA
int botao1 = 28;
int botao2 = 29;
int botao3 = 30; 

//variaveis de memoria
int lib1 = 0,lib2 = 0,lib3 = 0;   
int rele1PinState = 1;

//####################### incluir colaboradores nesta lista ############################
const int MAX_USUARIOS = 10; //numero maximo de colaboradores
struct Usuario{
  String uid;
  String nome;
};
Usuario usuarios[MAX_USUARIOS] = {
  {"A9 1B F5 B3", "Elon Musk"},
  {"B3 77 CA 11", "Albert Einstein"},
  {"E3 11 1C 13", "Isaac Newton"},
  {"B1 11 11 11", "Marie Curie"},
  {"C0 10 10 10", "Nikola Tesla"}
};
//#####################################################################################

void setup(){
  Serial.begin(9600); // Inicia a serial
  SPI.begin();        // Inicia  SPI bus
  mfrc522.PCD_Init(); // Inicia MFRC522
  lcd.begin(16, 2);   // Configura o número de colunas e linhas no LCD.

  // Imprime uma mensagem no LCD.
  lcd.print("      senai");

  pinMode (botao1 ,INPUT); 
  pinMode (botao2, INPUT); 
  pinMode (botao3, INPUT); 

  pinMode(rele1Pin, OUTPUT);
  pinMode(rele2Pin, OUTPUT);          
  pinMode(rele3Pin, OUTPUT);          
}
//======================================================================================

void loop(){
  botao1=digitalRead(28);
  botao2=digitalRead(29);
  botao3=digitalRead(30); 

  //Seleciona a chave que deseja retirar
  if (botao1) {
    tempoAtual = millis();
    if(tempoAtual - tempoAnterior >= intervaloEspera){
      escolha_chave("Chave 1","Aproxime a Tag"); 
      lib1=1;
      tempoAnterior = tempoAtual;
    }
  }
      
     
  if (botao2) {
    tempoAtual = millis();
    if(tempoAtual - tempoAnterior >= intervaloEspera){
      escolha_chave("Chave 2","Aproxime a Tag"); 
      lib2=1;
      tempoAnterior = tempoAtual;
    }
  }

  if (botao3) {
    tempoAtual = millis();
    if(tempoAtual - tempoAnterior >= intervaloEspera){
      escolha_chave("Chave 3","Aproxime a Tag"); 
      lib3=1;
      tempoAnterior = tempoAtual;
    }
  }

  //So ira ler o cracha caso aperte algum botao de selecao de chaves
  try{
  if((lib1 == 1) || (lib2 == 1) || (lib3 == 1)){
    // Procura por novos cartões
    if (!mfrc522.PICC_IsNewCardPresent()) {delay(500);return;}
    // Seleciona um dos cartões
    if (!mfrc522.PICC_ReadCardSerial()) {delay(500);return;}

    Serial.println();
    // Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo = "";
    byte letra;
    
    //Prepara os caracteres do cartao para hexadecimal
    for (byte i = 0; i < mfrc522.uid.size; i++){
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    Serial.println();

    conteudo.toUpperCase();
    //Realiza autenticacao do acesso para pessoas cadastradas
    int numUsuarios = sizeof(usuarios) / sizeof(usuarios[0]);

    //Verifique se o índice está dentro dos limites do array antes de acessá-lo 
    //para evitar possíveis estouros de buffer.
    if(numUsuarios <= 0 || numUsuarios > MAX_USUARIOS){
      Serial.println("Erro: Numero invalido de usuarios");
      return;
    }

    String uid = conteudo.substring(1);
    bool usuarioEncontrado = false;

    for ( int i = 0; i < numUsuarios; i++){
      if (uid.equals(usuarios[i].uid)) { 
        usuarioEncontrado = true;

        if((lib1 == 1) and (estadoch1 == false)){ 
          acesso_liberado("Chave 1", usuarios[i].nome, rele1Pin);
          estadoch1 = true;
        }else if ((lib1 == 1) and (estadoch1 == true)){
          devolveChave("Chave 1", usuarios[i].nome, rele1Pin);
          estadoch1 = false;
          }
        if((lib2 == 1) and (estadoch2 == false)){
          acesso_liberado("Chave 2", usuarios[i].nome, rele2Pin);
          estadoch2 = true;
          }else if((lib2 ==1) and (estadoch2 == true)){
            devolveChave("Chave 2", usuarios[i].nome, rele2Pin);
            estadoch2 = false;
          }
        if((lib3 == 1) and (estadoch3 == false)){ 
          acesso_liberado("Chave 3", usuarios[i].nome, rele3Pin);
          estadoch3 = true;
          }else if((lib3 == 1) and( estadoch3 == true)){
            devolveChave("Chave 3", usuarios[i].nome, rele3Pin);
            estadoch3 = false;
          }

        limpa_memoria();
        break; //sai do loop quando o usuario e encontrado
      }
    }
    if(!usuarioEncontrado){
      escolha_chave(" Cracha","   Sem Acesso");
      limpa_memoria();
      delay(2000);
      mensageminicial();
      return;
    }
    
  } //Fim if
}//try
catch(const char* e){
  Serial.println("Erro: "+ String(e));
}
  
  mensageminicial();
  limpa_memoria();
  delay(200);

}//Fim loop

void limpa_memoria(){
  lib1 = 0;
  lib2 = 0;
  lib3 = 0;    
}

void escolha_chave(String texto1, String texto2){
    delay(1000);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print(texto1);
    lcd.setCursor(0, 1);
    lcd.print(texto2);
 }

void acesso_liberado(String nomeChave, String nomeUsuario,int rele){
  String dadosEsp = "";
  dadosEsp = "Dados:⚠️Aviso:\nChave retirada: "+nomeChave+" \nUsuario: "+nomeUsuario;
  Serial.println("\n"+dadosEsp); //envia dados pro ESP32 via serial
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(nomeChave + "-Liberada");
  lcd.setCursor(0, 1);
  lcd.print(nomeUsuario);

  digitalWrite(rele, HIGH);
  delay(3000);
  mensageminicial();
}

void devolveChave(String nomeChave, String nomeUsuario,int rele){
  String dadosEsp = "";
  dadosEsp = "Dados:⚠️Aviso:\nChave devolvida: "+nomeChave+" \nUsuario: "+nomeUsuario;
  Serial.println("\n"+dadosEsp); //envia dados pro ESP32 via serial
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(nomeChave + "-Entregue");
  lcd.setCursor(0, 1);
  lcd.print(nomeUsuario);

  digitalWrite(rele, LOW);
  delay(3000);
  mensageminicial();
}

void mensageminicial(){
  lcd.clear();
  lcd.print("      senai");
}