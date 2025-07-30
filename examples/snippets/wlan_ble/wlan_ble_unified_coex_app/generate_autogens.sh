python3 ble/jinja_renderer.py -t utils/command_template.j2 -y root_configs.yml -o root_auto_gen.c
python3 ble/jinja_renderer.py -t utils/command_template.j2 -y ble/btc_configs.yml -o btc_auto_gen.c