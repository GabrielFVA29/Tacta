#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
SoftwareSerial softSerial(/*rx =*/10, /*tx =*/11);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini mp3;

// BOTÕES
const int BOTAO_INICIAL = 2;
const int BOTAO_1 = 3;
const int BOTAO_2 = 4;
const int BOTAO_3 = 5;

// MOTOR VIBRACALL
const int MOTOR = 9;

// NÍVEIS
int nivelAtual = 0;
// 0 = fácil
// 1 = médio
// 2 = difícil

int tentativas = 0;
const int MAX_TENTATIVAS = 2;

int vibracaoInicial;
int opcoes[3];

void setup() {
  pinMode(BOTAO_INICIAL, INPUT_PULLUP);
  pinMode(BOTAO_1, INPUT_PULLUP);
  pinMode(BOTAO_2, INPUT_PULLUP);
  pinMode(BOTAO_3, INPUT_PULLUP);

  pinMode(MOTOR, OUTPUT);
  analogWrite(MOTOR, 255);

  Serial.begin(9600);
  delay(1500);
  FPSerial.begin(9600);
}

void loop() {
  if (!mp3.begin(FPSerial)) {
    Serial.println("Erro ao iniciar DFPlayer Mini");
    while (true);
  }
  
  // analogWrite(MOTOR, 255);

  mp3.volume(25);

  randomSeed(analogRead(A0));

  tocarAudio("a");
  // 0001.mp3 -> Bem-vindo ao Tacta. Aperte o botão inicial para começar.

  esperarBotaoInicial();

  tocarAudio(2);
  // 0002.mp3 -> Você ouvirá uma vibração e depois três opções de resposta.

  tocarAudio(3);
  // 0003.mp3 -> Após ouvir as opções, escolha a correta usando os botões 1, 2 ou 3.

  tocarAudio(4);
  // 0004.mp3 -> Clique novamente no botão inicial para confirmar entendimento.

  esperarBotaoInicial();

  iniciarJogo();

  tocarAudio(1);
  // Depois do jogo, volta para a mensagem inicial.
}

void iniciarJogo() {
  nivelAtual = 0;

  while (nivelAtual < 3) {
    tentativas = 0;
    bool acertouNivel = false;

    while (tentativas < MAX_TENTATIVAS && acertouNivel == false) {
      acertouNivel = jogarNivel(nivelAtual);

      if (acertouNivel == false) {
        tentativas++;

        if (tentativas < MAX_TENTATIVAS) {
          tocarAudio(14);
          // 0014.mp3 -> Você errou. Tente novamente.
        } else {
          tocarAudio(15);
          // 0015.mp3 -> Você errou novamente. Fim de jogo.
          return;
        }
      }
    }

    if (acertouNivel == true) {
      nivelAtual++;

      if (nivelAtual < 3) {
        tocarAudio(16);
        // 0016.mp3 -> Você passou para o próximo nível.
      }
    }
  }

  tocarAudio(17);
  // 0017.mp3 -> Parabéns, você venceu o jogo Tacta.
}

bool jogarNivel(int nivel) {
  if (nivel == 0) {
    tocarAudio(5);
    // 0005.mp3 -> Nível fácil.
  } else if (nivel == 1) {
    tocarAudio(6);
    // 0006.mp3 -> Nível médio.
  } else {
    tocarAudio(7);
    // 0007.mp3 -> Nível difícil.
  }

  tocarAudio(8);
  // 0008.mp3 -> Três, dois, um.

  delay(500);

  vibracaoInicial = random(0, 3);

  tocarAudio(9);
  // 0009.mp3 -> Vibração inicial.

  tocarVibracao(nivel, vibracaoInicial);

  delay(1000);

  prepararOpcoes();

  tocarAudio(10);
  // 0010.mp3 -> Opções de resposta.

  tocarAudio(11);
  // 0011.mp3 -> Um.
  tocarVibracao(nivel, opcoes[0]);

  delay(700);

  tocarAudio(12);
  // 0012.mp3 -> Dois.
  tocarVibracao(nivel, opcoes[1]);

  delay(700);

  tocarAudio(13);
  // 0013.mp3 -> Três.
  tocarVibracao(nivel, opcoes[2]);

  delay(700);

  tocarAudio(18);
  // 0018.mp3 -> Responda agora.

  int resposta = esperarResposta();

  if (opcoes[resposta] == vibracaoInicial) {
    tocarAudio(19);
    // 0019.mp3 -> Você acertou.
    return true;
  } else {
    tocarAudio(20);
    // 0020.mp3 -> Você errou.
    return false;
  }
}

void prepararOpcoes() {
  opcoes[0] = 0;
  opcoes[1] = 1;
  opcoes[2] = 2;

  for (int i = 0; i < 3; i++) {
    int sorteado = random(0, 3);

    int temp = opcoes[i];
    opcoes[i] = opcoes[sorteado];
    opcoes[sorteado] = temp;
  }
}

int esperarResposta() {
  while (true) {
    if (digitalRead(BOTAO_1) == LOW) {
      esperarSoltar(BOTAO_1);
      return 0;
    }

    if (digitalRead(BOTAO_2) == LOW) {
      esperarSoltar(BOTAO_2);
      return 1;
    }

    if (digitalRead(BOTAO_3) == LOW) {
      esperarSoltar(BOTAO_3);
      return 2;
    }
  }
}

void esperarBotaoInicial() {
  while (digitalRead(BOTAO_INICIAL) == HIGH) {
    // Espera apertar o botão inicial
  }

  delay(200);

  while (digitalRead(BOTAO_INICIAL) == LOW) {
    // Espera soltar o botão
  }

  delay(200);
}

void esperarSoltar(int botao) {
  delay(200);

  while (digitalRead(botao) == LOW) {
    // Espera soltar o botão
  }

  delay(200);
}

void tocarVibracao(int nivel, int tipo) {
  if (nivel == 0) {
    vibracaoFacil(tipo);
  } else if (nivel == 1) {
    vibracaoMedia(tipo);
  } else {
    vibracaoDificil(tipo);
  }
}

void vibracaoFacil(int tipo) {
  if (tipo == 0) {
    vibrar(900);
  }

  if (tipo == 1) {
    vibrar(350);
    pausa(300);
    vibrar(350);
  }

  if (tipo == 2) {
    vibrar(250);
    pausa(250);
    vibrar(250);
    pausa(250);
    vibrar(250);
  }
}

void vibracaoMedia(int tipo) {
  if (tipo == 0) {
    vibrar(600);
    pausa(200);
    vibrar(300);
  }

  if (tipo == 1) {
    vibrar(300);
    pausa(200);
    vibrar(600);
  }

  if (tipo == 2) {
    vibrar(300);
    pausa(150);
    vibrar(300);
    pausa(150);
    vibrar(450);
  }
}

void vibracaoDificil(int tipo) {
  if (tipo == 0) {
    vibrar(220);
    pausa(120);
    vibrar(180);
    pausa(120);
    vibrar(220);
  }

  if (tipo == 1) {
    vibrar(180);
    pausa(120);
    vibrar(220);
    pausa(120);
    vibrar(180);
  }

  if (tipo == 2) {
    vibrar(200);
    pausa(100);
    vibrar(200);
    pausa(180);
    vibrar(200);
  }
}

void vibrar(int tempo) {
  analogWrite(MOTOR, 255);
  delay(tempo);
  analogWrite(MOTOR, 0);
}

void pausa(int tempo) {
  analogWrite(MOTOR, 0);
  delay(tempo);
}

void tocarAudio(int numero) {
  mp3.playMp3Folder(numero);
  delay(4000);
}
