#include "cua_tu_esp.h"

void handleOk() {
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.sendHeader("Access-Control-Allow-Methods", "*");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json; charset=utf-8", "{\"status\":\"success\"}");
}

void handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
    ECHOLN("done!");
}

void Open(){
    server.send(200, "text/html", "{\"status\":\"open\"}");
    ECHOLN("open");
    statusStop = false;
    Forward = true;
    daytay = false;
    digitalWrite(DIR, HIGH);
    SetPWMspeed.start();
    tickerCaculateSepeed.start();
}
void OpenClick(){
    ECHOLN("open_click");
    statusStop = false;
    Forward = true;
    daytay = false;
    digitalWrite(DIR, HIGH);
    SetPWMspeed.start();
    tickerCaculateSepeed.start();
}

void Close(){
    server.send(200, "text/html", "{\"status\":\"close\"}");
    ECHOLN("close");
    statusStop = false;
    Forward = false;
    daytay = false;
    digitalWrite(DIR, LOW);
    SetPWMspeed.start();
    tickerCaculateSepeed.start();
}

void CloseClick(){
    ECHOLN("close_click");
    statusStop = false;
    Forward = false;
    daytay = false;
    digitalWrite(DIR, LOW);
    SetPWMspeed.start();
    tickerCaculateSepeed.start();
}

void Stop(){
    ECHOLN("Stop");
    server.send(200, "text/html", "{\"status\":\"stop\"}");
    tickerCaculateSepeed.stop();
    SetPWMspeed.stop();
    digitalWrite(PWM, LOW);
    delay(200);
    statusStop = false;
    daytay = false;
    timecaculateSpeed = 0;
    if(Forward == true){
        digitalWrite(DIR, LOW);     //cho dong co quay nghich
        Forward = false;
    }else{
        digitalWrite(DIR, HIGH);
        Forward = true;
    }
    SetPWMStopSpeed.start();
}

void StopClick(){
    ECHOLN("Stop_click");
    tickerCaculateSepeed.stop();
    SetPWMspeed.stop();
    digitalWrite(PWM, LOW);
    delay(200);
    statusStop = false;
    daytay = false;
    timecaculateSpeed = 0;
    if(Forward == true){
        digitalWrite(DIR, LOW);     //cho dong co quay nghich
        Forward = false;
    }else{
        digitalWrite(DIR, HIGH);
        Forward = true;
    }
    SetPWMStopSpeed.start();
}

void buttonClick(){
    ECHOLN("buttonClick");
    digitalWrite(PWM, LOW);
    daytay = false;
    delay(500);
    if(statusStop == false){
        StopClick();
    }
    else if(Forward == false && statusStop == true){
        //OpenClick();
        CloseClick();
    }else{
        OpenClick();
        //CloseClick();
    }
}

void inputSpeed(){
    if(Forward == true){
        countPulFG++;
    }else{
        countPulFG--;
    }
    //ECHOLN(countPulFG);
}

void caculateSpeed(){
    ECHOLN("speed");
    if(timecaculateSpeed <= 10){
        timecaculateSpeed++;
    }
    pul = countPulFG;
    speed = (pul - prepul)/(0.1*6);
    prepul = pul;
    ECHO("van toc: ");
    ECHOLN(speed);
    if(abs(speed) <= MINSPEED && timecaculateSpeed >= 3){
        ECHOLN("Da dung lai");
        tickerCaculateSepeed.stop();
        SetPWMspeed.stop();
        digitalWrite(PWM, LOW);
        statusStop = false;
        if(fristRun == true && countFrirstRun < 5){
            countFrirstRun++;
        }
        if(fristRun == true && countFrirstRun == 1){
            countPulFG = 0;
            prepul = 0;
        }
        else if(fristRun == true && countFrirstRun == 2){
            countPulDistant = abs(countPulFG);
            if(countPulFG < 0){
                countPulFG = 0;
                prepul = 0;
            }
            fristRun = false;
        }

        
        if(Forward == true){
            digitalWrite(DIR, LOW);     //cho dong co quay nghich
            Forward = false;
        }else{
            digitalWrite(DIR, HIGH);
            Forward = true;
        }

        timecaculateSpeed = 0;
        SetPWMStopSpeed.start();    
    }
}

void setpwmStopMotor(){
    count_stop_motor++;
    //ECHOLN(count_stop_motor);
    // if(Forward == true){
    //     digitalWrite(DIR, HIGH);
    // }else{
    //     digitalWrite(DIR, LOW);
    // }
    if(count_stop_motor % 4 == 0){
        digitalWrite(PWM, HIGH);
    }else{
        digitalWrite(PWM, LOW);
        //countSetPwm = 0;
    }
    if(count_stop_motor >= 30000){        
        SetPWMStopSpeed.stop();
        delay(100);
        digitalWrite(PWM, LOW);
        delay(100);
        count_stop_motor = 0;
        statusStop = true;
        daytay = true;
    }
    
}

void setpwmMotor(){
    countSetPwm++;
    if(modeFast == false){ //ti le PWM la 3 HIGH 1 LOW
        if(countSetPwm <= 3){
            digitalWrite(PWM, LOW);
        }else{
            digitalWrite(PWM, HIGH);
            countSetPwm = 0;
        }
    }else{                //ti le PWM la 1 HIGH 3 LOW
        if(countSetPwm <= 3){
            digitalWrite(PWM, HIGH);
        }else{
            digitalWrite(PWM, LOW);
            countSetPwm = 0;
        }
    }   

    // if(fristRun == false){  //neu di het do dai hanh trinh thi se dung lai
    //     if(countPulFG = 0 || countPulFG = countPulDistant){ //dung lai
    //         Setspeed.stop();
    //     }
    // }
}


void tickerupdate(){
    //tickerSetApMode.update();
    tickerCaculateSepeed.update();
    SetPWMspeed.update();
    SetPWMStopSpeed.update();
}


bool testWifi() {
    ECHO("Connecting to: ");
    ECHOLN(SSID);
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    server.close();
    delay(1000);
    //setupIP();      //cai dat ip theo quy dinh
    WiFi.config(ip, gateway, subnet);
    WiFi.mode(WIFI_STA);        //bat che do station
    WiFi.begin(SSID, PASS);
    int c = 0;
    ECHOLN("Waiting for Wifi to connect");
    while (c < 20) {
        if (WiFi.status() == WL_CONNECTED) {
            ECHOLN("\rWifi connected!");
            ECHO("Local IP: ");
            ECHOLN(WiFi.localIP());
            return true;
        }
        delay(500);
        ECHO(".");
        c++;
        // if(digitalRead(PIN_CONFIG) == LOW){
        //     break;
        // }
    }
    ECHOLN("");
    ECHOLN("Connect timed out");
    return false;
}

void StartNormalSever(){
    server.on("/", HTTP_GET, handleRoot);
    server.on("/open", HTTP_GET, Open);
    server.on("/close", HTTP_GET, Close);
    server.on("/stop", HTTP_GET, Stop);
    server.on("/", HTTP_OPTIONS, handleOk);
    server.on("/open", HTTP_OPTIONS, handleOk);
    server.on("/close", HTTP_OPTIONS, handleOk);
    server.on("/stop", HTTP_OPTIONS, handleOk);
    server.begin();
    ECHOLN("HTTP server started");
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(DIR, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(CONFIG, INPUT);
    pinMode(inputFG, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(hallSensor1, INPUT_PULLUP);
    pinMode(hallSensor2, INPUT_PULLUP);

    testWifi();
    StartNormalSever();

    attachInterrupt(digitalPinToInterrupt(hallSensor1), dirhallSensor1, FALLING);
    attachInterrupt(digitalPinToInterrupt(hallSensor2), dirhallSensor2, FALLING);
    attachInterrupt(digitalPinToInterrupt(inputFG), inputSpeed, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON), dirhallSensor3, FALLING);
}


void loop() {
    // put your main code here, to run repeatedly:

    tickerupdate();
    server.handleClient();

    if(fristRun == false && Forward == true && countPulFG < SCALE*countPulDistant){
        modeFast = true;
    }else if(fristRun == false && Forward == false && countPulFG > (1-SCALE)*countPulDistant){
        modeFast = true;       
    }else{
        modeFast = false;
    }

    if (WiFi.status() != WL_CONNECTED){
        if (testWifi()){
            StartNormalSever();
        } 
    }

//    if(digitalRead(BUTTON) == LOW){
//        buttonClick();
//    }


}
void dirhallSensor1(){      //nhan du lieu tu cam bien ben ngoai
//    ECHOLN("1");
      if(daytay == true && statusStop == true){
          ECHOLN("1");
          if(countHall3 == true){
              //cho dong co chay thuan
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("thuan");
              OpenClick();
          }
          else if(countHall2 == true){
              //cho dong co chay nghich
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("nghich");
              CloseClick();
          }
          else{
              countHall1 = true;
          }
      }
}

void dirhallSensor2(){
//    ECHOLN("2");
      if(daytay == true && statusStop == true){
          ECHOLN("2");
          if(countHall1 == true){
              //cho dong co chay thuan
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("thuan");
              OpenClick();
          }
          else if(countHall3 == true){
              //cho dong co chay nghich
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("nghich");
              CloseClick();
          }
          else{
              countHall2 = true;
          }
      }
}
void dirhallSensor3(){
//    ECHOLN("3");
      if(daytay == true && statusStop == true){
          ECHOLN("3");
          if(countHall2 == true){
              //cho dong co chay thuan
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("thuan");
              OpenClick();
          }
          else if(countHall1 == true){
              //cho dong co chay nghich
              countHall1 = false;
              countHall2 = false;
              countHall3 = false;
              ECHOLN("nghich");
              CloseClick();
          }
          else{
              countHall3 = true;
          }
      }
}
