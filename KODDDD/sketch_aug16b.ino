#include <AFMotor.h>      // Adafruit Motor Shield kütüphanesini ekle
#include <Servo.h>        // Servo Motor kütüphanesini ekle       
#include <NewPing.h>      // Ultrasonik sensör kütüphanesini ekle

#define TRIG_PIN A0 // Ultrasonik sensöre lehimlenmiş Motor Sürücü Korumasındaki Pin A0
#define ECHO_PIN A1 //Ultrasonik sensöre lehimlenmiş Motor Sürücü Korumasındaki Pim A1
#define MAX_DISTANCE 300 // maksimum kullanılabilir sensör ölçüm mesafesini 300cm'ye ayarlar
#define MAX_SPEED 160 // DC çekiş motorlarının hızını 150/250'ye veya tam hızın yaklaşık% 70'ine ayarlar - güç tüketimini azaltmak için.
#define MAX_SPEED_OFFSET 40 // bu, iki DC çekiş motoru arasındaki farklara izin vermek için ofseti ayarlar
#define COLL_DIST 30 // robotun durup 30 cm'ye döneceği mesafeyi belirler
#define TURN_DIST COLL_DIST+20 // robotun nesneden uzaklaştığı mesafeyi belirler
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // mesafeyi ölçmek için doğru pimleri kullanmak üzere sensör kütüphanesini ayarlar.


AF_DCMotor leftMotor1(1, MOTOR12_1KHZ); // Motor Sürücü Kalkanı üzerindeki M1 çıkışını kullanarak motor 1'i oluşturun, 1kHz PWM frekansına ayarlayın
AF_DCMotor leftMotor2(2, MOTOR12_1KHZ); // M2 çıkışı kullanarak motor # 2 oluşturun, 1kHz PWM frekansına ayarlayın
AF_DCMotor rightMotor1(3, MOTOR34_1KHZ);//M3 çıkışını kullanarak 1 kHz PWM frekansına ayarlanmış motor # 3 oluşturun
AF_DCMotor rightMotor2(4, MOTOR34_1KHZ);// M4 çıkışını kullanarak 1 kHz PWM frekansına ayarlanmış motor # 4 oluşturun
Servo myservo;  // bir servoyu kontrol etmek için servo nesnesi yarat 

int leftDistance, rightDistance; // her iki taraftaki mesafeleri
int curDist = 0;
String motorSet = "";
int speedSet = 0;

//-------------------------------------------- KURULUM DÖNGÜSÜ ----------------------------------------------------------------------------
void setup() { 
  myservo.attach(10);  // servoyu 10 pimine (Motor Sürücü Korumasındaki SERVO_1) servo nesnesine tutturur
  myservo.write(90); // servoya 90 derece pozisyon vermesini söyler yani. ileriye dönük.
  delay(1000); // bir saniye gecikme
 }
//------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------ANA DÖNGÜ ------------------------------------------------------------------------------
void loop() {
  myservo.write(90);  // gözleri ileri götür
  delay(90);
  curDist = readPing();   // okuma mesafesi
  if (curDist < COLL_DIST) {changePath();} // eğer ileri engellenirse yön değiştir
  moveForward();  // ileri git
  delay(500);
 }
//-------------------------------------------------------------------------------------------------------------------------------------

void changePath() { 
  moveStop();   // ileri hareketi durdur
  myservo.write(36); // sağa olan mesafeyi kontrol et
    delay(500);
    rightDistance = readPing();// doğru mesafeyi ayarla
    delay(500);
    myservo.write(144);  // sola dengesizliği kontrol et
    delay(700);
    leftDistance = readPing(); // sola mesafeyi ayarla
    delay(500);
    myservo.write(90); // merkeze dön
    delay(100);
    compareDistance();
  }

  
void compareDistance()   // en uzun mesafeyi bulun
{
  if (leftDistance>rightDistance) // eğer sol daha az tıkalı ise
  {
    turnLeft();
  }
  else if (rightDistance>leftDistance) // eğer hak daha az engellenirse
  {
    turnRight();
  }
   else // eğer eşit derecede tıkanmışlarsa
  {
    turnAround();
  }
}


//-------------------------------------------------------------------------------------------------------------------------------------

int readPing() { // ultrasonik sensör mesafesini oku
  delay(70);   
  unsigned int uS = sonar.ping();
  int cm = uS/US_ROUNDTRIP_CM;
  return cm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void moveStop() {leftMotor1.run(RELEASE); leftMotor2.run(RELEASE); rightMotor1.run(RELEASE); rightMotor2.run(RELEASE);}  // motorları durdur.
//-------------------------------------------------------------------------------------------------------------------------------------
void moveForward() { 
    motorSet = "FORWARD";
    leftMotor1.run(FORWARD);      // ilerlemeye devam et
    leftMotor2.run(FORWARD);      // ilerlemeye devam et
    rightMotor1.run(FORWARD);     // ilerlemeye devam et
    rightMotor2.run(FORWARD);     // ilerlemeye devam et
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // pilleri çok hızlı şekilde doldurmamak için hızı yavaşça yükseltin
  {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet); 
    rightMotor2.setSpeed(speedSet);
    delay(5);
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------
void moveBackward() { 
    motorSet = "BACKWARD";
    leftMotor1.run(BACKWARD);     // geriye doğru giderken çevir
    leftMotor2.run(BACKWARD);     // geriye doğru giderken çevir
    rightMotor1.run(BACKWARD);    // geriye doğru giderken çevir
    rightMotor2.run(BACKWARD);    // geriye doğru giderken çevir
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // pilleri çok hızlı şekilde doldurmamak için hızı yavaşça yükseltin
  {
    leftMotor1.setSpeed(speedSet);
    leftMotor2.setSpeed(speedSet);
    rightMotor1.setSpeed(speedSet); 
    rightMotor2.setSpeed(speedSet); 
    delay(5);
  }
}  
//-------------------------------------------------------------------------------------------------------------------------------------
void turnRight() { 
  motorSet = "RIGHT";
  leftMotor1.run(FORWARD);      // motor 1'i ileri çevir
  leftMotor2.run(FORWARD);      // motor 2'i ileri çevir
  rightMotor1.run(BACKWARD);    // motor 3'i ileri çevir
  rightMotor2.run(BACKWARD);    // motor 4'i ileri çevir
  rightMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);     
  delay(1500); // 1500 için motorları bu şekilde çalıştır        
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);     // her iki motoru da ileri sar
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);      
}  
//-------------------------------------------------------------------------------------------------------------------------------------
void turnLeft() { 
  motorSet = "LEFT";
  leftMotor1.run(BACKWARD);      // motor 1'i geriye doğru çevirin
  leftMotor2.run(BACKWARD);      // motor 2'i geriye doğru çevirin
  leftMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);     
  leftMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);    
  rightMotor1.run(FORWARD);    // motor 3'ü ileri çevir
  rightMotor2.run(FORWARD);     // motor 4'ü ileri çevir
  delay(1500); // run motors this way for 1500  
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);      // ilerlemeye devam et
  leftMotor2.run(FORWARD);      // ilerlemeye devam et
  rightMotor1.run(FORWARD);     // ilerlemeye devam et
  rightMotor2.run(FORWARD);     // ilerlemeye devam et
}  
//-------------------------------------------------------------------------------------------------------------------------------------
void turnAround() { 
  motorSet = "RIGHT";
  leftMotor1.run(FORWARD);      // motor 1'i ileri çevir
  leftMotor2.run(FORWARD);      // motor 2'i ileri çevir
  rightMotor1.run(BACKWARD);   // motor 3'ü geriye doğru çevirin
  rightMotor2.run(BACKWARD);  // motor 4'ü geriye doğru çevirin
  rightMotor1.setSpeed(speedSet+MAX_SPEED_OFFSET);      
  rightMotor2.setSpeed(speedSet+MAX_SPEED_OFFSET);
  delay(1700); // 1700 için motorları bu şekilde çalıştır
  motorSet = "FORWARD";
  leftMotor1.run(FORWARD);     // her iki motoru da ileri sar
  leftMotor2.run(FORWARD);
  rightMotor1.run(FORWARD);
  rightMotor2.run(FORWARD);      
}
