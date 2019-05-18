//указываем подключенные пины
#define analogInput0 0
#define analogInput1 1
#define relPin1 11
#define relPin2 12
#define ledPin1 2
#define ledPin2 3
#define ledPin3 4
#define ledPin4 5
#define ledPin5 6
#define butPin1 8
//период автовключения
unsigned const long checkPeriod=600000;
//по умолчанию система думает, что компы включены
bool pc1isOff=false;
bool pc2isOff=false;
//переменная для проверки, отправлена ли команда, что бы не было отправки 2 раза
bool pc1sentCommand=true;
bool pc2sentCommand=true;
//переменная для режима работы системы в атоматическом режиме
bool autoTurn=true;
//определяем, находимся ли мы в меню
bool inMenu=false;
//включаем автоматический режим для обоих компов
bool pc1notAuto=false;   
bool pc2notAuto=false;
//какой пункт меню выбран
int menuSelected=1;
bool ledState = HIGH;   
//сколько зажата кнопка
long millis_held;    
long secs_held;      
long prev_secs_held; 
byte previous = HIGH;
unsigned long firstTime;
unsigned long lastBlink;
//включениее первого компа
void pc1on(){
  digitalWrite(relPin1,LOW);
  delay(250);
  digitalWrite(relPin1,HIGH);
  pc1sentCommand=true;
  
}
//выключение компов, i - номер компа, k - сколько должна быть зажата кнопка
void turnOff(int i, int k){
  
  if (i==1){
    digitalWrite(relPin1,LOW);
    delay(k);
    digitalWrite(relPin1,HIGH);
    pc1sentCommand=true;
  }
  if (i==2){
    digitalWrite(relPin2,LOW);
    delay(k);
    digitalWrite(relPin2,HIGH);
    pc1sentCommand=true;
  }
}
//включение второго компа
void pc2on(){
  digitalWrite(relPin2,LOW);
  delay(250);
  digitalWrite(relPin2,HIGH);
  pc2sentCommand=true;
}
//проверка состояния компов, опытным путем установил, что если значение на пине
//меньше 90, значит можно считать, что питания нет
void checkPCs(){
  int value1=0;
  int value2=0;
  value1 = analogRead(analogInput0);
  value2 = analogRead(analogInput1);
  if (value1<90){
    pc1isOff=true;
    pc1sentCommand=false;
    digitalWrite(ledPin1, !pc1isOff);
  }
  else{
    pc1isOff=false;
    pc1sentCommand=true;
    digitalWrite(ledPin1, !pc1isOff);
  }
  if (value2<90){
    pc2isOff=true;
    digitalWrite(ledPin2, !pc2isOff);
    
    pc2sentCommand=false;
  }
  else{
    pc2isOff=false;
    pc2sentCommand=true;
    digitalWrite(ledPin2, !pc2isOff);
  }
}
//обработка нажатия на кнопку
void checkButton(){ 
  int current = digitalRead(butPin1);
  
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;

  if (millis_held > 50) {

   if (current == HIGH && previous == LOW) {
      if (secs_held < 3) {
        if (inMenu){
          if (menuSelected==1){
            menuSelected=2;
            digitalWrite(ledPin1, HIGH);
          }
          else{
            menuSelected=1;
            digitalWrite(ledPin2, HIGH);

          }
        }
        else{
          autoTurn=!autoTurn;
          digitalWrite(ledPin3, autoTurn);
        }
      }
      //если кнопка зажата больше 3х секунд - входим в меню, или выходим из него
      if (secs_held >= 3) {
        if (inMenu){
          if (menuSelected==1){
            pc1notAuto=!pc1notAuto;
            checkPCs();
            digitalWrite(ledPin4,!pc1notAuto);
            digitalWrite(ledPin1,!pc1isOff);
            digitalWrite(ledPin2,!pc2isOff);
          }
          if (menuSelected==2){
            pc2notAuto=!pc2notAuto;
            checkPCs();
            digitalWrite(ledPin5,!pc2notAuto);
            digitalWrite(ledPin2,!pc2isOff);
            digitalWrite(ledPin1,!pc1isOff);
          }
        }
        digitalWrite(ledPin3, autoTurn);
        inMenu=!inMenu;
      }
    }
  }
  previous = current;
  prev_secs_held = secs_held;
}
//управление морганием светодиодов
void ledControl(){
    unsigned long timeBlinking = millis();
    unsigned int interval=1000;
    if (inMenu){ interval=500;}
    if (timeBlinking - lastBlink >= interval) {
    lastBlink = timeBlinking;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  if (inMenu)
  {
    digitalWrite(ledPin3,ledState);
    if (menuSelected==1){
      digitalWrite(ledPin1,ledState);
    }
    if (menuSelected==2){
      digitalWrite(ledPin2,ledState);
    }
  }
}
}
//считываем данные введеные пользователем
void readSerial(){
  String str="";
  while(Serial.available()>0) {
      str=Serial.readStringUntil('\n');
      str.trim();
    }
  if (str.length()>0){
    writeSerial(str);
  }
}
//выводим ответы на введенные пользователем команды
void writeSerial(String str){
     if (str=="?" || str=="help")
    {
      Serial.println();
      Serial.println(F("status - status of PCs"));
      Serial.println(F("on1 - turn on 1st PC"));
      Serial.println(F("on2 = turn on 2nd PC"));
      Serial.println(F("off1 - turn off 1st PC (force button will be pressed for 8secs)"));
      Serial.println(F("off2 - turn off 2nd PC (force button will be pressed for 8secs)"));
      Serial.println(F("soff1 - turn off 1st PC (soft button will be pressed for 1250 millisecs)"));
      Serial.println(F("soff2 - turn off 2nd PC (soft button will be pressed for 1250 millisecs)"));
      Serial.println(F("auto1 - change auto mode for 1st PC"));
      Serial.println(F("auto2 - change auto mode for 2nd PC"));
      Serial.println(F("auto - change auto mode"));
      Serial.println(F("check - check PCs status immidiately"));
      Serial.println();
    }
    if (str=="status"){
      Serial.println();
      Serial.print(F("pc1 is on - "));
      Serial.println(!pc1isOff);
      Serial.print(F("pc2 is on - "));
      Serial.println(!pc2isOff);
      Serial.print(F("pc1 is in auto mode - "));
      Serial.println(!pc1notAuto);
      Serial.print(F("pc2 is in auto mode - "));
      Serial.println(!pc2notAuto);
      Serial.print(F("system is in auto mode - "));
      Serial.println(autoTurn);
      Serial.println();
    }
    if (str=="on1"){
      if(pc1isOff){ 
        pc1on();
        Serial.println();
        Serial.println(F("ok"));
        Serial.println();
        }
      else{
        Serial.println();
        Serial.println(F("pc already turned on"));
        Serial.println();
      }
    }
    if (str=="on2"){
      Serial.println();
      if(pc2isOff){ 
        pc2on();
        Serial.println(F("ok"));
        }
      else{
        Serial.println(F("pc already turned on"));
      }
      Serial.println();
    }
    if (str=="off1"){
      Serial.println();
        if (!pc1isOff){
          Serial.println(F("Turning PC1 off..."));
          turnOff(1,8000);
          Serial.println(F("OK"));
        }
        else {
          Serial.println(F("pc already turned off"));
      }
      Serial.println();
    }
    if (str=="off2"){
      Serial.println();
        if (!pc2isOff){
          Serial.println(F("Turning PC2 off..."));
          turnOff(2,8000);
          Serial.println(F("OK"));
        }
        else {
          Serial.println(F("pc already turned off"));
      }
      Serial.println();
    }
        if (str=="soff1"){
          Serial.println();
          if (!pc1isOff){
            turnOff(1,1500);
            Serial.println(F("Turning PC1 off..."));
            Serial.println(F("OK"));
          }
          else {
            Serial.println(F("pc already turned off"));
        }
        Serial.println();
    }
    if (str=="soff2"){
      Serial.println();
        if (!pc2isOff){
          Serial.println(F("Turning PC2 off..."));
          turnOff(2,1500);
          Serial.println(F("OK"));
        }
        else {
          Serial.println(F("pc already turned off"));
      }
      Serial.println();
    }
    if (str=="auto"){
      Serial.println();
      autoTurn=!autoTurn;
      digitalWrite(ledPin3, autoTurn);
      Serial.print(F("OK auto mode os now - "));
      Serial.println(autoTurn);
      Serial.println();
    }
    if (str=="auto1"){
      Serial.println();
      pc1notAuto=!pc1notAuto;
      digitalWrite(ledPin4,!pc1notAuto);
      Serial.print(F("OK, auto mode for PC1 is now - "));
      Serial.println(!pc1notAuto);
      Serial.println();
    }
    if (str=="auto2"){
      Serial.println();
      pc2notAuto=!pc2notAuto;
      digitalWrite(ledPin5,!pc2notAuto);
      Serial.print(F("OK, auto mode for PC2 is now - "));
      Serial.println(!pc2notAuto);
      Serial.println();
    }
    if (str=="check"){
      Serial.println();
      checkPCs();
      Serial.print(F("pc1 is on - "));
      Serial.println(!pc1isOff);
      Serial.print(F("pc2 is on - "));
      Serial.println(!pc2isOff);
      Serial.println();
    }
    str="";
}
//инициализация программы
void setup() {
  pinMode(analogInput0, INPUT);
  pinMode(analogInput1, INPUT);
  pinMode(relPin1, OUTPUT);
  pinMode(relPin2, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  //pinMode(butPin1, INPUT);
  digitalWrite(relPin1, HIGH);
  digitalWrite(relPin2, HIGH);
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  digitalWrite(ledPin3, HIGH);
  digitalWrite(ledPin4, HIGH);
  digitalWrite(ledPin5, HIGH);
  digitalWrite(butPin1,  HIGH);
  Serial.begin(115200);
  checkPCs();
}
//основной цикл
void loop(){
  checkButton();
  //моргаем светодиодами только ессл в меню
  if(inMenu){
    ledControl();
  }
  //считываем данные только если не в меню, иначе будет ощущение, что лагает, когда данные будут счиываться
  if(!inMenu){
    readSerial();
  }
  //раз в секунду проверяем статус компьютеров
  static unsigned long TimeWork1= 0;
  unsigned long delta1 = millis()-TimeWork1;
  if (delta1>=1000 && !inMenu){ 
    TimeWork1=millis();
    checkPCs();
  }
  //раз в 10минут включаем компьютеры, если они ыключены, не находимся в меню и всё в автоматическом режиме
  static unsigned long TimeWork = 0;
  unsigned long delta = millis()-TimeWork;
  if (delta>=checkPeriod && !inMenu){ 
    TimeWork=millis();
    if (autoTurn){
      if(!pc1sentCommand && pc1isOff && !pc1notAuto){
        pc1on();
      }
      if(!pc2sentCommand && pc2isOff && !pc2notAuto){
        pc2on();
      }
    }
  }
}
