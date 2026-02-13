import os
import subprocess
from multiprocessing import Pool
from pathlib import Path
from typing import List, Union

SDK_ROOT = Path(os.environ['GSDK_ROOT'])

my_env = os.environ.copy()


ROOT_PATH = Path(__file__).parent.parent.absolute()
SCRIPTS_PATH = ROOT_PATH / "xapi"
API3_PATH = SDK_ROOT / "protocol/bluetooth/bgapi_protocol/api3"
ORIG_TEMPLATES_PATH = API3_PATH / "templates"
MODIFIED_TEMPLATES_PATH = SCRIPTS_PATH / "templates"
TEMPLATE_INC_PATHS = [
    ORIG_TEMPLATES_PATH,
    MODIFIED_TEMPLATES_PATH,
    API3_PATH,
    SCRIPTS_PATH,
]
GENERATOR_PATH = API3_PATH / "generate.py"
OUTPUT_PATH = SCRIPTS_PATH / "autogen"

my_env["PYTHONPATH"] = API3_PATH / "raw/api.py"

DEVICE = "wifi"
XAPI_FILE_NAME = "wiseconnect3"
XAPI_PATH = ROOT_PATH /"../../.."/ f"{XAPI_FILE_NAME}.xapi"

DEVICE_CLASSES = ["system", "net_intf", "net_profile", "net_cred", "ap", "common", "scan", "ping", "client", "mqtt_client"]

TEMPLATE_IO_PAIRS = [
    ("app_wifi_commander.ct", f"app_{DEVICE}_commander.c", None),
    ("app_wifi_api.ht", f"app_{DEVICE}_api.h", None),
    ("app_internal_wifi_api.ht", f"app_internal_{DEVICE}_api.h", None),
    ("app_wifi_device_stack_init.ht", f"app_{DEVICE}_class.h", None),
    ("commander_wifi_xapi.h.jinja", f"app_{DEVICE}_xapi_cmd_rx_handler.h", None),
    ("wifi_api.xmlt", f"app_{DEVICE}.xapi", None),
]

for cls in DEVICE_CLASSES:
    TEMPLATE_IO_PAIRS.append(
        ("commander_wifi_lib_handler.c.jinja", f"app_{DEVICE}_{cls}_xapi_cmd_lut_handler.c", cls)
    )


def generate(
    template_path: Path,
    xapi_path: Path,
    output_path: Path,
    include_classes: Union[str, List[str]] = [],
    include_paths=[],
):
    print(f"GENERATE {output_path} from {template_path} using {xapi_path}")
    if isinstance(include_classes, str):
        include_classes = [include_classes]
    command = [
        "python",
        str(GENERATOR_PATH),
        template_path.as_posix(),
        str(xapi_path),
        str(output_path),
    ]
    if include_classes:
        command.append("--include_classes")
        command.append(",".join(include_classes))
    if include_paths:
        include_path_list = []
        for inc_path in include_paths:
            include_path_list.append("-I")
            include_path_list.append(inc_path.as_posix())
    command.extend(include_path_list)
    process_result = subprocess.run(command, cwd=SCRIPTS_PATH)
    if process_result.returncode != 0:
        print(f"Failed to generate {output_path}")
        if process_result.stdout:
            print(process_result.stdout)
        if process_result.stderr:
            print(process_result.stderr)


def generate_all(template_io_pairs, parallel=True):
    generate_args = []
    for template_filename, out_filename, class_name in template_io_pairs:
        if not out_filename:
            out_filename = template_filename.replace(".ct", ".c")
            out_filename = out_filename.replace(".ht", ".h")
            out_filename = out_filename.replace(".c.jinja", ".c")
            out_filename = out_filename.replace(".h.jinja", ".h")
        template_path = Path(template_filename)
        output_path = OUTPUT_PATH / out_filename
        xapi_path = XAPI_PATH
        generate_args.append(
            (template_path, xapi_path, output_path, class_name, TEMPLATE_INC_PATHS)
        )
    if parallel:
        with Pool(processes=12) as pool:
            pool.starmap(generate, generate_args)
    else:
        for args in generate_args:
            generate(*args)



if __name__ == "__main__":
    generate_all(TEMPLATE_IO_PAIRS)