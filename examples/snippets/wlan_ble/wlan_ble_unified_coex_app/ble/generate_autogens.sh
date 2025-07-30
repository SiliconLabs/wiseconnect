python3 jinja_renderer.py -t ble_events_autogen.c.j2 -y ble_configs.yml -o ble_event_hdlr_auto_gen.c
python3 jinja_renderer.py -t ble_cmds_autogen.c.j2 -y ble_configs.yml -o ble_cmd_hdlr_auto_gen.c
python3 jinja_renderer.py -t ble_events_autogen.h.j2 -y ble_configs.yml -o ble_event_hdlr_auto_gen.h
python3 jinja_renderer.py -t ble_cmds_autogen.h.j2 -y ble_configs.yml -o ble_cmd_hdlr_auto_gen.h