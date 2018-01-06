#!/usr/bin/env bash
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd ..
python3.6 ./infra/run/app_runner.py --interface ipython --app "piano_stairs.src.pi_wifi_to_audio.wifi_to_audio" "$@"
