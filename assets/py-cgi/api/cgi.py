import os, json, shutil
from datetime import datetime

IMAGES = "./assets/py-cgi/images"

BODY = os.getenv("BODY", None)
METHOD = os.getenv("METHOD", None)
PATH_INFO = os.getenv("PATH_INFO", None)
FILENAME = os.getenv("FILENAME", None)

def response(code):
	now = datetime.now()

	if (code == 200):
		print("HTTP/1.1 200 OK")
	elif (code == 201):
		print("HTTP/1.1 201 Created")
	elif (code == 202):
		print("HTTP/1.1 202 Accepted")
	else:
		print("HTTP/1.1 404 Not Found")

	print("Date: {}".format(now.strftime("%a, %d %b %Y %H:%M:%S GMT")))

	if (code == 404):
		print("Content-Type: text/html")
		content = "<h1>Not Found</h1>"
		print(f"Content-Length: {len(content)}")
		print("\r")
		print(content)
		return

	print("Content-Type: application/json")

	res = []
	for image in os.listdir(IMAGES):
		if image != ".placeholder":
			res += ["/images/" + image]

	dump = json.dumps(res)
	length = len(dump)

	print(f"Content-Length: {length}")

	print("\r")

	print(dump)

if __name__ == "__main__":
	try:
		code = 200
		if (METHOD == "POST"):
			code = 201
			if not FILENAME:
				raise Exception("Internal Server Error")
			shutil.move(FILENAME, IMAGES + "/" + FILENAME)
		elif (METHOD == "DELETE"):
			code = 202
			if BODY:
				body = json.loads(BODY)
				file = body["file"][7:]
				os.remove(IMAGES + "/" + file)
		if (PATH_INFO != "/api"):
			code = 404
		response(code)
	except:
		now = datetime.now()
		print("HTTP/1.1 500 Internal Server Error")
		print("Date: {}".format(now.strftime("%a, %d %b %Y %H:%M:%S GMT")))
		print("Content-Type: text/html")
		content = "<h1>Internal Server Error</h1>"
		print(f"Content-Length: {len(content)}")
		print("\r")
		print(content)
