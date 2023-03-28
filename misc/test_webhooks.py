#!/usr/bin/env python

import asyncio
import websockets
import subprocess , os
scanOTA ='avahi-browse -t -r  "_arduino._tcp" | grep address | cut -d "[" -f 2 | cut -d "]" -f 1'

ip = subprocess.check_output(scanOTA, shell=True)
ip = ip.decode("utf-8")

gateway = f"ws://{ip}/webserialws"

print(gateway)

async def _send_await(msg):
    print("here")
    async with websockets.connect(gateway) as websocket:
        await websocket.send(msg)

import threading
import time
class dummy:
    msg = []


async def my_async_function():
    # do some asynchronous work here
    while True:
        if len(dummy.msg) > 3:
            dummy.msg = [dummy.msg[-1]]
        if len(dummy.msg):
            text = dummy.msg.pop(0)
            await _send_await(text)
            print("Async function completed "+text)
            
        # time.sleep(1/500)

def run_async_in_thread():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    thread_loop = threading.Thread(target=loop.run_until_complete, args=(my_async_function(),),daemon=True)
    thread_loop.start()

def send(msg):
    dummy.msg.append(msg)