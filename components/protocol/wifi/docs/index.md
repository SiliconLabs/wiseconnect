# Introduction

The SL Wi-Fi API provides low level access to Wi-Fi functionality although in most cases the application code doesn't typically interact with the Wi-Fi layer but would rely on higher level API and components. 
Applications can leverage the high-level connectivity functionalities provided by SL Network and IoT Sockets which use and/or abstract out the low-level Wi-Fi functions.

Other use cases such as calibration, tests, or certification may need access to those low-level Wi-Fi functions. This document describes the SL Wi-Fi API, the new standardized Silicon Labs' Wi-Fi API that exposes those low-level Wi-Fi functions.

The SL Wi-Fi API is designed to be a minimal abstraction of key Wi-Fi concepts while also being extensible and allow for simplified customer frameworks. One example of a simplified customer framework is the Callback Framework that implements the Wi-Fi event handler and exposes a callback mechanism to the customer.

# API Structure
The SL Wi-Fi API must function on non-Silicon Labs hardware and as such relies on two host interface abstractions; one to interact with the host hardware platform, and another to interact with the host buffer management system (typically provided by the network stack).

@image html sl_wifi.drawio.svg SL Wi-Fi API structure

# Data flow
Data from the Wi-Fi driver is classified into two distinct types; data frames destined for the host network stack, and event data. 
All event data is passed through the Wi-Fi event handler that can be defined by the user. Simplified customer frameworks provide an implementation of this event handler and then expose an alternate interface such as a callback mechanism.

@image html wifi_data_flow.drawio.svg SL Wi-Fi data flow
