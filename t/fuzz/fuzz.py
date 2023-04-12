from threading import Thread
import sys, http.client
from queue import Queue

concurrent = 200

def doWork():
    while True:
        url = q.get()
        res = getStatus(url)
        doSomethingWithResult(res)
        q.task_done()

def getStatus(url):
    try:
        conn = http.client.HTTPConnection(url)
        conn.request("GET", "/")
        res = conn.getresponse()
        conn.close()
        return res
    except Exception as e:
        print(e)
        return "error", url

def doSomethingWithResult(res):
    print(res.status, res.getheaders())

q = Queue(concurrent * 2)
for i in range(concurrent):
    t = Thread(target=doWork)
    t.daemon = True
    t.start()
try:
    for i in range(10000):
        q.put("localhost:9000")
    q.join()
except KeyboardInterrupt:
    sys.exit(1)
