#!/usr/bin/env python

import os, signal, time, sys
from subprocess import *

class trigger:
    def __init__(self, threshold, interval, check_fn):
        self.threshold = threshold
        self.interval = interval
        self.check_fn = check_fn

        self.signal_fn = self.signal_handler
        self.trigger_state = 'N'
        self.load = 0.0

    def signal_handler(self, signal_number, stack_frame):
        if signal_number == 1:
            try:
                self.file_handle.close()
            except:
                pass
            sys.exit(0)

    def run_pipe(self):
        proc_lst = []
        for idx,command in enumerate(self.commands):
            lst = command.split()
            if idx == 0:
                proc = Popen(lst, stdout=PIPE, stdin=PIPE)
            else:
                proc = Popen(lst, stdin=proc_lst[idx-1].stdout, \
                        stdout=PIPE, stderr=PIPE)
            proc_lst.append(proc)
        (out, err) = proc_lst[len(proc_lst)-1].communicate()    
        return (out, err)

    def start_log(self):
        print 'Starting logging process at load: %f' % self.load
        self.trigger_state = 'Y'
        child = os.fork()

        if child == 0:
            self.file_handle = open(self.log_loc, 'wb')
            while True:
                signal.signal(1, self.signal_handler)
                (out, err) = self.run_pipe()
                self.file_handle.write('%s\n' % out)
                time.sleep(self.interval)
        else:
            self.child_pid = child

    def kill_log(self):
        print 'Stopping logging process at load %f' % self.load
        self.trigger_state = 'N'
        if self.child_pid:
            killer_proc = Popen(['kill','-1',str(self.child_pid)])
            killer_proc.communicate()

class cpu_trigger(trigger):
    def __init__(self, threshold=0.95, interval=5.0):
        trigger.__init__(self, threshold, interval, check_fn=self.cpu_check)
        self.commands = [
                'ps -eo pcpu,pid,user,args',
                'sort -nk1',
                'tail -10',
                ]
        self.test_command = ['uptime']
        self.text = 'CPU Load'
        self.log_loc = 'cpu_log.txt'
        
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
        trig1.run_pipe()
        time.sleep(3)

if __name__ == '__main__':
    main()
