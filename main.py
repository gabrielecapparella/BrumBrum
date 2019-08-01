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
	command = '<0,{},{}>'.format(x, y).encode()
	master.ser.write(command)

	# if y==0:
	# 	master.motor_l.set_state(x)
	# 	master.motor_r.set_state(-x)
	# else:
	# 	slow = y-((abs(x*y)/100)*(y/abs(y))) # magic
	# 	if x>0:
	# 		master.motor_l.set_state(y)
	# 		master.motor_r.set_state(slow)
	# 	else:
	# 		master.motor_l.set_state(slow)
	# 		master.motor_r.set_state(y)

	return 'ok'


def motors_setup():
	master.motor_l = DC_Motor(pins['en1'], pins['in1'], pins['in2'])
	master.motor_r = DC_Motor(pins['en2'], pins['in3'], pins['in4'])


def teardown_handler(signal, frame):
	GPIO.cleanup()
	raise SystemExit


#motors_setup()

#signal.signal(signal.SIGINT, teardown_handler)
master.ser = serial.Serial('/dev/ttyS0', 9600)
master.run('0.0.0.0', 8080)