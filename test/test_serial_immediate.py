import pytest
from polargraph import Polargraph
import env

# Immediate commands are interpreted quickly

CMD_SETPENWIDTH = 'C02,{pen_width},END'
CMD_CHANGEMOTORSPEED = 'C03,{motor_speed},END'
CMD_CHANGEMOTORACCEL = 'C04,{motor_accel},END'
CMD_SETMACHINESIZE = 'C24,{width},{height},END'
CMD_SETMACHINENAME = 'C25,{name},END'
CMD_GETMACHINEDETAILS = 'C26,END'
CMD_RESETEEPROM = 'C27,END'
CMD_SETMACHINEMMPERREV = 'C29,{mm_per_rev},END'
CMD_SETMACHINESTEPSPERREV = 'C30,{steps_per_rev},END'
CMD_SETMOTORSPEED = 'C31,{motor_speed},END'
CMD_SETMOTORACCEL = 'C32,{motor_accel},END'
CMD_SETMACHINESTEPMULTIPLIER = 'C37,{step_multiplier},END'
CMD_SETPENLIFTRANGE = 'C45,{down_pos},{up_pos},END'
CMD_SET_ROVE_AREA = 'C21,{pos_x},{pos_y},{width},{height},END'
CMD_SET_DEBUGCOMMS = 'C47,END'
CMD_MODE_STORE_COMMANDS = 'C33,{newfilename},{replace},END'
CMD_MODE_EXEC_FROM_STORE = 'C34,{filename},END'
CMD_MODE_LIVE = 'C35,END'
CMD_START_TEXT = 'C38,END'

class TestImmediateTests(object):

    pg = Polargraph()

    @classmethod
    def setup_class(self):
        print 'Setting up'
        opened = self.pg.start_serial_comms(comm_port=env.COMM_PORT)
        if not opened:
            print 'There was a problem opening the communications port (%s). '\
                'It should be entered exactly as you see it in' \
                'your operating system.' % env.COMM_PORT
            exit()
    
    @classmethod
    def teardown_class(self):
        print 'tearing down'
        self.pg.close()


    def test_set_pen_width(self):
        
        # Send the penwidth command
        command = CMD_SETPENWIDTH.format(pen_width=1.0)
        self.pg.write_command(command)
        lines = list()
        while 'MSG,C:{}'.format(command) not in lines:
            lines.append(self.pg.read_line())
        assert 'READY_200' in lines
        assert 'MSG,I,Changed Pen width to 1.00mm' in lines
        
        # Get the penWidth from the machine back
        command = CMD_GETMACHINEDETAILS
        self.pg.write_command(command)
        lines = list()
        while 'MSG,C:{}'.format(command) not in lines:
            lines.append(self.pg.read_line())
        assert 'READY_200' in lines
        assert 'PGPENWIDTH,1.00,END' in lines

        # Send the penwidth command
        command = CMD_SETPENWIDTH.format(pen_width=1.5)
        self.pg.write_command(command)
        lines = list()
        while 'MSG,C:{}'.format(command) not in lines:
            lines.append(self.pg.read_line())
        assert 'READY_200' in lines
        assert 'MSG,I,Changed Pen width to 1.50mm' in lines

        # Get the penWidth from the machine back
        command = CMD_GETMACHINEDETAILS
        self.pg.write_command(command)
        lines = list()
        while 'MSG,C:{}'.format(command) not in lines:
            lines.append(self.pg.read_line())
        assert 'READY_200' in lines
        assert 'PGPENWIDTH,1.50,END' in lines



    def test_andswer(self):
        assert 4 == 4
    