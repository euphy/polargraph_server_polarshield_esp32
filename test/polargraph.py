import os
import serial
import time

import sys


class Polargraph():
    """
    This is a crude model of a drawing machine. It includes it's drawing state
    as well as the state workonof the communications lines to the machine and the
    queue of commands going to it.

    """

    def __init__(self):
        self.time_started = time.time()

    serial_port = None

    # State
    ready = False
    commands_written_count = 0

    # Timing
    command_sent_at_time = 0

    def start_serial_comms(self, comm_port, baud_rate=57600):
        """
        Attempts to connect this machine to it's comm_port.
        :param comm_port: The name of the comm port
        :param baud_rate: The speed to open the serial connection at

        :return:
        """
        try:
            self.serial_port = serial.Serial(comm_port, baudrate=baud_rate)
            print("Connected successfully to %s (%s)." % (comm_port, serial))

            while not self.ready:
                self.read_line()
                
            return True

        except Exception as e:
            print("Oh there was an exception loading the port %s: %s" % (comm_port, e))
            print(e)
            self.serial_port = None
            return False

    def read_line(self, echo=True):
        l = self.serial_port.readline().strip()
        print("received: {:s}".format(l))
        if l.startswith("READY"):
            self.ready = True

        return l

    def write_command(self, command):
        if self.ready:
            print("Command {}: {}"
                .format(self.commands_written_count, command))
            self.serial_port.write(command + "\n")
            self.ready = False
            self.command_sent_at_time = time.time()

    def close(self):
        self.serial_port.close()
        print("Finished sending {:d} commands in {:0.2f}"
            .format(self.commands_written_count, 
                    time.time() - self.time_started))
        
