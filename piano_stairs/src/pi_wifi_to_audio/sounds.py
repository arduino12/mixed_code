import pygame


class Sounds(object):

    SOUNDS_BITS = -16

    def __init__(self, frequency, buffer_size, max_channels, is_mono=False):
        pygame.mixer.pre_init(frequency, self.SOUNDS_BITS, 1 if is_mono else 2, buffer_size)
        pygame.mixer.init()
        pygame.mixer.set_num_channels(max_channels)
        self._sounds = []
    
    def set_playlist(self, file_paths):
        self._sounds = [pygame.mixer.Sound(i) for i in file_paths]

    def play(self, index):
        self._sounds[index % len(self._sounds)].play()
