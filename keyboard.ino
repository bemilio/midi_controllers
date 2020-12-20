//definisco i pin come costanti
const int riga1 = 5;
const int riga2 = 6;
const int riga3 = 7;
const int riga4 = 8;
const int clock = 13;
const int latch = 12;
const int data = 11;
const int led = 13;
//il canale è costante
const int comando = 0x90;
//vettore di comandi da mandare al registro
int colonna[] = {B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000};
int i=0;
boolean chkbutton[8][4];
int premo[8][4];
int j;
int a;
//RICORDA: nota 60 = Do 4 (do centrale)

void setup(){
  Serial.begin (31250);
  pinMode (riga1, INPUT);
  pinMode (riga2, INPUT);
  pinMode (riga3, INPUT);
  pinMode (riga4, INPUT);
  pinMode (clock, OUTPUT);
  pinMode (latch, OUTPUT);
  pinMode (data, OUTPUT);
  pinMode (led, OUTPUT);
  /*definisco la corrispondenza matrice-note, devo farlo in due blocchi separati perchè la matrice è sballata, vedi quaderno.
 ATTENZIONE: cambiando tastiera devi cambiare questa sezione!*/ 
 for(a = 0; a<4; a++)
   {
     for(j = 0; j<8; j++) 
      {
      premo[j][a]=67+j+8*a;
      chkbutton[j][a]= false;
      }
     premo[0][a]=67+8+(8*(a));
   }
  premo[6][3]=65;
  premo[7][3]=66;
  premo[0][3]=67;
  delay (1000);
}
void loop (){
  if (i==8)
    i=0;
  digitalWrite(clock,LOW);
  digitalWrite(latch,LOW);
  shiftOut(data, clock, MSBFIRST, colonna[i]);
  digitalWrite(latch,HIGH);
  scanriga_on(riga1, 0, i);
  scanriga_on(riga2, 1, i);
  scanriga_on(riga3, 2, i);
  scanriga_on(riga4, 3, i);
  scanriga_off(riga1, 0, i);
  scanriga_off(riga2, 1, i);
  scanriga_off(riga3, 2, i);
  scanriga_off(riga4, 3, i);
  i++; 
}  
//funzione per mandare la nota midi. veloc=127 quando accendi la nota, veloc=0 per spegnerla
void notaOn(int nota, int veloc){
  Serial.write(comando);
  Serial.write(nota);
  Serial.write(veloc);
}
//controlla se la riga ha un valore alto, in caso accende la nota
void scanriga_on(int riga_n, int r, int c){
    if (digitalRead(riga_n)==HIGH && !chkbutton[c][r]){
      notaOn(premo[c][r],127);
      chkbutton[c][r]=true;
    }
}
//uguale a prima, ma la spegne
void scanriga_off(int riga_n, int r, int c){
    if (digitalRead(riga_n)==LOW && chkbutton[c][r]){
    notaOn(premo[c][r],0);
    chkbutton[c][r]=false;
   }
}
