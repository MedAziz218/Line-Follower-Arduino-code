from tkinter import *
import asyncio
from test_webhooks import send,run_async_in_thread
def update_values(*args):
    # print ( w2.get(),end='')
    send(str(w2.get()))
def quit_prog(*args):
    send('0')
    quit()
root = Tk()
root.title('vez')
w2 = Scale(root, from_=0, to=255, orient=HORIZONTAL ,command=update_values)
w2.set(0)
w2.pack(fill='x')
Button(root, text='Quit', command=quit_prog).pack()

root.update()
import os
os.system("i3-msg 'floating enable; resize set 400 200; move position center;'")
run_async_in_thread()
root.mainloop()
