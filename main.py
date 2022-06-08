from ipaddress import ip_interface
from timeit import repeat
from serial import Serial, SerialException
from serial.tools import list_ports
import sys
import pygame
from pygame.locals import *
import threading
from queue import Queue
import time
import struct
import math


commandQueue = Queue()
global connection_status
connection_status = "DESCONECTADO"
connection_status_color = pygame.Color(255, 255, 255)
serial_port = "COM4"
PING_DELAY = 5
LATENCY = 0
MAX_CONSECUTIVE_COMMANDS = 10

BASE = 0x00
OMBRO = 0x01
COTOVELO = 0x02
GARRA = 0x03

keep_serial = True

def serial_handler():
    global keep_serial
    global connection_status
    global LATENCY

    while keep_serial:
        try:
            global connection_status
            global connection_status_color

            serial_connections = list_ports.comports()
            if (len(serial_connections) > 0):
                port = serial_port if serial_port else list_ports.comports()[0].name

                connection_status = f"Conectando-se a: {port}"
                port = Serial(port)
                connection_status = f"CONECTADO em: {port.name}"
                connection_status_color = pygame.Color(0, 255, 0)
            else:
                connection_status = f"Aguardando conexão com o Arduino"
            break
        
        except SerialException as e:
            if "FileNotFoundError" in str(e):
                connection_status = f"Aduino não encontrado em {port}!"
            else:
                connection_status = f"Acesso negado em {port}:{e}" 
            connection_status_color = pygame.Color(255,0,0)
        except IndexError as e: 
            connection_status = f"Nenhum dispositivo conectado!"
            connection_status_color = pygame.Color(255,0,0)


    command_buffer:bytearray = bytearray()
    to_send_buffer:bytearray = bytearray()
    last_sent = time.time()
    last_command = None
    repeated_counter:int = 0

    while keep_serial:

        # if port.in_waiting > 0:
        #     received_packet = port.read(port.in_waiting)        
        #     if received_packet[0] == 0xFF:
        #         LATENCY = int.from_bytes((received_packet[1], received_packet[2], received_packet[3], received_packet[4]), "big")
        
        current_command = commandQueue.get()
        port.write(current_command)
        print(current_command)


serialThread = threading.Thread(target=serial_handler)


WIDTH, HEIGHT = 500, 500
WINDOW_SIZE = (WIDTH, HEIGHT)

pygame.init()
pygame.display.set_caption("Controlador do braço robótico")
screen = pygame.display.set_mode(WINDOW_SIZE, 0, 32)
clock = pygame.time.Clock()
font = pygame.font.SysFont(None, 32)

pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(i) for i in range(pygame.joystick.get_count())]
# for joystick in joysticks:
#     print(joystick.get_name())

serialThread.start()

joystock = joysticks[0] if len(joysticks) > 0 else None
speeds = [30, 60, 80, 100, 150, 180]
speed = 0
paulo = 0XFF

last_values = [9123913912, 9123913912, 9123913912, 9123913912, 9123913912, 9123913912, 9123913912]
colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]
joystick_axis_color = 0
joystick_axis_size = 10
joystick_limit_radius = joystick_axis_size * 5
joystick_draw_position = (WIDTH - WIDTH / 2, HEIGHT - HEIGHT / 4)
motion = [0, 0, 0, 0, 0, 0]
servos = [BASE, OMBRO, paulo, COTOVELO, OMBRO, GARRA]
activation_intervals = [-1024, -768, -512, -256,  0, 256, 512, 768, 1024]
error_margin = 0.03



def clamp(minimum, x, maximum):
    return max(minimum, min(x, maximum))

def get_input_interval(vinput):
    return activation_intervals.index(min([int(clamp(-1024, vinput-i, 1024)) for i in activation_intervals]))

while True:
    screen.fill((0, 0, 0))
    x_value = 0 if joystock is None else (joystick_draw_position[0] + joystock.get_axis(0) * joystick_limit_radius)
    y_value = 0 if joystock is None else (joystick_draw_position[1] + joystock.get_axis(1) * joystick_limit_radius)
    last_value = joystock.get_axis(1) if joystock is not None else 0
    
    pygame.draw.circle(screen, pygame.Color(255,255,255), joystick_draw_position , joystick_limit_radius, width=5)
    axis_line = pygame.draw.line(screen, pygame.Color(255, 255, 255), joystick_draw_position, (x_value, y_value))
    axis_circle_rect = pygame.draw.circle(screen, colors[int(connection_status == "CONECTADO")], (x_value, y_value), joystick_axis_size)

    latency_text = f"PING: {str(LATENCY)}ms"
    latency_text_color = pygame.Color(255, 255, 255)
    latency_text_img = font.render(latency_text, True, latency_text_color)
    screen.blit(latency_text_img, (WIDTH/2 - latency_text_img.get_width() / 2 , 75))

    joystick_status_text = "Joystick não conectado" if len(joysticks) < 1 else f"Joystick:{joysticks[0].get_name()}"
    joystick_status_color = pygame.Color(255, 0, 0) if len(joysticks) < 1 else pygame.Color(0, 255, 0)
    joystick_status_img = font.render(joystick_status_text, True, joystick_status_color)
    screen.blit(joystick_status_img, (WIDTH/2 - joystick_status_img.get_width() / 2 , 50))

    text = connection_status
    img = font.render(text, True, connection_status_color)
    screen.blit(img, (WIDTH/2 - img.get_width() / 2 , 100))

    for axis in [0,1,3]:
        if abs(motion[axis]) < 0.1:
            motion[axis] = 0

    for axis in [2,5]:
        motion[axis] = 0 if int(motion[axis]) == -1 else motion[axis]

    # if (motion[1] != 0):        
    #     last_value = last_values[1]
    #     is_repeating = abs(last_value - joystock.get_axis(1)) >= 0.00003

    #     if (last_value != 0 and is_repeating):
    #         angle = clamp(-1024, int((last_value * 1024)), 1024)
    #         negative = (angle < 0)
    #         command = bytearray([0x01, OMBRO, int(negative)])
    #         commandQueue.put(command)  

    for axis in [0,1,3]:
        if motion[axis] != 0:
            last_value = last_values[axis]
            is_repeating = abs(last_value - joystock.get_axis(axis)) <= 0.0003 
            

            if (last_value != 0 and is_repeating): 
                angle = clamp(-1024, int((last_value * 1024)), 1024)
                negative = not (angle < 0)
                command = bytearray([0x01, servos[axis], int(negative)])
                commandQueue.put(command) 
                print(f"Putting Command : {command}")

    for axis in [2,5]:
        if motion[axis] != 0:
            last_value = last_values[axis]
            is_repeating = abs( last_value - joystock.get_axis(axis)) >= 0.00003
            if (last_value != 0 and is_repeating): 
                negative = (axis == 2)
                command = bytearray([0x01, servos[axis], int(negative)])
                commandQueue.put(command) 
                # print(f"Putting Command : {command}")                

    for event in pygame.event.get():   
        if event.type == JOYAXISMOTION:
            if event.axis in [0,1,2,3,4,5]:

                motion[event.axis] = event.value
                last_values[event.axis] = event.value
                angle = clamp(-1024, int((event.value * 1024)), 1024)
                negative = not (angle < 0)
                
                servo = servos[event.axis]
                command = bytearray([0x01, servo, int(negative)])
                commandQueue.put(command)
                # print(f"Putting command event : {command}")
        
        if event.type == JOYBUTTONDOWN:
            if event.button == 5:
                servo = servos[5]
                command = bytearray([0x01, servo, 0])
                commandQueue.put(command)
                # print(f"Putting command event : {command}")

            elif event.button == 4:
                servo = servos[5]
                command = bytearray([0x01, servo, 1])
                commandQueue.put(command)
                # print(f"Putting command event : {command}")

        if event.type == JOYDEVICEADDED:
            joysticks = [pygame.joystick.Joystick(i) for i in range(pygame.joystick.get_count())]
            for joystick in joysticks:
                print(joystick.get_name())
        if event.type == JOYDEVICEREMOVED:
            joysticks = [pygame.joystick.Joystick(i) for i in range(pygame.joystick.get_count())]
        if event.type == QUIT:
            pygame.quit()
            sys.exit()

        if event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                pygame.quit()
                serialThread.join()
                sys.exit()

    pygame.display.update()
    clock.tick(10)