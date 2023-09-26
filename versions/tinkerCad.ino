// Creditos a "ashey_chan" por disponibilizar o modelo de toque de "megalovania"
// Código base de "megalovania": https://www.tinkercad.com/things/4PYCSayLHq9

struct HighScore {
  int score;
};

enum GameState {
  START_GAME,
  PLAY_LEVEL,
  GAME_OVER,
  PLAY_AGAIN,
  END_GAME,
  BAD_TIME
};

const int MAX_HIGH_SCORES = 5;
const int BUZZER_PIN[2] = {12, 13};
const int BUTTONS[5] = {6, 5, 4, 3, 2};
const int LED_PINS[4] = {10, 9, 8, 7};
const int BUT_VALUES[5] = {1, 2, 3, 4, 5};
const int FREQUENCIA[4] = {500, 350, 400, 300};
const int delayCycle[10]={30, 30, 190, 380, 190, 190, 190, 30, 30, 55};
const bool piscaBuzzer[10]={false, true, true, false, true, true, false, true, true, false};

const int buzzer1[8][10]={
    {294, 294, 587, 440, 415, 392, 349, 294, 349, 392},
    {262, 262, 587, 440, 415, 392, 349, 294, 349, 392},
    {233, 233, 587, 440, 415, 392, 349, 294, 349, 392},
    {220, 220, 587, 440, 415, 392, 349, 294, 349, 392},
    {649, 649, 1309, 978, 922, 870, 870, 774, 870, 922},
    {617, 617, 1309, 978, 922, 870, 870, 774, 870, 922},
    {588, 588, 1309, 978, 922, 870, 870, 774, 870, 922},
    {675, 675, 1309, 978, 922, 870, 870, 774, 870, 922}
};

const int buzzer2[4][10]={
    {194, 194, 194, 194, 194, 194, 194, 194, 194, 194},
    {162, 162, 162, 162, 162, 162, 162, 162, 162, 162},
    {133, 133, 133, 133, 133, 133, 133, 133, 133, 133},
    {120, 120, 120, 120, 120, 150, 150, 170, 170, 190}
};

const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const int BUT_POSI = sizeof(BUTTONS) / sizeof(BUTTONS[0]);
const int NUM_BUZZ = sizeof(BUZZER_PIN) / sizeof(BUZZER_PIN[0]);

const long SERIAL_TIMEOUT = 10000;

int sequencia;
int tempoPisca;
int level = 1;
String recebido;
String sequenciaNumerica = "";
GameState stateGame = START_GAME;
HighScore highScores[MAX_HIGH_SCORES];

void setup() {
  Serial.begin(9600);
  for(int i = 0; i < NUM_LEDS; i++){
    pinMode(LED_PINS[i], OUTPUT);
  }
  for(int i = 0; i < BUT_POSI; i++){
    pinMode(BUTTONS[i], INPUT_PULLUP);
    Serial.println(BUTTONS[i]);
  }
  for(int i = 0; i < NUM_BUZZ; i++){
    pinMode(BUZZER_PIN[i], OUTPUT);
  }
  randomSeed(analogRead(0));
}

void loop() {
  switch (stateGame) {
    case START_GAME:
      comeca_jogo();
      break;
    case PLAY_LEVEL:
      nivel();
      break;
    case GAME_OVER:
      gameover();
      break;
    case PLAY_AGAIN:
      aplicaHighScore();
      playagain();
      break;
    case END_GAME:
      exit(0);
  }
}

void comeca_jogo(){
  nivelDificuldade();
  if (stateGame == BAD_TIME){
    delay(1000);
    megalovania();
    stateGame = PLAY_AGAIN;
  }
  else{
    stateGame = PLAY_LEVEL;
    Serial.println("Jogo comecando...");
    piscaTocaLed(250, 1, 600);
    delay(400);
  }
}

void templateDificuldade (){
  Serial.println("* INICIO *");
  Serial.println("Escolha o nivel:");
  Serial.println("(1) Jardim de Infancia");
  Serial.println("(2) Jovem Dinamico");
  Serial.println("(3) Mestre Shifu");
  Serial.println("(4) O Escolhido");
  Serial.println("(5) You're gonna have a bad time...");
}

void nivelDificuldade(){
  templateDificuldade ();

  while (true) {
    leResposta();
    Serial.println(recebido);
    if (recebido.equals("1")) {
      tempoPisca = 700;
      Serial.println("Selecionado: Jardim de Infancia");
      break;

    } else if (recebido.equals("2")) {
      tempoPisca = 500;
      Serial.println("Selecionado: Jovem Dinamico");
      break;

    } else if (recebido.equals("3")) {
      tempoPisca = 250;
      Serial.println("Selecionado: Mestre Shifu");
      break;

    } else if (recebido.equals("4")) {
      tempoPisca = 125;
      Serial.println("Selecionado: O Escolhido");
      break;

    } else if (recebido.equals("5")) {
      stateGame = BAD_TIME;
      Serial.println("Selecionado: You're gonna have a bad time...");
      break;

    } else {
      Serial.println("Parece que voce nao me entendeu");
      Serial.println("Vai para o Jardim de Infancia");
      tempoPisca = 700;
      break;
    }
  }
}

void piscaTocaLed(int tempo, int vezes, int frequencia) {
  for (int i = 0; i < vezes; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(LED_PINS[j], HIGH);
      tone(BUZZER_PIN[0], frequencia);
    }
    delay(tempo);
    for (int j = 0; j < NUM_LEDS; j++) {
      digitalWrite(LED_PINS[j], LOW);
      noTone(BUZZER_PIN[0]);
    }
    delay(tempo);
  }
}

void toqueGenerico(int ledId, int buzzer, int frequencia, int tempo){
  digitalWrite(LED_PINS[ledId], HIGH);
  tone(BUZZER_PIN[buzzer], frequencia);
  delay(tempo);
  digitalWrite(LED_PINS[ledId], LOW);
  noTone(BUZZER_PIN[buzzer]);
  delay(tempo);
}

void nivel(){
  Serial.print("* Nivel "); Serial.print(level); Serial.println(" *");
  geraSequencia(tempoPisca, level);
  leResposta();
  if (recebido.equals(sequenciaNumerica)){
    vitoria();
  } else {
    derrota();
  }
}

void geraSequencia (int tempo, int numLEDs){
  int ordemLeds[numLEDs];

  for (int i = 0; i < numLEDs; i++){
    ordemLeds[i] = random(1, NUM_LEDS + 1);
  }
  sequenciaNumerica = "";

  for (int j = 0; j < numLEDs; j++){
    int ledIndex = ordemLeds[j] - 1;
    toqueGenerico(ledIndex, 0, FREQUENCIA[ledIndex], tempo);
    sequenciaNumerica += String(ordemLeds[j]);
  }
}

void leResposta(){
  Serial.println("* Insira sua resposta *");
  long startTime = millis();
  recebido = "";
  
  while (millis() - startTime < SERIAL_TIMEOUT){
    if (Serial.available()) {
      recebido = Serial.readString();
      break;

    } else {
      recebido += verificaBotoes();
      if (recebido.length() == level && recebido.length() > 0) {
        break;
      }
    }
  }
}

String verificaBotoes() {
  String sequencia = "";
  
  for (int i = 0; i < BUT_POSI; i++) {
    int buttonState = digitalRead(BUTTONS[i]);
    if (buttonState == HIGH) {
      if (i == 4){
        toqueGenerico(3, 1, buzzer2[3][0], 300);
      } else {
        toqueGenerico(i, 0, FREQUENCIA[i], 300);
      }
      sequencia += String(BUT_VALUES[i]);
    }
  }
  if (sequencia != ""){
    return sequencia;
  }
}


void vitoria() {
  Serial.println("Parabens! Voce ganhou.");
  String playerName = "";
  level++;
  piscaTocaLed(175, 3, 750);
  delay(500);
}

void derrota() {
  stateGame = GAME_OVER;
  Serial.println("Errooooo!!!");
  delay(250);
}

void gameover() {
  Serial.println("Game Over");
  piscaTocaLed(200, 4, 200);
  stateGame = PLAY_AGAIN;
}

void playagain(){
  long startTime = millis();
  recebido = "";
  
  Serial.println("");
  Serial.println("Jogar novamente? (s/n)");
  
  while (millis() - startTime < SERIAL_TIMEOUT){
    if (Serial.available()) {
      recebido = Serial.readString();
      break;
    } else {
      recebido += verificaBotoes();
      if (recebido.length() > 0) {
        break;
      }
    }
  }

  if (recebido.equalsIgnoreCase("s") || recebido.equalsIgnoreCase("1")){
    Serial.println("S");
    restart();
  } else {
    Serial.println("N");
    end();
  }

}

void restart() {
  level = 1;
  stateGame = START_GAME;
  Serial.println("");
}

void end() {
  stateGame = END_GAME;
  Serial.println("");
  Serial.println("Jogo encerrado");
  piscaTocaLed(450, 1, 200);
  piscaTocaLed(450, 1, 150);
  piscaTocaLed(450, 1, 100);
  Serial.println("");
}

void aplicaHighScore(){
  highScores[MAX_HIGH_SCORES - 1].score = level - 1;
  sortHighScores();
  displayHighScores();
}

void sortHighScores() {
  for (int i = 0; i < MAX_HIGH_SCORES - 1; i++) {
    for (int j = i + 1; j < MAX_HIGH_SCORES; j++) {
      if (highScores[i].score < highScores[j].score) {
        HighScore temp = highScores[i];
        highScores[i] = highScores[j];
        highScores[j] = temp;
      }
    }
  }
}

void displayHighScores() {
  Serial.println("Maiores Pontuacoes:");
  for (int i = 0; i < MAX_HIGH_SCORES; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(" - ");
    Serial.println(highScores[i].score);
  }
}

void megalovania (){
  apresentacao();
  delay(700);
  ciclo1();
  ciclo2();
  ciclo3();
}

void apresentacao(){
  String fala = "Esta um belo dia la fora...";
  for (int i = 0; i < fala.length(); i++) {
    char letra = fala[i];
    Serial.print(letra);
    if (i%2 == 0) {
      toqueGenerico(3, 1, buzzer2[3][0], 50);
    } else {
      toqueGenerico(0, 1, buzzer2[3][0], 50);
    }
    if (letra == ' ') {
      delay(100);
    } else {
      delay(40);
    }
  }
  Serial.println("");
}

void ciclo1(){
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 10; j++){
        if (piscaBuzzer[j]){
          toquesCiclo1(3, buzzer1[i][j], j);
        } else{
          toquesCiclo1(0, buzzer1[i][j], j);
        }
      }
    }
}

void ciclo2(){
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 10; j++){
        if (piscaBuzzer[j]) {
          toquesCliclo2_3(3, 2, buzzer1[i][j], buzzer2[i][j], j);
        } else {
          toquesCliclo2_3(0, 1, buzzer1[i][j], buzzer2[i][j], j);
        }  
      }
    }
}

void ciclo3(){
    for (int i = 4; i < 8; i++){
      for (int j = 0; j < 10; j++){
        if (piscaBuzzer[j]) {
          toquesCliclo2_3(3, 1, buzzer1[i][j], buzzer2[i-4][j], j);
        } else {
          toquesCliclo2_3(0, 2, buzzer1[i][j], buzzer2[i-4][j], j);
        }
      }
    }
}

void toquesCiclo1(int ledId, int frequencia, int delayId){
  digitalWrite(LED_PINS[ledId], HIGH);
  tone(BUZZER_PIN[0], frequencia, 50); 
  delay(100);
  digitalWrite(LED_PINS[ledId], LOW);
  noTone(BUZZER_PIN[0]);
  delay(delayCycle[delayId]);
}

void toquesCliclo2_3(int ledId1, int ledId2, int frequencia1, int frequencia2, int delayId){
  digitalWrite(LED_PINS[ledId1], HIGH);
  digitalWrite(LED_PINS[ledId2], HIGH);
  tone(BUZZER_PIN[0], frequencia1, 50); delay(50); noTone(BUZZER_PIN[0]);
  tone(BUZZER_PIN[1], frequencia2, 50); delay(50); noTone(BUZZER_PIN[1]);
  digitalWrite(LED_PINS[ledId1], LOW);
  digitalWrite(LED_PINS[ledId2], LOW);
  delay(delayCycle[delayId]);
}

