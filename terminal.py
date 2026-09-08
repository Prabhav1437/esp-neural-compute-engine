#!/usr/bin/env python3
import sys
import time
import serial
import threading

PORT = "/dev/cu.usbserial-0001"
BAUD = 115200

def read_from_esp(ser, stop_event):
    while not stop_event.is_set():
        try:
            if ser.in_waiting:
                line = ser.readline().decode('utf-8', errors='ignore')
                sys.stdout.write(line)
                sys.stdout.flush()
            else:
                time.sleep(0.05)
        except Exception:
            break

def main():
    print("==================================================")
    print("      ESP32 Interactive Mac Terminal Console      ")
    print("==================================================")
    print(f"Connecting to ESP32 on {PORT} at {BAUD} baud...")
    
    try:
        ser = serial.Serial(PORT, BAUD, timeout=0.5)
    except Exception as e:
        print(f"Error opening serial port {PORT}: {e}")
        sys.exit(1)
        
    time.sleep(0.5)
    ser.dtr = False
    ser.rts = False
    time.sleep(0.2)
    
    stop_event = threading.Event()
    reader_thread = threading.Thread(target=read_from_esp, args=(ser, stop_event), daemon=True)
    reader_thread.start()

    print("Connected! Type expressions (e.g. 2 + 2, 100 / 4) and press Enter.")
    print("Press Ctrl+C or type 'exit' to quit.")
    print("--------------------------------------------------\n")

    try:
        while True:
            # Local input line prompt (visible as you type, sent only on Enter)
            user_input = input("ESP32-Input> ")
            if user_input.strip().lower() == 'exit':
                print("Exiting console.")
                break
            ser.write((user_input + "\n").encode('utf-8'))
            ser.flush()
            time.sleep(0.3)
    except (KeyboardInterrupt, EOFError):
        print("\nExiting console.")
    finally:
        stop_event.set()
        ser.close()

if __name__ == "__main__":
    main()
