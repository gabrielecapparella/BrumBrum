class Motor {
  private:
    int en;
    int fwd;
    int rev;
    int pwm_freq;
    int pwm_res;
    int pwm_ch;

  public:
    Motor(int en, int in1, int in2, int pwm_f, int pwm_r, int pwm_ch) {
      this->en = en;
      this->fwd = in1;
      this->rev = in2;
      this->pwm_freq = pwm_f;
      this->pwm_res = pwm_r;
      this->pwm_ch = pwm_ch;

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
};

const int PWM_L = 23;
const int IN_1_L = 17;
const int IN_2_L = 22;

const int PWM_R = 21;
const int IN_1_R = 4;
const int IN_2_R = 16;

const int PWM_FREQ = 100;
const int PWM_RES = 8; // 0-255

Motor motor_left = Motor(PWM_L, IN_1_L, IN_2_L, PWM_FREQ, PWM_RES, 0);
Motor motor_right = Motor(PWM_R, IN_1_R, IN_2_R, PWM_FREQ, PWM_RES, 1);

enum Instruction { MOTOR, SERVO, DISTANCE };

char received_chars[32];
boolean new_command = false;
int command[6]; // 1 command + 5 parameters

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
}

void loop() {
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
        
        /*if (y==0) {
          motor_left.set_speed(x);
          motor_right.set_speed(-x);
        } else {
          int slower = (int) (y-(abs(x*y)/100*sign(y))); // magic
          
          slower = (int) (slower*255/100);
          y = (int) (y*255/100);
          
          if (x>0) {
            motor_left.set_speed(y);
            motor_right.set_speed(slower);
          } else {
            motor_left.set_speed(slower);
            motor_right.set_speed(y);
          }
        }*/
    }
    new_command = false;
  }
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
