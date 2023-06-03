import sys, os, http.client, queue, threading, time, signal

NUM_REQUESTS = 100000
CONCURRENCY = 4
BASE_URL = "localhost:5000"

thread_lock = threading.Lock()
failures = 0


def do_work():
    while True:
        url = q.get()
        start_time = time.time()
        res = do_request(url)
        elapsed_time = time.time() - start_time

        # print(f"Request Time: {elapsed_time * 1000}ms")
        if res.status != 200:
            with thread_lock:
                failures += 1

        q.task_done()


def do_request(url):
    try:
        conn = http.client.HTTPConnection(url)
        conn.request("GET", "/")
        res = conn.getresponse()
        conn.close()
        return res
    except Exception as e:
        print(e)
        os.kill(os.getpid(), signal.SIGTERM)


q = queue.Queue(CONCURRENCY * 2)


def main():
    for i in range(CONCURRENCY):
        t = threading.Thread(target=do_work)
        t.daemon = True
        t.start()

    try:
        start_time = time.time()
        for i in range(NUM_REQUESTS):
            q.put(BASE_URL)
        q.join()

        total_time = time.time() - start_time
        print(f"Completed run with {NUM_REQUESTS} and {failures} failures")
        print(f"Total time elapsed: {total_time} seconds")

    except KeyboardInterrupt:
        sys.exit(1)


if __name__ == "__main__":
    main()
