#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <motor_drv.h>
#include <sia_board_io.h>

using namespace std;

class OmniDriver {
  private:
    Motor motor; // offizieller SIA-Treiber wird aufgerufen

    int keep_degree_in_intervall(int degree) { //Setzt Gradzahl in Intervall: 0-360
      degree = fmod(degree, 360.0);
      if (degree < 0)
          degree += 360.0;
      return degree;
    }

    float calculate_motor_speed(int degree /*Eingabewinkel*/, int m_degree /*Winkel des Motors*/) {
      int drive_degree = keep_degree_in_intervall(degree - m_degree);

      if (drive_degree == 90 || drive_degree == 270){
        return 0;
      }

      float speed = cos(drive_degree * (M_PI / 180));
      return speed;
    }

    float calculate_correction_speed(float speed, int degree, float zero_degree, float ninety_degree){
      speed = speed * 1000;
      zero_degree = zero_degree * 1000;
      ninety_degree = ninety_degree * 1000;

      degree = keep_degree_in_intervall(degree);
      degree = fmod(degree, 180.0);

      if (degree > 90){
        degree = degree - 90;
      }

      float correction = map(degree, 0, 90, zero_degree, ninety_degree);

      return (correction / 1000);
    }

    float get_max_of_three(float val1, float val2, float val3){
      float max_1 = max(abs(val1), abs(val2));
      float max_2 = max(max_1, abs(val3));
      return max_2;
    }

    void set_motor_speed(float m1Speed, float m2Speed, float m3Speed, int degree=999) {
      if (degree < 998){
        m1Speed = m1Speed * calculate_correction_speed(m1Speed, degree, 1.0, 1.0);
        m2Speed = m2Speed * calculate_correction_speed(m1Speed, degree, 0.90, 1.00);
        m3Speed = m3Speed * calculate_correction_speed(m1Speed, degree, 1.0, 0.76);
      }

      for (int x = 20; x <= 100; x++) { // Beschleunigung
        int m1_mapSpeed = map(x, 0, 100, 0, m1Speed);
        int m2_mapSpeed = map(x, 0, 100, 0, m2Speed);
        int m3_mapSpeed = map(x, 0, 100, 0, m3Speed);
        motor.driverGo(7, m1_mapSpeed, m2_mapSpeed, m3_mapSpeed, 0);
        delay(10);
      }
    }

    float map_motor_to_speed(float motor_speed, int specified_speed, float max_speed){
      if(motor_speed > 0){
        motor_speed = map(abs(motor_speed), 0, max_speed, 0, specified_speed);
      }else if(motor_speed < 0){
        motor_speed = -map(abs(motor_speed), 0, max_speed, 0, specified_speed);
      }
      return motor_speed;
    }

    float get_time_for_distance(int distance /* in cm */, int degree) {
      float distance_m = distance * 0.01;

      float m1Speed = calculate_motor_speed(degree, 30) * 100;
      float m2Speed = -calculate_motor_speed(degree, -30) * 100;
      float m3Speed = -calculate_motor_speed(degree, 90) * 100;

      float max = get_max_of_three(m1Speed, m2Speed, m3Speed);
      
      m1Speed = map_motor_to_speed(m1Speed, 80, max);
      m2Speed = map_motor_to_speed(m2Speed, 80, max);
      m3Speed = map_motor_to_speed(m3Speed, 80, max);

      float v = map_motor_to_speed(m1Speed, 1124.5, 100) / 1000;
      float v_res = v / cos((degree - 30) * (M_PI / 180));
      float a_res = v_res / 0.827;
      float distance_a = 0.5 * a_res * 0.683929 * 1.465;

      Serial.println("Distance: " + String(distance_a));

      if (distance_m <= distance_a) {
        float time_ms = sqrt(2 * distance_m / a_res) * 1000;
        return time_ms;
      }else {
        float s_neu = distance_m * 1.45985 - distance_a;
        float time_ms = (s_neu / v_res) * 1000 + 827;
        return time_ms;
      }
    }

  public:
    OmniDriver(){
      Serial.println("Starting driver...");
    }

    void drive(int degree, int speed) {
      float m1Speed = calculate_motor_speed(degree, 30) * 100;
      float m2Speed = -calculate_motor_speed(degree, -30) * 100;
      float m3Speed = -calculate_motor_speed(degree, 90) * 100; 

      float max = get_max_of_three(m1Speed, m2Speed, m3Speed);

      m1Speed = map_motor_to_speed(m1Speed, speed, max);
      m2Speed = map_motor_to_speed(m2Speed, speed, max);
      m3Speed = map_motor_to_speed(m3Speed, speed, max);

      set_motor_speed(m1Speed, m2Speed, m3Speed, degree);
    };

    void distance_drive(int distance /* in cm */, int degree){
      float time = get_time_for_distance(distance, degree);
      drive(degree, 80);
      delay(time);
      stop();
    };

    void rotate_left(int speed) {
      set_motor_speed(-speed, -speed, -speed);
    };

    void rotate_right(int speed) {
      set_motor_speed(speed, speed, speed);
    };

    void stop() {
      motor.driverStop(MOTOR_ALL);
    };

};
