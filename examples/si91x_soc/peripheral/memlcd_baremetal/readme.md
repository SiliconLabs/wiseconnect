# MEMLCD

## Introduction

- This example project demonstrates use of the Memory Liquid Crystal Display (LCD) module in a bare metal application, using Silicon Labs Graphics Library (glib).

## Prerequisites

- This MEMLCD bare metal example is compatible only with Si917-BRD4338A due to hardware limitation.

## Output

- Renders "MEMLCD Sample App" and "Hello There!" strings on MEMLCD.

## Note

- Define the macro RTE_SSI_ULP_MASTER_MOSI_PIN as 1 (instead of 11) in RTE_Device_917.h. 