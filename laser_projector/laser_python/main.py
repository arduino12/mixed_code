# import serial
# import time
import math

X_AXIS_MASK = 1
Y_AXIS_MASK = 2
STEPS_PER_ANGLE = 1


class Steps(object):
    def __init__(self, x, y):
        p = (x, y)
        self.dx, self.dy = map(abs, p)
        self.sx, self.sy = map(lambda i: 0 if i == 0 else 1 if i > 0 else -1, p)
        self.err = (self.dx if self.dx > self.dy else -self.dy) / 2
        
    def step(self):
        e = self.err
        axes_mask = 0
        if e >= -self.dx:
            self.err -= self.dy
            axes_mask |= X_AXIS_MASK
        if e < self.dy:
            self.err += self.dx
            axes_mask |= Y_AXIS_MASK
        return axes_mask


class Axes(object):
    STEPS = "<->"
    Z = 5 ** 2

    x_step, y_step = 0, 0
    x_point, y_point = 0, 0

    @classmethod
    def relative_point(cls, x, y):
        cls.absolute_point(x - cls.x_point, y - cls.y_point)

    @classmethod
    def absolute_point(cls, x, y):
        steps = Steps(x, y)
        rx, ry = 0, 0
        for _ in xrange(max(steps.dx, steps.dy)):
            axes_mask = steps.step()
            if axes_mask & X_AXIS_MASK:
                rx += steps.sx
            if axes_mask & Y_AXIS_MASK:
                ry += steps.sy
            sx, sy = cls.point_to_steps(rx, ry)
            cls.absolute_steps(sx, sy)
        cls.x_point, cls.y_point = x, y

    @classmethod
    def absolute_steps(cls, x, y):
        cls.relative_steps(x - cls.x_step, y - cls.y_step)

    @classmethod
    def relative_steps(cls, x, y):
        cls.x_step += x
        cls.y_step += y
        steps = Steps(x, y)
        for _ in xrange(max(steps.dx, steps.dy)):
            axes_mask = steps.step()
            print " ".join(map(lambda i: cls.STEPS[i + 1],
                               (steps.sx if axes_mask & X_AXIS_MASK else 0,
                                steps.sy if axes_mask & Y_AXIS_MASK else 0)))

    @classmethod
    def point_to_angels(cls, x, y):
        n = math.sqrt(x ** 2 + y ** 2 + cls.Z)
        a = math.asin(y / n)
        return map(math.degrees, (math.asin(x / n / math.cos(a)), a))

    @classmethod
    def point_to_steps(cls, x, y):
        angels = cls.point_to_angels(x, y)
        return [int(i * STEPS_PER_ANGLE) for i in angels]


def _main():
    _interactive_shell_embed_arguments = {
        "banner1": "2 Axes Laser Projector (shell interface)",
        "confirm_exit": False,
        "exit_msg": "Bye :)",
    }

    from IPython.terminal.embed import InteractiveShellEmbed as _InteractiveShellEmbed
    _InteractiveShellEmbed(**_interactive_shell_embed_arguments)()
    # from internal.ipython_shell import _Ipython_shell
    # _Ipython_shell()

if __name__ == "__main__":
    _main()


# steps is angels * magic- so work with position (in steps)
# 
# class Axis(object):
#     AXIS_STEPS_PER_ANGLE = 70
#     AXIS_ANGLE_PER_ANALOG = 90
#     
#     def __init__(self):
#         self.position = self.get_analog_position() * self.AXIS_STEPS_PER_ANGLE
#         
#     def get_analog_position(self):
#         return 0 / self.AXIS_ANGLE_PER_ANALOG

#    @classmethod
#    def move_relative(cls, x1, y1):
#        x0, y0 = cls.current_x, cls.current_y
#        big = abs(y1 - y0) > abs(x1 - x0)
#        if big:
#            x0, y0 = y0, x0
#            x1, y1 = y1, x1
#        if x0 > dx:
#            x0, x1 = x1, x0
#            y0, y1 = y1, y0
#        
#        dx = x1 - x0
#        dy = abs(y1 - y0)
#        
#        err = dx / 2
#        ystep = 1 if y1 > y0 else -1
#        
#        while x0 <= x1:
#            if
#        
#        diff = (dx, dy)
#        steps = [1 if i > 0 else -1 for i in diff]
#        diff = map(abs, diff)
#        count = max(diff)
#        axis = diff.index(count)
#        tmp = 0
#        for i in xrange(count):
#            cls.step(axis, steps[axis])
#            tmp += diff[1 - axis]/count
#            if tmp >= count:
#                tmp -= count
#                cls.step(1 - axis, steps[1 - axis])

#    @classmethod
#    def move_relative_1(cls, dx, dy):
#        diff = (dx, dy)
#        steps = [1 if i > 0 else -1 for i in diff]
#        diff = map(abs, diff)
#        count = max(diff)
#        axis = diff.index(count)
#        tmp = 0
#        for i in xrange(count):
#            cls.step(axis, steps[axis])
#            tmp += diff[1 - axis]
#            if tmp >= count:
#                tmp -= count
#                cls.step(1 - axis, steps[1 - axis])
