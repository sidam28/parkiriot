//ESP01-Arduino Uno Web server test
// www.arducoding.com
#include <SoftwareSerial.h>
#include <Servo.h>
#define DEBUG true

SoftwareSerial espSerial(2, 3); // TX | RX
Servo myservo;
int ldr = 0, ldr1 = 0, ldr2 = 0, ldr3 = 0, ldr4 = 0, ldr5 = 0, ldr6 = 0;
long jarak = 0; 
String  parkir = "",lampu = "mati", buser = "mati";


void setup()
{
  Serial.begin(9600);      //Serial monitor 
  espSerial.begin(115200);  //ESP Baud rate

  
  sendData("AT+RST\r\n",2000,DEBUG);    // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG);   // ESP01 sebagai access point
  sendData("AT+CIFSR\r\n",1000,DEBUG);    // Cek ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG);   // Konfig multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); //server on port 80
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(9, OUTPUT); //led
  digitalWrite(9, LOW);
  pinMode(10, OUTPUT); //pTring
  pinMode(11, INPUT);  //pEcho
  pinMode(13, OUTPUT); // buser
  
  
}

long readHC()
{
  digitalWrite(10, LOW);
  delayMicroseconds(2);
  digitalWrite(10, HIGH);
  delayMicroseconds(10);
  digitalWrite(10, LOW);
  return pulseIn(11, HIGH);
}

 

int Id_koneksi;
void loop(){

  
  jarak = 0.01723 * readHC();
  
  

  ldr = analogRead(A0);
  ldr1 = analogRead(A1);
  ldr2 = analogRead(A2);
  ldr3 = analogRead(A3);
  ldr4 = analogRead(A4);
  ldr5 = analogRead(A5);
  ldr6 = analogRead(A6);


  Serial.println(jarak);
//cek parkir
  if(ldr1 < 200 && ldr2 < 200 &&  ldr3 < 200 &&  ldr4 < 200 &&  ldr5 < 200 &&  ldr6 < 200){
    noTone(13);
    buser="diam";
    parkir="penuh";
    if(jarak<10) {
    tone(13, 1000);
    buser="berbunyi";
    }
  }else{
    noTone(13);
    buser="mati";
    parkir="ready";
  }
  

// menyalakan lampu
  if(ldr<200){
    digitalWrite(9, HIGH);
    lampu="nyala";
  }
  else{
    digitalWrite(9, LOW);  
    lampu="mati";
  }
  delay(300);
  if(espSerial.available()){
  //kirim data ke browser
  if(espSerial.find("+IPD,")){
    delay(300);
    Id_koneksi = espSerial.read()-48;
    
    String webpage;
    webpage = "HTTP/1.1 200 OK\r\n";
    webpage += "Content-Type: text/html\r\n";
    webpage += "Connection: close\r\n";
    webpage += "Refresh: 10\r\n"; //refresh browser 15 detik
    webpage += "\r\n";
    webpage += "<!DOCTYPE HTML>\r\n";
    webpage += "<html>\r\n";
    webpage +="<h1>Monitor Parkir</h1>";
    espsend(webpage);


   
    String output="";
    output+="<p>Parkir : ";
    output+=parkir;
    output+="</p>";
    output+="<p>Buser: ";
    output+=buser;
    output+="</p>";
    output+="<p>lampu: ";
    output+=lampu;
    output+="</p><br>";
    
    espsend(output);

    
     String div_ldr="<p>Parkir><p>";
    
     String div_ldr_data ="";
     
     div_ldr+="<p>1: ";
     if(ldr1<200){
      div_ldr+="diisi";
     }
     else{
      div_ldr+= "kosong";
     }
     div_ldr+="</p><p>2: ";
     if(ldr2<200){
      div_ldr+="diisi";
     }
     else{
      div_ldr+= "kosong";
     }
     div_ldr+="</p><p>3: ";
     if(ldr3<200){
      div_ldr+="diisi";
     }
     else{
      div_ldr+= "kosong";
     }
     espsend(div_ldr);
     
     String div_ldr1="</p><p>4: ";
     if(ldr4<200){
      div_ldr1+="diisi";
     }
     else{
      div_ldr1+= "kosong";
     }
     div_ldr1+="</p><p>5: ";
     if(ldr5<200){
      div_ldr1+="diisi";
     }
     else{
      div_ldr1+= "kosong";
     }
     div_ldr1+="</p><p>6: ";
     if(ldr6<200){
      div_ldr1+="diisi";
     }
     else{
      div_ldr1+= "kosong";
     }
     div_ldr1+="</p>";


     
     espsend(div_ldr1);

     String webpage2;
     webpage2="</body></html>";
     espsend(webpage2);
      
     String closeCommand = "AT+CIPCLOSE=";  //menutup koneksi 
     closeCommand+=Id_koneksi; 
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
  }
}
 
//kirim data ESP ke halaman web browser
 void espsend(String d){
   String cipSend = " AT+CIPSEND=";
   cipSend += Id_koneksi; 
   cipSend += ",";
   cipSend +=d.length();
   cipSend +="\r\n";
   sendData(cipSend,1000,DEBUG);
   sendData(d,1000,DEBUG); 
}

//menampilkan data dari ESP di Serial Monitor         
String sendData(String command, const int timeout, boolean debug){
  String response = "";
  espSerial.print(command);
  long int time = millis();
  while( (time+timeout) > millis()){
     while(espSerial.available()){
       char c = espSerial.read(); 
       response+=c;
    }  
  }
  
  if(debug){
    Serial.print(response); //Tampilkan respon ESP di serial monitor arduino
  }
  return response;
}
