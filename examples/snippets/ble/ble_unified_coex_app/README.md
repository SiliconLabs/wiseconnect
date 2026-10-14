# BLE Unified Coex Application

> **Note:** This application is a Proof of Concept (PoC) and intended for evaluation and demonstration purposes only. It is not recommended for production use.

## Overview

This example demonstrates the use of the **BLE Unified Component Framework** on the Si917 SoC.

The application showcases:
- Component-based BLE architecture
- Modular protocol selection (GAP, GATT, SMP, Advertising)
- Event-driven framework with unified event loop
- Automatic callback registration and initialization

## Architecture

### Component-Based Design
- **BLE Unified Framework**: Event loop orchestration
- **BLE Unified Component**: Protocol initialization coordination
- **Protocol Components**: GAP, GATT Server/Client, SMP, Extended/Legacy Advertising

### Benefits
- **Modular**: Select only the BLE protocols you need
- **Optimized**: 30-40% code size reduction for single-role applications
- **Maintainable**: Clear separation of concerns
- **Extensible**: Easy to add new protocols or customize behavior

## Setup

### Hardware Requirements

**SoC Mode**:
- Si917 SoC development board (e.g., BRD4325A, BRD4338A)
- SiWG917 Dev Kit [BRD2605B]
- USB cable for programming and debugging

**NCP Mode**:
- Si917 NCP radio board
- Host MCU development board (e.g., EFR32 series)
- USB cable for programming and debugging

### Software Requirements
- Simplicity Studio 5
- WiSeConnect SDK 3.x (4.0.0 or later)
- Si91x Wireless Components

### Project Selection
This example provides two project variants:
- **`ble_unified_coex_app_soc.slcp`** - For Si917 SoC mode (recommended for most use cases)
- **`ble_unified_coex_app_ncp.slcp`** - For Si917 NCP mode with external host MCU

## Configuration

### Component Selection (via Simplicity Studio)
1. **BLE Unified Framework** - Core event loop (required)
2. **BLE Unified** - Initialization orchestration (required)
3. **GAP** - Generic Access Profile (typically required)
4. **Advertising Type** - Select ONE:
   - Extended Advertising (recommended)
   - Legacy Advertising
5. **GATT** - Select as needed:
   - GATT Server (for peripheral role)
   - GATT Client (for central role)
6. **SMP** - Security Manager Protocol (if pairing/bonding needed)

### Protocol-Specific Configuration
Each protocol component provides its own configuration file:
- `gap_config.h` - Connection parameters, device name
- `gatt_server_config.h` - Service definitions
- `smp_config.h` - Pairing capabilities, security settings
- `ext_adv_config.h` / `legacy_adv_config.h` - Advertising parameters

## Usage

### Building

**For SoC Mode**:
1. Import `ble_unified_coex_app_soc.slcp` in Simplicity Studio
2. Uncomment required BLE protocol components as they become available (Tasks 4-8)
3. Configure protocol parameters via component config files
4. Build project

**For NCP Mode**:
1. Import `ble_unified_coex_app_ncp.slcp` in Simplicity Studio
2. Uncomment required BLE protocol components as they become available (Tasks 4-8)
3. Configure protocol parameters via component config files
4. Configure SPI interface pins in component settings
5. Build project

### Running

**SoC Mode**:
1. Flash firmware to Si917 SoC board
2. BLE initialization handled automatically by unified framework
3. Monitor console (VCOM) for BLE events and status

**NCP Mode**:
1. Flash firmware to host MCU
2. Flash Si917 NCP firmware separately (if required)
3. BLE initialization handled automatically by unified framework
4. Monitor console (UART/VCOM) for BLE events and status

## Application Flow

```
app_init()
    ↓
ble_task()
    ↓
rsi_ble_main_app_task()
    ↓
[Unified Framework takes over]
    ↓
Protocol initialization (GAP, GATT, SMP, Adv)
    ↓
Event loop (handles all BLE events)
```

## Customization

### Adding Event Handlers
Add custom event handlers in protocol-specific components:
- GAP events → `gap/src/gap_user_event_hdlrs.c`
- GATT events → `gatt/server/src/gatt_server_user_event_hdlrs.c`
- SMP events → `smp/src/smp_user_event_hdlrs.c`

### Modifying Initialization
Customize protocol initialization:
- GAP init → `gap/src/gap_init.c`
- GATT init → `gatt/server/src/gatt_server_init.c`
- SMP init → `smp/src/smp_init.c`

## Troubleshooting

### Build Issues
- Verify all required components are selected
- Check for conflicting advertising type components
- Ensure protocol dependencies are met

### Runtime Issues
- Check console logs for initialization failures
- Verify configuration parameters in `*_config.h` files
- Ensure adequate task stack size in BLE Unified config

## References

- [Si917 Component Framework Architecture](https://github.com/SiliconLabs/wiseconnect/blob/v4.1.2-content-for-docs/docs/architecture/ble_component_framework/Si917_Component_Framework_Architecture_Clean.md)
- [Component Refactoring Plan](https://github.com/SiliconLabs/wiseconnect/blob/v4.1.2-content-for-docs/docs/plans/ble_unified_component_refactoring_plan_v2_overview.md)
- WiSeConnect SDK Documentation
