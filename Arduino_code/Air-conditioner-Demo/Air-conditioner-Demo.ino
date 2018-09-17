/*Code được phát triển từ các bài sau:
 *+ Cách lập trình nhiều con LED 7 đoạn (loại đơn): http://arduino.vn/bai-viet/214-cach-lap-trinh-nhieu-con-led-7-doan-loai-don
 *+ Điều khiển bật tắt đèn LED từ xa thông qua NodeJS và WebSocket với thư viện SocketIO: http://arduino.vn/result/1449-dieu-khien-bat-tat-den-led-tu-xa-thong-qua-nodejs-va-websocket-voi-thu-vien-socketio
 *+ ESP8266 kết nối Internet - Phần 1: Cài đặt ESP8266 làm một socket client kết nối tới socket server trong mạng LAN: http://arduino.vn/bai-viet/1496-esp8266-ket-noi-internet-phan-1-cai-dat-esp8266-lam-mot-socket-client-ket-noi-toi
 *+ 
 */
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>


#define SSID "ThomasHo"
#define PASSWORD "36963696"
#define URL_AIR_CON "air-con"

const char* HOST_NAME =  "192.168.43.185";
const int PORT = 3000;

#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

/*
shiftOut ra 1 Module LED 7 đoạn đơn
*/
//chân ST_CP của 74HC595 - D
int latchPin = 4; //Chan D2 ESP-12E
//chân SH_CP của 74HC595
int clockPin = 10; //Chan SD3 ESP-12E
//Chân DS của 74HC595
int dataPin = 5; //Chan D1 ESP-12E

//Status LED: Standby - RED - Air conditioner STOP
const int sttStandby = 13; // Chân D7 - GPIO13
//Status LED: Standby - GREEN - Air conditioner RUN
const int sttActive = 12; // Chân D6 - GPIO12

// Ta sẽ xây dựng mảng hằng số với các giá trị cho trước
// Các bit được đánh số thứ tự (0-7) từ phải qua trái (tương ứng với A-F,DP)
// Vì ta dùng LED 7 đoạn chung cực dương nên với các bit 0
// thì các đoạn của LED 7 đoạn sẽ sáng
// với các bit 1 thì đoạn ấy sẽ tắt
 
//mảng có 10 số (từ 0-9) và 
const byte Seg[10] = {
  0b11000000,//0 - các thanh từ a-f sáng
  0b11111001,//1 - chỉ có 2 thanh b,c sáng
  0b10100100,//2
  0b10110000,//3
  0b10011001,//4
  0b10010010,//5
  0b10000010,//6
  0b11111000,//7
  0b10000000,//8
  0b10010000,//9
};
 
 
void setup() {
  //Bật baudrate ở mức 57600 để giao tiếp với máy tính qua Serial
  Serial.begin(57600);
  delay(5);
 
  //Việc đầu tiên cần làm là kết nối vào mạng Wifi
  Serial.print("Ket noi vao mang ");
  Serial.println(SSID);
 
  //Kết nối vào mạng Wifi
  WiFi.begin(SSID, PASSWORD);
 
  //Chờ đến khi đã được kết nối
  while (WiFi.status() != WL_CONNECTED) { //Thoát ra khỏi vòng 
    delay(5);
    Serial.print('.');
    digitalWrite(sttStandby, HIGH);
  }
 
  Serial.println();
  Serial.println(F("Da ket noi WiFi"));
  Serial.println(F("Di chi IP cua ESP8266 (Socket Client ESP8266): "));
  digitalWrite(sttStandby, LOW);
  digitalWrite(sttActive, HIGH);
  Serial.println(WiFi.localIP());
 
     
  //pinMode các chân này là OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(sttStandby, OUTPUT);
  pinMode(sttActive, OUTPUT);
}

/*Hàm để in giá trị truyền về từ NODE-MCU ESP-12E cho mạch */
void HienThiLED7doan(unsigned long Giatri, byte SoLed = 2) {
  
  byte *array= new byte[SoLed];
  for (byte i = 0; i < SoLed; i++) {
  //Lấy các chữ số từ phải quá trái
    array[i] = (byte)(Giatri % 10UL);
    Giatri = (unsigned long)(Giatri /10UL);
  }
  digitalWrite(latchPin, LOW);
  for (int i = SoLed - 1; i >= 0; i--)
    shiftOut(dataPin, clockPin, MSBFIRST, Seg[array[i]]); 
  
  digitalWrite(latchPin, HIGH);
  free(array);
}

void PrintLED(int temperature){
  
}

void loop() { 
  /*Ý tưởng:
   ***************************Dieu khien LED bang ESP 12E - NODE-MCU 1.0***************************
   * ESP-12E sẽ nhận gói dữ liệu .json từ server trả về
   * Sau đó sẽ hiển thị 2 giá trị
   * + mode của máy điều hòa: Standby hoặc Active
   * + nhiệt độ của máy điều hòa hiện tại
   * File json sẽ có 2 giá trị: Giá trị của nhiệt độ và mode của máy lạnh
   * Các lưu ý: 
   * + Nếu mode là Standby thì giá trị của bảng hiển thị là 88
   * + Nhiệt độ sẽ do 
  */
  static unsigned long airConTemp = 24; //Nhiệt độ của Air-conditioner

  WiFiClient client;
  String url = "";
  String temparature;
  
  url += "/api/air-temperature";
  //url += "&ip=";
  //url += WiFi.localIP().toString();
  
  
  if (!client.connect(HOST_NAME, PORT)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + HOST_NAME + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("[Response:]");
  
  while (client.connected())  {
    if (client.available()){
      temparature = client.readStringUntil('\n');
      Serial.println(temparature);
    }
  }

  //Serial.println("Do C");
  //Serial.println(temparature);
  HienThiLED7doan(temparature.toInt(), 2);
  
  //airConTemp = (airConTemp + 2) % 100UL; // Vòng tuần hoàn từ 0--99
  delay(500);//Đợi 0.5s cho mỗi lần cập nhật
}
