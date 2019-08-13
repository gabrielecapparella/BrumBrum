#!/usr/bin/python3

from flask import Flask, render_template, request
import RPi.GPIO as GPIO
import signal
import serial

master = Flask(__name__)
pins = {
	'en1': 16,
	'in1': 20,
	'in2': 21,
	'en2': 8,
	'in3': 7,
	'in4': 1	
}

class DC_Motor:
	def __init__(self, en_pin, fwd_pin, rev_pin):
		GPIO.setmode(GPIO.BCM)
		GPIO.setup(en_pin, GPIO.OUT)
		GPIO.setup(fwd_pin, GPIO.OUT)
		GPIO.setup(rev_pin, GPIO.OUT)
		self.fwd = fwd_pin
		self.rev = rev_pin
		self.en = en_pin
		self.speed_ctrl = GPIO.PWM(en_pin, 100)
		self.speed_ctrl.start(0)
		GPIO.output(en_pin, True)

	# speed should be in [-100, 100]
	# positive value means forward, negative means reverse
	def set_state(self, speed: float):
		print('set_state', speed)
		if abs(speed)<15: speed = 0 # REMOVE BEFORE FLIGHT
		self.speed_ctrl.ChangeDutyCycle(abs(speed))
		GPIO.output(self.fwd, speed>0)
		GPIO.output(self.rev, speed<0)


@master.route('/')
def index():
	return render_template('index.html')


@master.route('/motors')
def move():
	x = int(request.args['x'])
	y = int(request.args['y'])

	if y==0:
		l = x
		r = -x
	else:
		slow = int(y-((abs(x*y)/100)*sign(y))) # magic
		if x>0:
			l = y
			r = slow
		else:
			l = slow
			r = y
	
	print('<0,{},{}>'.format(l, r))
	command = '<0,{},{}>'.format(l, r).encode()
	master.ser.write(command)

	return 'ok'


def motors_setup():
	master.motor_l = DC_Motor(pins['en1'], pins['in1'], pins['in2'])
	master.motor_r = DC_Motor(pins['en2'], pins['in3'], pins['in4'])


def teardown_handler(signal, frame):
	GPIO.cleanup()
	raise SystemExit

def sign(n):
	if n>0: return 1
	elif n<0: return -1
	return 0


#motors_setup()

#signal.signal(signal.SIGINT, teardown_handler)
if __name__ == '__main__':
	master.ser = serial.Serial('/dev/ttyS0', 9600)
	master.run('0.0.0.0', 8080)