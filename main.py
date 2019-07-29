#!/usr/bin/python3

from flask import Flask, render_template, request

master = Flask(__name__)

@master.route('/')
def index():
	return render_template('index.html')

@master.route('/move')
def move():
	x = request.args['x']
	y = request.args['y']
	print(x, y)
	return 'ok'

master.run('0.0.0.0', 8080)