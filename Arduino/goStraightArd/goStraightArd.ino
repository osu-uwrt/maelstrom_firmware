
#include <Servo.h>

#include <ArduinoHardware.h>
#include <ros.h>
#include <riptide_msgs/PwmStamped.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Empty.h>
#include <riptide_msgs/Depth.h>
#include "MS5837.h"
#include "Wire.h"

MS5837 sensor;

//function prototypes
int16_t valid(int16_t pwm);
void pwm_callback(const riptide_msgs::PwmStamped &cmd);
//Globals
int STOP = 1500;  //thruster stop value
int killVal = 0;  //kill switch read value, default to KS out of bot
int missVal = 0;  //mission switch read value, default to MS out of bot
//individual values of pins plugged in
int killPin = 46;
int missPin = 12;
int spl_pin = 9;
int ssl_pin = 3;
int hpa_pin = 7;
int hsa_pin = 4; 
int hpf_pin = 10;
int hsf_pin = 5;
int swf_pin = 6;
int swa_pin = 2;

//using a PWM set the servo
Servo servo_spl, servo_ssl, servo_hpa, servo_hsa, servo_hpf, servo_hsf, servo_swf, servo_swa;
// ROS is the best
ros::NodeHandle nh;
std_msgs::Empty kill;
std_msgs::Empty mission;
riptide_msgs::Depth depth;
ros::Publisher mission_pub("state/mission", &mission);
ros::Publisher kill_pub("state/kill", &kill);
ros::Subscriber<riptide_msgs::PwmStamped> pwm_sub("command/pwm", &pwm_callback);
ros::Publisher state_pub("state/depth", &depth);


void setup() {
  
  Serial.begin(57600);
  //pins for the mission and kill
  pinMode(killPin, INPUT); 
  pinMode(missPin, INPUT); 
  //pin assignments for the ESCs->thrusters
  servo_spl.attach(spl_pin);
  servo_ssl.attach(ssl_pin);
  servo_hpa.attach(hpa_pin);
  servo_hsa.attach(hsa_pin);
  servo_hpf.attach(hpf_pin);
  servo_hsf.attach(hsf_pin);
  servo_swf.attach(swf_pin);
  servo_swa.attach(swa_pin);
  //ROS node handler setup
  nh.initNode();
  nh.subscribe(pwm_sub);
  nh.advertise(mission_pub);
  nh.advertise(kill_pub);
  nh.advertise(state_pub); 
  Wire.begin();
  sensor.init();
  sensor.setFluidDensity(997); //fluid density of freshwater in kg/m^3
  
}

void loop() {
  //check for callback
  nh.spinOnce();
  //Serial.println("I've got ROS going");
  killVal = digitalRead(killPin);
  missVal = digitalRead (missPin);

  
  //depth sensor stuffs

  sensor.read();  //read the sensor
  //build the ROS depth state, depth. is the mission computer stuff while sensor. is actually read off the sensor
  depth.depth = sensor.depth();
  depth.temp = sensor.temperature();
  depth.pressure = sensor.pressure();
  depth.altitude = sensor.altitude();
  //publish!
  state_pub.publish(&depth);

  
  //heartbeat for kill switch
  if (killVal == 1) {
     kill_pub.publish(&kill);
  }
  //if heartbeat gone (KS removed) write stop
  else {
    servo_spl.writeMicroseconds(valid(STOP));
    servo_ssl.writeMicroseconds(valid(STOP));
    servo_hpa.writeMicroseconds(valid(STOP));
    servo_hsa.writeMicroseconds(valid(STOP));
    servo_hpf.writeMicroseconds(valid(STOP));
    servo_hsf.writeMicroseconds(valid(STOP));
    servo_swf.writeMicroseconds(valid(STOP));
    servo_swa.writeMicroseconds(valid(STOP));
  }
  //second heartbeat for the mission start
  if(missVal ==1) {
      mission_pub.publish(&mission);
  }

}

void pwm_callback(const riptide_msgs::PwmStamped &cmd){
  /*
   * List of thrusters:
   *  surge_port_hi   surge_port_lo
   *  surge_stbd_hi   surge_stbd_lo
   *  sway_aft        sway_fwd
   *  heave_port_aft  heave_stdb_aft
   *  heave_port_fwd  heave_stdb_fwd
   */
   servo_spl.writeMicroseconds(valid(cmd.pwm.surge_port_lo));
   servo_ssl.writeMicroseconds(valid(cmd.pwm.surge_stbd_lo));
   servo_hpa.writeMicroseconds(valid(cmd.pwm.heave_port_aft));
   servo_hsa.writeMicroseconds(valid(cmd.pwm.heave_stbd_aft));
   servo_hpf.writeMicroseconds(valid(cmd.pwm.heave_port_fwd));
   servo_hsf.writeMicroseconds(valid(cmd.pwm.heave_stbd_fwd));
   servo_swf.writeMicroseconds(valid(cmd.pwm.sway_fwd));
   servo_swa.writeMicroseconds(valid(cmd.pwm.sway_aft));
   
  
}

// Ensure 1100 <= pwm <= 1900
int16_t valid(int16_t pwm)
{
  pwm = pwm > 1900 ? 1900 : pwm;
  pwm = pwm < 1100 ? 1100 : pwm;
  return pwm;
}

