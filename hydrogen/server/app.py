#! /usr/bin/env python
# -*- coding: utf8 -*-

from flask import Flask
import json
from flask import abort
from flask import request
from flask import Response
import uuid
import subprocess
import time
import os
from datetime import timedelta
from flask import make_response, request, current_app
from functools import update_wrapper


if not os.path.exists('/tmp/hydrogen_server'):
	os.mkdir('/tmp/hydrogen_server')
	os.mkdir('/tmp/hydrogen_server/in')
	os.mkdir('/tmp/hydrogen_server/out')


app = Flask(__name__)



def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, basestring):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, basestring):
        origin = ', '.join(origin)
    if isinstance(max_age, timedelta):
        max_age = max_age.total_seconds()

    def get_methods():
        if methods is not None:
            return methods

        options_resp = current_app.make_default_options_response()
        return options_resp.headers['allow']

    def decorator(f):
        def wrapped_function(*args, **kwargs):
            if automatic_options and request.method == 'OPTIONS':
                resp = current_app.make_default_options_response()
            else:
                resp = make_response(f(*args, **kwargs))
            if not attach_to_all and request.method != 'OPTIONS':
                return resp

            h = resp.headers

            h['Access-Control-Allow-Origin'] = origin
            h['Access-Control-Allow-Methods'] = get_methods()
            h['Access-Control-Max-Age'] = str(max_age)
            if headers is not None:
                h['Access-Control-Allow-Headers'] = headers
            return resp

        f.provide_automatic_options = False
        return update_wrapper(wrapped_function, f)
    return decorator


@app.route('/')
def index():
	return "Welcome to the hydrogen test server! POST to /exec to get a response."

@app.route('/alive')
def alive():
	return "{'status': 'ready'}";

@app.route('/exec', methods=['POST'])
@crossdomain(origin="*")
def execute():
	sessionkey = str(uuid.uuid4());
	print("request id: " + sessionkey)
	if not request.json or not 'program' in request.json:
		abort(400);	

	f = open('/tmp/hydrogen_server/in/' + sessionkey, "w")
	of = open('/tmp/hydrogen_server/out/' + sessionkey, 'w');
	for line in request.json.get("program"):
		# check if this is correct
		f.write(line.encode('ascii', 'ignore') + "\n");
	f.close();
	ret = "";
	
	proc = subprocess.Popen(["hydrogen", "/tmp/hydrogen_server/in/" + sessionkey], stdout=of, stderr=subprocess.STDOUT);
	cnt = 0
	pid = proc.pid
	while True:
		time.sleep(.05);
		cnt += .05
		if proc.poll() == None or cnt >= 1 or os.path.getsize('/tmp/hydrogen_server/out/' + sessionkey) > 5000000:
			try:
				proc.terminate()
			except:
				pass;
			break;
	time.sleep(1);

	rets = open('/tmp/hydrogen_server/out/' + sessionkey, 'r').readlines();
	rjson = [];
	for line in rets:
		if line[-1:] == '\n':
			line = line[:-1]
		rjson.append(line)

	os.remove('/tmp/hydrogen_server/out/' + sessionkey);
	os.remove('/tmp/hydrogen_server/in/' + sessionkey);
	return json.dumps(rjson), 200

@app.route('/exec', methods=["OPTIONS"])
@crossdomain(origin="*", headers=["Content-Type"])
def getopts():
	return Response()

if __name__ == '__main__':
	app.run(debug=True, port=5000)
