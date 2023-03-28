import asyncio
import websockets
import time
gateway = "ws://192.168.4.1/webserialws"


import threading

class dummy:
    msg = ""

async def my_async_function():
    # do some asynchronous work here
    while True:
        if dummy.msg:
            dummy.msg = ""
            print("Async function completed")

def run_async_in_thread():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    thread_loop = threading.Thread(target=loop.run_until_complete, args=(my_async_function(),))
    thread_loop.start()
   

