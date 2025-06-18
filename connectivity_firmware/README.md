# Connectivity Firmware 

The **connectivity_firmware** directory comprises of three subdirectories: **lite**, **standard**, and **siwx91x_slot_b_images**. The **lite** and **standard** subdirectories each contain a single binary file, representing the default or SlotA images. The **siwx91x_slot_b_images** subdirectory is further divided into two subdirectories, **lite** and **standard**, each containing a SlotB binary file.

Top level directory layout of **connectivity_firmware**:

    ├── lite                                    # A subfolder containing the lite firmware.
    │   ├── SiWG917-B.2.x.x.x.2.x.rps           # This firmware image supports SiWG917M110LGTBA IC OPN and it is valid only for SoC mode.
    │
    ├── standard                                # A subfolder containing the standard firmware.
    │   ├── SiWG917-B.2.x.x.x.0.x.rps           # This firmware image supports all other IC OPNs and is valid for both NCP and SoC modes.
    │
    ├── siwx91x_slot_b_images                   # A subfolder containing SiWx917 SlotB images.
    │   ├── lite/
    │   │   ├── SiWG917-B-SlotB.2.x.x.x.2.x.rps # SlotB lite firmware image (Valid for 8MB Common-Flash SoC mode only).
    │   ├── standard/ 
    │       ├── SiWG917-B-SlotB.2.x.x.x.0.x.rps # SlotB Standard firmware image (Valid for 8MB Common-Flash SoC mode only).
    │
    ├── README.md                               # A file carrying basic details about the directory contents.

> Note:
> - Please refer to the release notes for the features supported by the Standard vs. Lite firmware images.
> - The default firmware images (SlotA) are located directly under the **lite** and **standard** folders.
> - SlotB firmware images are located in the **siwx91x_slot_b_images** subfolders.
> - Please refer to the release notes for the requirements to run the SlotB image.
