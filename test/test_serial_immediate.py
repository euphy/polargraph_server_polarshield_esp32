import pytest
from polargraph import Polargraph
import env

# Immediate commands are interpreted quickly

CMD_SETPENWIDTH = 'C02,{pen_width},END'
# CMD_CHANGEMOTORSPEED = 'C03,{motor_speed},END'
# CMD_CHANGEMOTORACCEL = 'C04,{motor_accel},END'
CMD_SETMACHINESIZE = 'C24,{width},{height},END'
# CMD_SETMACHINENAME = 'C25,{name},END'
CMD_GETMACHINEDETAILS = 'C26,END'
# CMD_RESETEEPROM = 'C27,END'
CMD_SETMACHINEMMPERREV = 'C29,{mm_per_rev},END'
CMD_SETMACHINESTEPSPERREV = 'C30,{steps_per_rev},END'
CMD_SETMOTORSPEED = 'C31,{motor_speed},END'
CMD_SETMOTORACCEL = 'C32,{motor_accel},END'
CMD_SETMACHINESTEPMULTIPLIER = 'C37,{step_multiplier},END'
# CMD_SETPENLIFTRANGE = 'C45,{down_pos},{up_pos},END'
# CMD_SET_ROVE_AREA = 'C21,{pos_x},{pos_y},{width},{height},END'
# CMD_SET_DEBUGCOMMS = 'C47,END'
# CMD_MODE_STORE_COMMANDS = 'C33,{newfilename},{replace},END'
# CMD_MODE_EXEC_FROM_STORE = 'C34,{filename},END'
# CMD_MODE_LIVE = 'C35,END'
# CMD_START_TEXT = 'C38,END'

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

    def get_response_to(self, command):
        self.pg.write_command(command)
        response_lines = list()
        while 'MSG,C:{}'.format(command) not in response_lines:
            response_lines.append(self.pg.read_line())
        return response_lines

    def get_machine_spec(self):
        command = CMD_GETMACHINEDETAILS
        response = self.get_response_to(command)
        return response

    def get_speed_spec(self):

        speed_spec = [x for x in self.get_machine_spec() if x.startswith('PGSPEED')]
        assert len(speed_spec) == 1

        splitted_spec = speed_spec[0].split(',')
        current_speed = splitted_spec[1]
        current_accel = splitted_spec[2]

        return {'speed': current_speed, 'accel': current_accel}

    @pytest.mark.skip()
    def test_set_pen_width(self):
        command = CMD_SETPENWIDTH.format(pen_width=1.0)
        assert 'MSG,I,Changed Pen width to 1.00mm' in self.get_response_to(command)
        assert 'PGPENWIDTH,1.00,END' in self.get_machine_spec()

        command = CMD_SETPENWIDTH.format(pen_width=1.5)
        assert 'MSG,I,Changed Pen width to 1.50mm' in self.get_response_to(command)
        assert 'PGPENWIDTH,1.50,END' in self.get_machine_spec()

    def test_set_motor_speed(self):
        command = CMD_SETMOTORSPEED.format(motor_speed=1000)
        assert 'New max speed: 1000.00' in self.get_response_to(command)
        assert '1000.00' == self.get_speed_spec().get('speed')

        command = CMD_SETMOTORSPEED.format(motor_speed=2000)
        assert 'New max speed: 2000.00' in self.get_response_to(command)
        assert '2000.00' == self.get_speed_spec().get('speed')

    def test_set_motor_accel(self):
        command = CMD_SETMOTORACCEL.format(motor_accel=1000)
        assert 'New acceleration: 1000.00' in self.get_response_to(command)
        assert '1000.00' == self.get_speed_spec().get('accel')

        command = CMD_SETMOTORACCEL.format(motor_accel=2000)
        assert 'New acceleration: 2000.00' in self.get_response_to(command)
        assert '2000.00' == self.get_speed_spec().get('accel')

    def test_set_mm_per_rev(self):
        command = CMD_SETMACHINEMMPERREV.format(mm_per_rev=105)
        assert 'New mm per rev: 105.00' in self.get_response_to(command)
        assert 'PGMMPERREV,105.00,END' in self.get_machine_spec()

        command = CMD_SETMACHINEMMPERREV.format(mm_per_rev=95)
        assert 'New mm per rev: 95.00' in self.get_response_to(command)
        assert 'PGMMPERREV,95.00,END' in self.get_machine_spec()

    def test_set_steps_per_rev(self):
        command = CMD_SETMACHINESTEPSPERREV.format(steps_per_rev=400)
        assert 'New steps per rev: 400' in self.get_response_to(command)
        assert 'PGSTEPSPERREV,400,END' in self.get_machine_spec()

        command = CMD_SETMACHINESTEPSPERREV.format(steps_per_rev=200)
        assert 'New steps per rev: 200' in self.get_response_to(command)
        assert 'PGSTEPSPERREV,200,END' in self.get_machine_spec()

    def test_set_step_multiplier(self):
        command = CMD_SETMACHINESTEPMULTIPLIER.format(step_multiplier=16)
        assert 'New step multiplier: 16' in self.get_response_to(command)
        assert 'PGSTEPMULTIPLIER,16,END' in self.get_machine_spec()

        command = CMD_SETMACHINESTEPMULTIPLIER.format(step_multiplier=8)
        assert 'New step multiplier: 8' in self.get_response_to(command)
        assert 'PGSTEPMULTIPLIER,8,END' in self.get_machine_spec()

    def test_set_size(self):
        command = CMD_SETMACHINESIZE.format(width=500, height=600)
        assert 'New size: 500, 600' in self.get_response_to(command)
        assert 'PGSIZE,500,600,END' in self.get_machine_spec()

        command = CMD_SETMACHINESIZE.format(width=700, height=800)
        assert 'New size: 700, 800' in self.get_response_to(command)
        assert 'PGSIZE,700,800,END' in self.get_machine_spec()

