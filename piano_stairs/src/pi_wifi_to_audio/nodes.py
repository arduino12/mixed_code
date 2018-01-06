import socket
import logging
import threading


class Nodes(threading.Thread):

    def __init__(self, ip_port, mapping_bitmask, step_pressed, loop):
        threading.Thread.__init__(self, target=self._recv_loop, daemon=True)
        self._ip_port = ip_port
        self._mapping_bitmask = mapping_bitmask
        self._step_pressed = step_pressed
        self._loop = loop
        self._running = True
        self._old_steps_mask = 0
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._logger = logging.getLogger(Nodes.__name__)
        self._logger.info('Nodes UDP server listen on %s for %s', self._ip_port, list(self._mapping_bitmask))

    def _recv_loop(self):
        self._sock.bind(self._ip_port)
        self._sock.settimeout(0.01)
        # self._sock.setblocking(False)
        while self._running:
            self._loop()
            try:
                data, (addr, _) = self._sock.recvfrom(2)
            except Exception:
                continue
            if not data:
                continue
            self._logger.debug('message from %s: %s', addr, data)
            mapping_bitmask = self._mapping_bitmask.get(addr)
            if mapping_bitmask is None:
                try:
                    name = socket.gethostbyaddr(addr)[0]
                except Exception:
                    self._logger.exception('gethostbyaddr')
                    continue
                if name not in self._mapping_bitmask:
                    continue
                self._logger.info('%s is on %s', name, addr)
                mapping_bitmask = self._mapping_bitmask[addr] = self._mapping_bitmask[name]
            
            node_mask = int(data)
            steps_mask = 0
            for node_bit, steps_bit in mapping_bitmask.items():
                if node_mask & node_bit:
                    steps_mask |= steps_bit
            
            xored_steps_mask = self._old_steps_mask ^ steps_mask
            bit, index = 1, 0
            while xored_steps_mask >= bit:
                if xored_steps_mask & bit:
                    if steps_mask & bit:
                        self._step_pressed(index)
                index += 1
                bit *= 2
            self._old_steps_mask = steps_mask

    def stop(self):
        self._running = False
        self.join()
        self._sock.close()
