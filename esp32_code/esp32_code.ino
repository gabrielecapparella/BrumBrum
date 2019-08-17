class Motor {
  private:
    int fwd;
    int rev;
    int pwm_ch;
    int encoder_pulses_per_turn;
    volatile int encoder_pulses;
    int speed_update_freq;
    int last_pulse;
    int last_measurement;

  public:
    float speed; // pulses per interval
    
    Motor(int en, int in1, int in2, int pwm_f, int pwm_r, int pwm_ch, int enc_pulses_per_turn, int enc_freq) {
      this->fwd = in1;
      this->rev = in2;
      this->pwm_ch = pwm_ch;

      this->encoder_pulses_per_turn = enc_pulses_per_turn;
      this->speed_update_freq = enc_freq;
      
      this->encoder_pulses = 0;
      this->speed = 0;
      this->last_measurement = millis();

      pinMode(en, OUTPUT);
      pinMode(in1, OUTPUT);
      pinMode(in2, OUTPUT);
      ledcSetup(pwm_ch, pwm_f, pwm_r);
      ledcAttachPin(en, pwm_ch);
    }

  void set_speed(int speed) {
    if (speed>0) {
      digitalWrite(fwd, HIGH);
      digitalWrite(rev, LOW);
    } else if (speed<0) {
      digitalWrite(rev, HIGH);
      digitalWrite(fwd, LOW);    
    } else {
      digitalWrite(fwd, LOW);
      digitalWrite(rev, LOW);   
    }
    ledcWrite(pwm_ch, abs(speed));
  }

  float get_speed() { // Round per second
    int now = millis();
    int elapsed = now - this->last_measurement;
    if (elapsed >= this->speed_update_freq) {
      this->speed = (float)(encoder_pulses*1000) / (float)(elapsed*this->encoder_pulses_per_turn);
      this->encoder_pulses = 0;
      this->last_measurement = now;
      if (this->speed>0) {
        Serial.print(this->speed);
        Serial.print(',');
      }
    }

    return this->speed;
  }

  void update_encoder() {
    this->encoder_pulses++;
  }
};

const int PWM_L = 23;
const int IN_1_L = 17;
const int IN_2_L = 22;

const int PWM_R = 21;
const int IN_1_R = 4;
const int IN_2_R = 16;

const int PWM_FREQ = 100;
const int PWM_RES = 8; // 0-255

const int ENCODER_R = 15;
const int ENCODER_L = 15; // REMOVE BEFORE FLIGHT
const int PULSES_PER_TURN = 10;
const int ENC_FREQ = 1000;

Motor motor_left = Motor(PWM_L, IN_1_L, IN_2_L, PWM_FREQ, PWM_RES, 0, PULSES_PER_TURN, ENC_FREQ);
Motor motor_right = Motor(PWM_R, IN_1_R, IN_2_R, PWM_FREQ, PWM_RES, 1, PULSES_PER_TURN, ENC_FREQ);

enum Instruction { MOTOR, SPEED, SERVO, DISTANCE };

char received_chars[32];
boolean new_command = false;
int command[6]; // 1 command + 5 parameters

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");

  // Attaching interrupts here is necessary because I can't give an instance method to attachInterrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_L), []{motor_left.update_encoder();}, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_R), []{motor_right.update_encoder();}, RISING);
}

void loop() {
  //motor_left.get_speed();
  motor_right.get_speed();
  
  recv_command();
  if (new_command) {
    switch ((Instruction) command[0]) {
      case MOTOR:
        //int l = (int) (command[1]*255/100);
        //int r = (int) (command[2]*255/100);
        
        int l = 155*sign(command[1]) + command[1];
        int r = 155*sign(command[2]) + command[2];
        //if (l==155) l = 0;
        //if (r==155) r = 0;

        motor_left.set_speed(l);
        motor_right.set_speed(r);
    }
    new_command = false;
  }
}

void update_encoder() {
  
}

void recv_command() {
  static boolean recv_in_progress = false;
  static byte ndx = 0;
  char start_marker = '<';
  char end_marker = '>';
  char rc;

  while (Serial.available() > 0 && !new_command) {
    rc = Serial.read();
    if (recv_in_progress) {
      if (rc != end_marker) {
        received_chars[ndx] = rc;
        ndx++;
      } else {
        received_chars[ndx] = '\0'; // terminate the string
        recv_in_progress = false;
        ndx = 0;
        new_command = true;
        parse_command();
      }
    } else if (rc == start_marker) {
      recv_in_progress = true;
    }
  }
}

void parse_command() {
  char * p = strtok(received_chars,",");
  command[0] = atoi(p);
  
  for (int i=1; i<=5; i++) {
    p = strtok(NULL, ",");
    if (p==NULL) break;
    command[i] = atoi(p);
  }
}

int sign(int n) {
  if (n>0) return 1;
  if (n<0) return -1;
  return 0;
}
