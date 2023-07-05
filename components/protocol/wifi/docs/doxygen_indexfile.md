@mainpage Summary

# Introduction

This document describes the SL Wi-Fi API that exposes the low-level Wi-Fi functions.

The SL Wi-Fi API is the new standardized Silicon Labs Wi-Fi API that provides low level access to Wi-Fi functionality although in most cases the application code doesn't typically interact with the Wi-Fi layer but rely on higher level API and components.
Applications can leverage the high-level connectivity functionalities provided by SL network and IoT sockets that use and/or abstract the low-level Wi-Fi functions.

Other use cases such as calibration, tests, or certifications may need access to low-level Wi-Fi functions.

The SL Wi-Fi API is designed to be a minimal abstraction of key Wi-Fi concepts while also being extensible and allow for simplified customer frameworks. One example of a simplified customer framework is the Callback framework that implements the Wi-Fi event handler and exposes a callback mechanism to the customer.

# API Structure

The SL Wi-Fi API must function on Non-Silicon Labs hardware and as such relies on two host interface abstractions:

* To interact with the host hardware platform.
* To interact with the host buffer management system (typically provided by the network stack).

@image html sl_wifi.drawio.svg SL Wi-Fi API structure

# Data flow

Data from the Wi-Fi driver is of 2 types:

1. Data frames destined for the host network stack.
2. Event data.

All event data is passed through the Wi-Fi event handler that can be defined by the user. Simplified customer frameworks provide an implementation of this event handler and then expose an alternate interface such as a callback mechanism.

@image html wifi_data_flow.drawio.svg SL Wi-Fi data flow
