/*Programma per avere, da 6 piezo in entrata, una batteria midi sensibile alla dinamica
in serie ai piezo: Diodo al germanio, condensatore da 47 nF in parallelo a una resistenza molto alta (4,7 MOhm) verso massa, azzera i condensatori con due 4066 
il programma legge le porte analogiche, e controlla quando superano la soglia.
dopo un ritardo per fare caricare i condensatori, rilegge quelli oltre la soglia ottenendo il valore di velocity e dà il comando di notaOn.
a fine ciclo scarica i condensatori con i 4066*/
#define ritardo 15
#define soglia1 20
#define soglia2 500
#define canale 1
#define kick 35 //per garageband
#define kick2 36
#define snare 38
#define tom1 48
#define tom2 47  
#define closedhh 42
#define openhh 46
#define crash 49
#define ride 51
#define correction 200
int pinControllo[6]={10, 9, 8, 7, 6, 5};
int piezo[6]={A0, A1, A2, A3, A4, A5};
unsigned long time[6]={0,0,0,0,0,0};
int lastnote[6]={0,0,0,0,0,0};
int sound[6]={snare, tom2, ride, closedhh, crash, kick2};
int nonbasicsound[6]={snare, tom2, ride, openhh, crash, kick2};//rosso, blu, verse, piattosx, piattodx, kick
boolean flag[6];
boolean flagGen;
int velocity, val;
int i,check;
byte comando=0x90;

void setup (){
  for(i=0;i<6;i++){
    pinMode(piezo[i], INPUT);
    pinMode (pinControllo[i], OUTPUT);
    digitalWrite (pinControllo[i], LOW);
  }
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
  Serial.begin(31250); //standard MIDI: 31250. Hairless: 19200 va bene
  while (!Serial); //aspetta che la porta seriale sia disponibile (almeno credo)
  comando = comando+canale; //imposta il comando di nota ON nel canale dato
}
void AccendiLed(){
    digitalWrite(13, HIGH);
    delay(ritardo);
    digitalWrite(13,LOW);
}
//PadCheck: in ingresso ha il pin, in uscita ha il valore cercato di AnalogRead
boolean PadCheck (int piez){
  int check;
  check=analogRead(piez);
  if (check>soglia1)
    return 1;
  else
    return 0;
}
void Azzeramento (int pin){
  digitalWrite(pin, HIGH);
  delay(ritardo);
  digitalWrite(pin, LOW);
}
      
void notaOn(int nota, byte veloc){
  Serial.write(comando);
  Serial.write(nota);
  Serial.write(veloc);
}
void notaOff(int nota){
  Serial.write(comando);
  Serial.write(nota);
  Serial.write(0);
}
void CheckTimeOff(unsigned long tempo[]){
  int k=0;
  unsigned long misura;
  misura= millis();
  for (k=0; k<6; k++){
    if (misura-tempo[k]>1500){
    notaOff(sound[k]);
    notaOff(nonbasicsound[k]);
    lastnote[k]=0;
    tempo[k]=millis();
    }
  }
}
boolean CheckTimeOn(int k, unsigned long tempo){
  unsigned long misura;
  misura=millis();
  int silent;
  if (i==3 || i==4)
    silent=100;
  else
    silent=70;  
  if(misura-tempo>silent)
    return 1;
  else
    return 0;
}

boolean CymbalCorrection (int k, int value, int lastvalue){  //ingresso: indice i, digitalread attuale, digitalread precedente
  if((k==3 || k==4) && (value<(lastvalue-correction))){
    Azzeramento(pinControllo[k]);
    return 0;
  }
  else
    return 1;
}
  
void loop(){
  CheckTimeOff(time);
  for(i=0;i<6;i++){
    flag[i]=PadCheck(piezo[i]);
  } 
  delayMicroseconds (300);//attesa di carica del condensatore. valori grandi: ritardo; valori piccoli: rischio di doppi colpi per "rimbalzo" del piezo. 
  //MEMO: analogRead impiega 100 microsecondi a leggere
  for(i=0;i<6;i++){
    if (flag[i]&&CheckTimeOn(i, time[i])){
      val=analogRead(piezo[i]);
        if (CymbalCorrection(i, val, lastnote[i])){
        flagGen=true;
        velocity=int((((long(val))*77)/1023)+50); //modificando attento all' OverFlow
        if (val<soglia2)
          notaOn(sound[i], velocity);
        else
          notaOn(nonbasicsound[i], velocity);
        Azzeramento(pinControllo[i]);
        lastnote[i]=val;
        time[i]=millis();
      }
    }
  } 
  if (flagGen){
    AccendiLed();
  }
  flagGen=false;
}


