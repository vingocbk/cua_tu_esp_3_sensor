#include <Arduino.h>
#include "AppDebug.h"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "ArduinoJson.h"
#include "Ticker.h"
#include "EEPROM.h"


#define hallSensor1 4  //16
#define hallSensor2 5  //14
#define hallSensor3 13
#define ledTestWifi 2
#define inputFG 12
#define PWM 14
#define DIR 16
#define BUTTON 15

#define SCALE 0.95
#define MINSPEED 8
#define CONFIG 0

#define RESPONSE_LENGTH 512     //do dai data nhan ve tu tablet
#define EEPROM_WIFI_SSID_START 0
#define EEPROM_WIFI_SSID_END 32
#define EEPROM_WIFI_MAX_CLEAR 512
#define EEPROM_WIFI_PASS_START 33
#define EEPROM_WIFI_PASS_END 64
#define EEPROM_WIFI_IP_START 65 
#define EEPROM_WIFI_IP_END 95
#define EEPROM_SET_MODE_RUN_BEGIN 100
#define EEPROM_DISTANT 105


#define SSID_PRE_AP_MODE "AvyInterior-"
#define PASSWORD_AP_MODE "123456789"


#define HTTP_PORT 80

#define SSID "HPT 2B"
#define PASS "hpt12345"

//#define SSID "hpt cnc"
//#define PASS "hpt12345"
#define CONFIG_HOLD_TIME 3000

ESP8266WebServer server(HTTP_PORT);

IPAddress ip(10, 10, 9, 158); // where xx is the desired IP Address
IPAddress gateway(10, 10, 9, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

//IPAddress ip(3, 3, 0, 157); // where xx is the desired IP Address
//IPAddress gateway(3, 3, 0, 1); // set gateway to match your network
//IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network




int countPulFG = 0;
int countPulFGDistant = 0;
int setmoderunbegin = 1; // cham nhat
uint32_t countPulDistant;                                 
uint8_t countSetPwm = 0;        //bien nay co tac dung dieu chinh toc do dong co
uint32_t countTime = 0;          //tinh thoi gian, timer 2 la 10us, vi the 100ms bien nay se co gia tri la 10.000
uint8_t sau_1_s = 0;
uint8_t timecaculateSpeed = 0;  //sau 500ms moi bat dau tinh van toc
int pul, prepul = 0;
float speed;
bool statusStop = true;     //true la he thong dang dung im, false la he thong dang chuyen dong
bool countHall1 = false, countHall2 = false, countHall3 = false;
bool modeFast = false;
bool Forward = true;    //chieu quay thuan
bool fristRun = true;   //kiem tra lan chay dau tinh khoang cach chieu dai tu
bool daytay = true;
bool Flag_Normal_Mode = true;
bool isSaveDistant = false;
int start_count_hall_sensor = 0;    //bat dau den 2 thi moi tinh day tay
int count_number_of_click = 0;      //click 5 lan thi chuyen mode
uint8_t countFrirstRun = 0;  //dem so lan va cham
uint32_t count_stop_motor = 0;
uint32_t time_click_button = 0;
uint32_t pre_time_click_button = 0;
int stop_dau = 0;           //gia tri set cho khoang dung lai
int stop_cuoi = 0;
int loai_bien_giong_nhau_cua_cam_bien = 0;
int luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;

void handleOk();
void handleRoot();
void getStatus();
void setModeRunBegin();
void inputSpeed();
void dirhallSensor1();
void dirhallSensor2();
void dirhallSensor3();
void setpwmMotor();
void tickerupdate();
void caculateSpeed();
bool testWifi();
void StartNormalSever();
void buttonClick();
void Open();
void OpenClick();
void Close();
void CloseClick();
void Stop();
void StopClick();
void setpwmStopMotor();
void inputDistant();        //doc quang duong
void setLedApMode();
void setupConfigMode();
String GetFullSSID();
void resetDistant();


//Ticker tickerSetApMode(setLedApMode, 200, 0);   //every 200ms
Ticker tickerCaculateSepeed(caculateSpeed, 100);   //every 200ms
Ticker SetPWMspeed(setpwmMotor, 10, 0, MICROS_MICROS);
Ticker SetPWMStopSpeed(setpwmStopMotor, 10, 0, MICROS_MICROS);
Ticker tickerSetApMode(setLedApMode, 200, 0);   //every 200ms
