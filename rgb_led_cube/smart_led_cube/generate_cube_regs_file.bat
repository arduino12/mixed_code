@echo off
python generate_cube_regs_file.py --led_format barg- --array_memory flash %*
REM python generate_cube_regs_file.py --led_format barg- --columns_to_masks 0 4 8 12 0 5 9 15 0 6 10 13 0 7 11 14 1 4 9 13 1 5 8 14 1 6 11 12 1 7 10 15 2 4 10 14 2 5 11 13 2 6 8 15 2 7 9 12 3 4 11 15 3 5 10 12 3 6 9 14 3 7 8 13 %*
