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

void getStatus(){
    if(Forward == true){
        server.send(200, "text/html", "{\"forward\" : \"close\"}");
    }else{
        server.send(200, "text/html", "{\"forward\" : \"open\"}");
    }
    ECHOLN("getstatus");

}

void setModeRunBegin(){
    server.send(200, "application/json; charset=utf-8", "{\"status\":\"success\"}");
    StaticJsonBuffer<RESPONSE_LENGTH> jsonBuffer;
    ECHOLN(server.arg("plain"));
    JsonObject& rootData = jsonBuffer.parseObject(server.arg("plain"));
    ECHOLN("--------------");
    if (rootData.success()){
        String setmoderunstr = rootData["setmoderun"];
        if(setmoderunstr == "1"){
            setmoderunbegin = 1;
        }else if(setmoderunstr == "2"){
            setmoderunbegin = 2;
        }else if(setmoderunstr == "3"){
            setmoderunbegin = 3;
        }
        ECHO("Writed: ");
        ECHOLN(setmoderunbegin);
        EEPROM.write(EEPROM_SET_MODE_RUN_BEGIN, char(setmoderunbegin));
        EEPROM.commit();
    }
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
    // detachInterrupt(digitalPinToInterrupt(BUTTON));
    ECHOLN("buttonClick");
    digitalWrite(PWM, LOW);
    daytay = false;
    delay(300);
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
    // attachInterrupt(digitalPinToInterrupt(BUTTON), buttonClick, RISING);

}

void setLedApMode() {
    digitalWrite(ledTestWifi, !digitalRead(ledTestWifi));
}


void inputSpeed(){
    if(Forward == true){
        countPulFG++;
    }else{
        countPulFG--;
    }
    // ECHOLN(countPulFG);
}

void inputDistant(){
    // if(Forward == true){
    //     countPulFGDistant++;
    // }else{
    //     countPulFGDistant--;
    // }
    // ECHOLN(countPulFGDistant);
}

void caculateSpeed(){
    //ECHOLN("speed");
    if(timecaculateSpeed <= 30){
        timecaculateSpeed++;
    }
    pul = countPulFG;
    speed = (pul - prepul)/(0.1*6);
    prepul = pul;
    
    //neu vuot qua khoang hanh trinh thi se dung lai
    // if(fristRun == false && (countPulFGDistant < stop_dau || countPulFGDistant > (countPulDistant - stop_cuoi))){
    //     if(countFrirstRun == 10){
    //         ECHOLN("Da dung lai do nam ngoai khoang hanh trinh");
    //         tickerCaculateSepeed.stop();
    //         SetPWMspeed.stop();
    //         digitalWrite(PWM, LOW);
    //         statusStop = false;
    //         if(Forward == true){
    //             digitalWrite(DIR, LOW);     //cho dong co quay nghich
    //             Forward = false;
    //         }else{
    //             digitalWrite(DIR, HIGH);
    //             Forward = true;
    //         }

    //         timecaculateSpeed = 0;
    //         SetPWMStopSpeed.start();
    //     }
    // }

    if(countFrirstRun == 2 && countPulFG > stop_dau && countPulFG < (countPulDistant - stop_cuoi)){        //countFrirstRun = 2 la luc bat dau qua trinh chay nhanh (modeFast = true)
        countFrirstRun = 10;        //lan dau tien chay nhanh
    }


    //ECHO("van toc: ");
    //ECHOLN(speed);
    if(abs(speed) <= MINSPEED && timecaculateSpeed >= 3){   //sau 3 lan chay thi moi tinh den van toc
        ECHOLN("Da dung lai");
        tickerCaculateSepeed.stop();
        SetPWMspeed.stop(); 
        digitalWrite(PWM, LOW);
        statusStop = false;

        if(fristRun == false && countPulFGDistant <= 3){
            countPulFGDistant = 0;
        }else if(fristRun == false && (countPulDistant -3) <= countPulFGDistant){
            countPulFGDistant = countPulDistant;
        }



        if(fristRun == true && countFrirstRun < 5){
            countFrirstRun++;
        }
        if(fristRun == true && countFrirstRun == 1){
            countPulFGDistant = 0;
            prepul = 0;
        }
        else if(fristRun == true && countFrirstRun == 2){
            countPulDistant = abs(countPulFGDistant);
            if(countPulFGDistant < 0){
                countPulFGDistant = 0;
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
        delay(300);
        luu_trang_thai_cua_sensor_ngay_khi_dung_lai = loai_bien_giong_nhau_cua_cam_bien;
        // ECHOLN(luu_trang_thai_cua_sensor_ngay_khi_dung_lai);
        loai_bien_giong_nhau_cua_cam_bien = 0;
        count_stop_motor = 0;
        statusStop = true;
        daytay = true;
    }
    
}

void setpwmMotor(){
    countSetPwm++;
    if(setmoderunbegin == 1){
        if(modeFast == false){ //ti le PWM la 3 HIGH 1 LOW
            if(countSetPwm <= 1){
                digitalWrite(PWM, LOW);
            }else{
                digitalWrite(PWM, HIGH);
                countSetPwm = 0;
            }
        }else{                //ti le PWM la 1 HIGH 3 LOWset
            digitalWrite(PWM, HIGH);
        } 
    }
    else if(setmoderunbegin == 2){
        if(modeFast == false){ //ti le PWM la 3 HIGH 1 LOW
            if(countSetPwm <= 2){
                digitalWrite(PWM, LOW);
            }else{
                digitalWrite(PWM, HIGH);
                countSetPwm = 0;
            }
        }else{                //ti le PWM la 1 HIGH 3 LOW
            digitalWrite(PWM, HIGH);
        } 
    }
    else if(setmoderunbegin == 3){
        if(modeFast == false){ //ti le PWM la 3 HIGH 1 LOW
            if(countSetPwm <= 3){
                digitalWrite(PWM, LOW);
            }else{
                digitalWrite(PWM, HIGH);
                countSetPwm = 0;
            }
        }else{                //ti le PWM la 1 HIGH 3 LOW
            digitalWrite(PWM, HIGH);
        } 
    }

}


void tickerupdate(){
    //tickerSetApMode.update();
    tickerCaculateSepeed.update();
    SetPWMspeed.update();
    SetPWMStopSpeed.update();
    tickerSetApMode.update();
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
            digitalWrite(ledTestWifi, LOW);
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
    server.on("/getstatus", HTTP_GET, getStatus);
    server.on("/setmoderun", HTTP_POST, setModeRunBegin);
    server.on("/open", HTTP_GET, Open);
    server.on("/close", HTTP_GET, Close);
    server.on("/stop", HTTP_GET, Stop);
    server.on("/", HTTP_OPTIONS, handleOk);
    server.on("/getstatus", HTTP_OPTIONS, getStatus);
    server.on("/setmoderun", HTTP_OPTIONS, setModeRunBegin);
    server.on("/open", HTTP_OPTIONS, handleOk);
    server.on("/close", HTTP_OPTIONS, handleOk);
    server.on("/stop", HTTP_OPTIONS, handleOk);
    server.begin();
    ECHOLN("HTTP server started");
}

String GetFullSSID() {
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    String macID;
    WiFi.mode(WIFI_AP);
    WiFi.softAPmacAddress(mac);
    macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    macID = SSID_PRE_AP_MODE + macID;
    ECHO("[Helper][getIdentify] Identify: ");
    ECHOLN(macID);
    return macID;
}


void setupConfigMode(){
    ECHOLN("[WifiService][setupAP] Open AP....");
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    server.close();
    delay(500);
    WiFi.mode(WIFI_AP_STA);
    IPAddress APIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(APIP, gateway, subnet);
    String SSID_AP_MODE = GetFullSSID();
    WiFi.softAP(SSID_AP_MODE.c_str(), PASSWORD_AP_MODE);
    ECHOLN(SSID_AP_MODE);

    ECHOLN("[WifiService][setupAP] Softap is running!");
    IPAddress myIP = WiFi.softAPIP();
    ECHO("[WifiService][setupAP] IP address: ");
    ECHOLN(myIP);
}




void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    EEPROM.begin(512);
    pinMode(DIR, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(CONFIG, INPUT);
    pinMode(inputFG, INPUT_PULLUP);
    pinMode(hallSensor3, INPUT_PULLUP);
    pinMode(hallSensor1, INPUT_PULLUP);
    pinMode(hallSensor2, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(ledTestWifi, OUTPUT);
    digitalWrite(ledTestWifi, HIGH);
    ECHOLN("");
    ECHOLN("START!!!");
    setmoderunbegin = char(EEPROM.read(EEPROM_SET_MODE_RUN_BEGIN));
    ECHOLN(setmoderunbegin);
    if(setmoderunbegin != 1 && setmoderunbegin != 2 && setmoderunbegin != 3){
        setmoderunbegin = 1;
        ECHOLN("read EEPROM fail, auto set 1!");
    }else{
        ECHO("read EEPROM done: ");
    }


    testWifi();
    StartNormalSever();

    attachInterrupt(digitalPinToInterrupt(hallSensor1), dirhallSensor1, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor2), dirhallSensor2, RISING);
    attachInterrupt(digitalPinToInterrupt(inputFG), inputSpeed, FALLING);
    attachInterrupt(digitalPinToInterrupt(hallSensor3), dirhallSensor3, RISING);
    // attachInterrupt(digitalPinToInterrupt(BUTTON), buttonClick, RISING);
}


void loop() {
    // put your main code here, to run repeatedly:

    tickerupdate();
    server.handleClient();

    if(fristRun == false && Forward == true && countPulFGDistant < SCALE*countPulDistant){
        modeFast = true;
    }else if(fristRun == false && Forward == false && countPulFGDistant > (1-SCALE)*countPulDistant){
        modeFast = true;       
    }else{
        modeFast = false;
    }

    if (Flag_Normal_Mode == true && WiFi.status() != WL_CONNECTED){
        digitalWrite(ledTestWifi, HIGH);
        if (testWifi()){
            StartNormalSever();
        }
    }

    if(Flag_Normal_Mode == true && digitalRead(BUTTON) == HIGH){
        buttonClick();
    }else{
        time_click_button = millis();
    }

    if(digitalRead(BUTTON) == HIGH && (time_click_button + CONFIG_HOLD_TIME) <= millis()){
        time_click_button = millis();
        digitalWrite(ledTestWifi, HIGH);
        Flag_Normal_Mode = false;
        tickerSetApMode.start();
        setupConfigMode();
    }


}
void dirhallSensor1(){      //nhan du lieu tu cam bien ben ngoai
    // if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 2){
    //     ECHOLN("true1");
    //     Forward = true;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }else if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 3){
    //     ECHOLN("false1");
    //     Forward = false;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }
    if(loai_bien_giong_nhau_cua_cam_bien != 1){
        loai_bien_giong_nhau_cua_cam_bien = 1;
        // ECHOLN("1");

        // if(Forward == true){
        //     countPulFGDistant++;
        // }else{
        //     countPulFGDistant--;
        // }

        if(daytay == true && statusStop == true){
            //ECHOLN("1");
            // loai_bien_giong_nhau_cua_cam_bien = 0;
            if(countHall2 == true){
                //cho dong co chay thuan
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("open");
                OpenClick();
            }
            else if(countHall3 == true){
                //cho dong co chay nghich
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("close");
                CloseClick();
            }
            else{
                countHall1 = true;
            }
        }
    }
    
}

void dirhallSensor2(){
    // if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 3){
    //     ECHOLN("true2");
    //     Forward = true;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }else if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 1){
    //     ECHOLN("false2");
    //     Forward = false;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }


    if(loai_bien_giong_nhau_cua_cam_bien != 2){
        loai_bien_giong_nhau_cua_cam_bien = 2;
        // ECHOLN("2");
        
        if(Forward == true){
            countPulFGDistant++;
        }else{
            countPulFGDistant--;
        }
        
        // ECHOLN(co untPulFGDistant);
        if(daytay == true && statusStop == true){
            //ECHOLN("2");
            // loai_bien_giong_nhau_cua_cam_bien = 0;
            if(countHall3 == true){
                //cho dong co chay thuan
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("open");
                OpenClick();
            }
            else if(countHall1 == true){
                //cho dong co chay nghich
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("close");
                CloseClick();
            }
            else{
                countHall2 = true;
            }
        }
    }
}
void dirhallSensor3(){
    // if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 1){
    //     ECHOLN("true3");
    //     Forward = true;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }else if(daytay == true && statusStop == true && luu_trang_thai_cua_sensor_ngay_khi_dung_lai == 2){
    //     ECHOLN("false3");
    //     Forward = false;
    //     luu_trang_thai_cua_sensor_ngay_khi_dung_lai = 0;
    // }

    if(loai_bien_giong_nhau_cua_cam_bien != 3){
        loai_bien_giong_nhau_cua_cam_bien = 3;
        // ECHOLN("3");

        // if(Forward == true){
        //     countPulFGDistant++;
        // }else{
        //     countPulFGDistant--;
        // }

        if(daytay == true && statusStop == true){
            //ECHOLN("3");
            // loai_bien_giong_nhau_cua_cam_bien = 0;
            if(countHall1 == true){
                //cho dong co chay thuan
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("open");
                OpenClick();
            }
            else if(countHall2 == true){
                //cho dong co chay nghich
                countHall1 = false;
                countHall2 = false;
                countHall3 = false;
                //ECHOLN("close");
                CloseClick();
            }
            else{
                countHall3 = true;
            }
        }
    }
}
