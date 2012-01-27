#!/usr/bin/env python

import os, signal, time
from subprocess import *

class trigger:
    def __init__(self, threshold, interval, check_fn):
        self.threshold = threshold
        self.interval = interval
        self.check_fn = check_fn

        self.signal_fn = self.signal_handler
        self.trigger_state = 'N'
        self.load = 0.0

    def signal_handler(signal_number):
        pass

    def run_pipe(self):
        pass

    def start_log(self):
        print 'Starting logging process at load: %f' % self.load
        self.trigger_state = 'Y'

    def kill_log(self):
        print 'Stopping logging process at load %f' % self.load
        self.trigger_state = 'N'

class cpu_trigger(trigger):
    def __init__(self, threshold=0.90, interval=5.0):
        trigger.__init__(self, threshold, interval, check_fn=self.cpu_check)
        self.commands = [
                'ps -eo pcpu,pid,user,args',
                'sort -nk1',
                'tail -10',
                ]
        self.test_command = ['uptime']
        self.text = 'CPU Load'
        
    def cpu_check(self, debug_mode='N'):
        proc = Popen(self.test_command,stdout=PIPE,stderr=PIPE)
        (out,err) = proc.communicate()
        self.load = float(out.strip().split()[-3])
        if self.load > self.threshold and self.trigger_state == 'N':
            self.start_log()
        elif self.load < self.threshold and self.trigger_state == 'Y':
            self.kill_log()
        elif debug_mode == 'Y':
            print 'DEBUG : %s %f' % (self.text, self.load)

class mem_trigger:
    def __init__(self):
        pass

def main():
    trig1 = cpu_trigger()
    while True:
        trig1.check_fn('Y')
        time.sleep(3)

if __name__ == '__main__':
    main()
