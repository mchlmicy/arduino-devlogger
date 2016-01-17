// INCLUDE - DB WRITE
#include <Bridge.h>
#include <Process.h>

// INCLUDE - LED DISPLAY
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// INCLUDE - RTC
#include "RTClib.h"

// INCLUDE - TIMED EVENT 
#include <TimedEvent.h>
#define BLINK 1
#define TIMER 2

// alarm
char alarm_notes[] = "iFbFiFbFiFbFiFbFiGbGiGbGiGbGiGbGlbjblbjblbjblbjbjaHajaHajaHajaHa"; // aerodynamic - daft punk
const int alarm_frequency = 900 /* test: 15 */, alarm_set_max = 4, alarm_tempo = 125;
int alarm_set = 0;


// dial
bool dial_state = 1;

// file paths
const String django_manager = "/mnt/sdb1/demo/manage.py", db_path = "/mnt/sdb1/demo/db.sqlite3";

// frequencies
int frequencies[] = {110, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988};
char frequency_names[] = { 'o', 'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b', 'h', 'H', 'i', 'I', 'j', 'k', 'K', 'l', 'L', 'm', 'M', 'n'};

// led display
Adafruit_7segment matrix = Adafruit_7segment();

// pins
const int blink_pin = 4, btn_pin = 5, tone_pin = 6, dial_pin = A0;

// rtc
RTC_Millis rtc;

// states
bool blink_state = HIGH, btn_state, btn_prev_state, timer_state = LOW;

// timer
int time[4], timer = 0;

// timestamp
String timestamp_start, timestamp_end; 

void setup() 
{
  // delay
  delay(3500); // so we can see awesome console setup :P
  
  // serial
  Serial.begin(9600);
  Serial.println("serial connection established");
  
  // bridge
  Serial.print("opening bridge connection...");
  Bridge.begin();
  Serial.println("SUCCESS");
  
  // led display
  Serial.print("opening led connection...");
  matrix.begin(0x70);
  Serial.println("SUCCESS");
  
  // rtc
  Serial.print("opening rtc connection...");
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("SUCCESS");
  
  // pins
  Serial.print("setting pins...");
  pinMode(btn_pin, INPUT);
  pinMode(tone_pin, OUTPUT);
  Serial.println("SUCCESS");
  
  // time events
  Serial.print("setting time events...");
  TimedEvent.addTimer(BLINK, 500, sec_blink);
  TimedEvent.addTimer(TIMER, 1000, run_timer);
  Serial.println("SUCCESS");
  
  Serial.println("READY");
}

/************/
/* The Loop */
/************/
void loop() 
{
  TimedEvent.loop();
  update_state();
}

/****************/
/* Timed Events */
/****************/
// blink led
void sec_blink(TimerInformation* Sender)
{
  digitalWrite(blink_pin, blink_state);
  if(blink_state == HIGH){blink_state = LOW;} else{blink_state = HIGH;}
}

// timer
void run_timer(TimerInformation* Sender)
{
  parse_time(timer);
  write_timer(time);
  read_dialswitch();
  if(dial_state == 1){run_alarm(timer);}
  if(alarm_set < alarm_set_max){alarm_set++;} else{alarm_set = 1;}  
  timer++;
}

/*************/
/* Functions */
/*************/
// alarm
void alarm(int num_notes) 
{
    for(int x = 0; x < num_notes; x++)
    {
      tone(tone_pin, get_frequency(alarm_notes[x]), alarm_tempo);
      delay(alarm_tempo); 
    }
    
    int offset = ((alarm_tempo * num_notes) / 1000);
    if(offset > 0){offset--;}
    offset_timer(offset);
}


// get frequency
int get_frequency(char note) 
{
  for (int y = 0; y < (sizeof(alarm_notes) - 2); y++)
  {
    if (frequency_names[y] == note)         
    {
      return(frequencies[y]); 
    }
  }
  return(0);  
}

// get timestamp
String get_timestamp()
{
  DateTime now = rtc.now();
  int DateComponent[] = {now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second()};
  String temp_component, timestamp = "";
  
  for(int x = 0; x < 6; x++)
  {
    temp_component = String(DateComponent[x]);
    
    if(x != 0 && DateComponent[x] < 10){temp_component = "0" + temp_component;}
    if(x == 2){temp_component = temp_component + " ";}
    else if(x < 3){temp_component = temp_component + "-";}
    else if(x != 5 ){temp_component = temp_component + ":";}
    
    timestamp = timestamp + temp_component;
  }
  
  return timestamp + ".00000";
}

// offset timer 
void offset_timer(int offset)
{
  timer = timer + offset;
}

// parse time
void parse_time(int timer)
{
  if(timer < 60){time[0] = 0, time[1] = 0; time[2] = (timer/10) % 10, time[3] = timer % 10;} 
  else if(timer < 3600)
  {
    int timer_minute = timer / 60, timer_second = timer % 60; 
    time[0] = timer_minute / 10, time[1] = timer_minute % 10, time[2] = timer_second / 10, time[3] = timer_second % 10;
  }
  else
  {
    int timer_hour = timer / 3600, timer_minute = (timer / 60) - (timer_hour * 60); 
    time[0] = timer_hour / 10, time[1] = timer_hour % 10, time[2] = timer_minute / 10, time[3] = timer_minute % 10;
  } 
}

// push data
void push_data()
{
  Serial.print("pushing data...");
  Process execute;
  execute.runShellCommand("sqlite3 -line "+ db_path +" 'insert into app_timecard (\"duration\", \"timestamp_start\", \"timestamp_end\") Values (\""+timer+"\", \""+timestamp_start+"\", \""+timestamp_end+"\");' ; echo $?");
  Serial.println("DONE");
}

// read dialswitch
void read_dialswitch()
{
  if(timer > 1)
  {
    int switch_val = analogRead(A0);
    if(switch_val > 350 && dial_state == 1){dial_state = 0; Serial.println("\nalarm turned off \n");}
    else if(switch_val < 350 && dial_state == 0){dial_state = 1; Serial.println("\nalarm turned on \n");}
  }
}

// reset timer
void reset()
{
  Serial.print("resetting...");
  alarm_set = 0;
  blink_state = LOW; digitalWrite(blink_pin, blink_state);
  timer = 0;
  Serial.println("DONE");
}

// run alarm
void run_alarm(int time)
{
  if(time % (alarm_frequency) == 0)
  {
    int num_notes = (64 * alarm_set) / alarm_set_max;
    alarm(num_notes);
  }
}

// state management
void update_state()
{
  btn_state = digitalRead(btn_pin);
  if(btn_state == HIGH && btn_state != btn_prev_state)
  {
    if(timer_state == HIGH){timer_state = LOW;}
    else if(timer_state == LOW){timer_state = HIGH;}
    update_timer(timer_state);
  }
  btn_prev_state = btn_state;
}

// timer management
void update_timer(bool state)
{
  if(state == HIGH){timestamp_start = get_timestamp();  TimedEvent.start(BLINK); TimedEvent.start(TIMER); Serial.println("timer status: RUNNING...");}
  else
  {
    timestamp_end = get_timestamp();
    TimedEvent.stop(BLINK); 
    TimedEvent.stop(TIMER);
    push_data();
    reset();
    Serial.println("timer status: OFF"); 
  }
}

// write timer
void write_timer(int time[])
{
  matrix.writeDigitNum(0, time[0], false);
  matrix.writeDigitNum(1, time[1], false);
  matrix.drawColon(true);
  matrix.writeDigitNum(3, time[2], false);
  matrix.writeDigitNum(4, time[3], true);
  matrix.writeDisplay();
  
  Serial.println(timer);
}
