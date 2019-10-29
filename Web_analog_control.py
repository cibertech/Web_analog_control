import tkinter
from threading import Thread
from tkinter import *
import socket
import time

def receive():
    # Receive data
    reply = " "
    while True:
        try:
            reply = s.recv(4096)
            decoded_reply = reply.decode('UTF-8')
            tdata = 'Received: ' + str(decoded_reply) + '\r\n'
            text1.delete(1.0, END)
            text1.insert('1.0', tdata)
            process_data(decoded_reply)
        except OSError:  # socket.error:
            break


def slider_change():
    global s1_value
    global old_s1_value
    global s2_value
    global old_s2_value
    while True:
        s1_value = s1.get()
        s2_value = s2.get()
        if s1_value != old_s1_value or s2_value != old_s2_value:
            request = 'S,' + str(s1_value) + ',' + str(s2_value) + ',' + '\r\n'
            try:
                s.sendall(request.encode())
            except socket.error:
                print('Send failed')
            old_s1_value = s1_value
            old_s2_value = s2_value
            time.sleep(.1)


s1_value = 0
old_s1_value = s1_value
s2_value = 0
old_s2_value = s2_value
m = tkinter.Tk()

port = "COM9"
baud = 115200

input_data = ""

TCP_IP = '192.168.1.237' #specific....change this according to your settings
TCP_PORT = 80
BUFFER_SIZE = 1024

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error:
    print('Failed to create socket')

try:
    s.connect((TCP_IP, 80))
except socket.error:
    print('Socket closed')

request = "HELLO"
try:
    s.sendall(request.encode())
except socket.error:
    print('Send failed')

def OnePlus():
    request = "SLIDER1"
    try:
        s.sendall(request.encode())
    except socket.error:
        print('Send failed')

def OneMinus():
    request = "SLIDER2"
    try:
        s.sendall(request.encode())
    except socket.error:
        print('Send failed')

def TwoPlus():
    print("")
    #ser.write(b'2,+' + bytes([10]))


def TwoMinus():
    print("")
    #ser.write(b'2,-' + bytes([10]))


def process_data(input_data):
    #global Application.w1
    global w1
    global w2
    global LED1
    global LED2
    global LED3
    global LED4
    x = input_data.split(",")
    #check if format is correct
    if x[0] == 'S':
        value1 = x[1].isdigit()
        if value1:
            w1.set(x[1])
        value1 = x[2].isdigit()
        if value1:
            w2.set(x[2])
        #tdata = 'V,' + x[1] + ',' + x[2] + '\r\n'
        #text1.delete(1.0, END)
        #text1.insert('1.0', tdata)
        #ser.write(tdata.encode())
    elif x[0] == 'B':
        value1 = x[1].isdigit()
        if value1:
            if x[1] == "1":
                LED1.place(x = 30,y = 200)
            else:
                LED1.place_forget()
        value1 = x[2].isdigit()
        if value1:
            if x[2] == "1":
                LED2.place(x=30, y=250)
            else:
                LED2.place_forget()
        value1 = x[3].isdigit()
        if value1:
            if x[3] == "1":
                LED3.place(x=30, y=300)
            else:
                LED3.place_forget()
        value1 = x[4].isdigit()
        if value1:
            if x[4] == "1":
                LED4.place(x=30, y=350)
            else:
                LED4.place_forget()
    input_data = ""


def destroy():
    global m
    global s
    s.close()
    m.destroy()


m.title("Analog control test ")
m.minsize(width=100, height=100)
m.geometry('800x600+0+0')

button1 = tkinter.Button(m, text='Exit', width=20, command=destroy)
button1.place(x=400, y=0)
button2 = tkinter.Button(m, text='SLIDER1', width=20, command=OnePlus)
button2.place(x=10, y=0)
button3 = tkinter.Button(m, text='SLIDER2', width=20, command=OneMinus)
button3.place(x=150, y=0)
button4 = tkinter.Button(m, text='2 +', width=20, command=TwoPlus)
button4.place(x=10, y=20)
button5 = tkinter.Button(m, text='2 -', width=20, command=TwoMinus)
button5.place(x=150, y=20)

w1 = Scale(m, from_=100, to=0)
w1.place(x = 10,y = 50)
w2 = Scale(m, from_=100, to=0)  # , orient=HORIZONTAL)
w2.place(x = 50,y = 50)  # .pack()
s1 = Scale(m, from_=100, to=0)
s1.place(x = 300, y = 200)
s2 = Scale(m, from_=100, to=0)
s2.place(x = 400, y = 200)
text1 = Text(m, width=40, height=2)
text1.place(x=400, y=50)
LED1 = Canvas(m, width=50, height=50)
LED1.place(x = 30,y = 200)
LED1.create_oval(10, 10, 40, 40, fill='blue')  # outline="#f11",fill="#1f1", width=2)
LED2 = Canvas(m, width=50, height=50)
LED2.place(x = 30,y = 250)
LED2.create_oval(10, 10, 40, 40, fill='blue')
LED3 = Canvas(m, width=50, height=50)
LED3.place(x = 30,y = 300)
LED3.create_oval(10, 10, 40, 40, fill='blue')
LED4 = Canvas(m, width=50, height=50)
LED4.place(x = 30,y = 350)
LED4.create_oval(10, 10, 40, 40, fill='blue')


#s.connect((TCP_IP, TCP_PORT))
#print('Socket is open...')

m.grid()

receive_thread = Thread(target=receive)
receive_thread.start()

slider_change_thread = Thread(target=slider_change)
slider_change_thread.start()

m.mainloop()
