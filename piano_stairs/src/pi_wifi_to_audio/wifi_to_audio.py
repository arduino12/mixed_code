import os
import time
import logging

from infra.app import app
from piano_stairs.src.pi_wifi_to_audio import constants

from piano_stairs.src.pi_wifi_to_audio import sounds
from piano_stairs.src.pi_wifi_to_audio import nodes


class WifiToAudio(app.App):
    STAIRS_COUNT = 12
    STAIRS_EDGE = 1 | 1 << (STAIRS_COUNT - 1)
    AUTO_CHANGE_PLAYLIST_TIME = 2 * 60
    BUTTOM_STAIR_CHANGE_PLAYLIST_TIME = 2.2
    STAIR_CHANGE_DIRACTION_TIME = 5
    
    SOUNDS_FREQUENCY = 44100
    SOUNDS_BUFFER_SIZE = 4096
    SOUNDS_MAX_CHANNELS = 32

    NODES_IP_PORT = ('', 8888)
    NODES_MAPPING_BITMASK = {
        'NODE-753CD': {1 << i: 1 << (i * 2) for i in range(0, STAIRS_COUNT // 2)},
        'NODE-7247C': {1 << i: 2 << (i * 2) for i in range(0, STAIRS_COUNT // 2)},
        # 'NODE-753CD': dict(zip([1 << i for i in range(0, 6)], [1 << i for i in range(0, 12, 2)])),
        # 'NODE-753CD': [1 << i for i in range(0, 12, 2)],
        # 'NODE-7247C': [1 << i for i in range(1, 12, 2)],
    }
    
    def __init__(self):
        app.App.__init__(self, constants)
        self._modules.extend((nodes, sounds))

        self.old_time = time.time()
        self.change_playlist_time = self.old_time
        self.old_press_time = 0
        self.is_reverse_playlist = False
        self._logger = logging.getLogger(WifiToAudio.__name__)

        self.sounds_folder = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(__file__))), 'sounds')
        walk = os.walk(self.sounds_folder)
        next(walk)
        self.playlists = {os.path.basename(path): sorted(files) for path, _, files in walk}
        for i in self.playlists.items():
            self._logger.info('%s:\n%s', *i)
        
        self.playlist_index = 0

        self.sounds = sounds.Sounds(self.SOUNDS_FREQUENCY, self.SOUNDS_BUFFER_SIZE, self.SOUNDS_MAX_CHANNELS)
        self.set_playlist(0)
        self.nodes = nodes.Nodes(self.NODES_IP_PORT, self.NODES_MAPPING_BITMASK, self.step_pressed, self.loop)
        self.nodes.start()

    def set_playlist(self, offset):
        self.playlist_index += offset
        self.playlist_index %= len(self.playlists)
        self._logger.info('set_playlist: %s', self.playlist_index)
        key = list(self.playlists.keys())[self.playlist_index]
        self.sounds.set_playlist([os.path.join(self.sounds_folder, key, i) for i in self.playlists[key]])

    def loop(self):
        cur_time = time.time()
        edge_down_time = cur_time - self.change_playlist_time
        edge_change_playlist = edge_down_time > self.BUTTOM_STAIR_CHANGE_PLAYLIST_TIME
        if cur_time - self.old_time > self.AUTO_CHANGE_PLAYLIST_TIME or edge_change_playlist:
            self.set_playlist(1 if self.nodes._old_steps_mask & 1 else -1)
            if edge_change_playlist and edge_down_time < 3600:
                self.sounds.play(0 if self.nodes._old_steps_mask & 1 else self.STAIRS_COUNT - 1)
                self._logger.info('edge stair change playlist')
            else:
                self._logger.info('auto change playlist')
            self.old_time = cur_time
            self.change_playlist_time = cur_time
            self.is_reverse_playlist = False

        if not (self.nodes._old_steps_mask & self.STAIRS_EDGE):
            self.change_playlist_time = cur_time

    def step_pressed(self, index):
        self._logger.info('step_pressed: %s', index)
        cur_time = time.time()
        self.old_time = cur_time
        self.change_playlist_time = cur_time

        if cur_time - self.old_press_time > self.STAIR_CHANGE_DIRACTION_TIME:
            if self.is_reverse_playlist != (index >= self.STAIRS_COUNT / 2):
                self.sounds._sounds.reverse()
                self.is_reverse_playlist = not self.is_reverse_playlist
        self.old_press_time = cur_time 
        self.sounds.play(index)

    def __exit__(self):
        self.nodes.stop()
        app.App.__exit__(self)
