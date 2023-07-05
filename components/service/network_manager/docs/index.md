# Introduction

The Network Management component provides a generic API to manage the state of all IP-based network interfaces.
This includes, but is not restricted to, Ethernet, Wi-Fi, Thread, Matter, Bluetooth, and Wi-SUN.

## Profiles

Network interface configurations are described by "profiles" that hold all the information needed to configure a particular interface.
Every type of network interface has a unique profile structure to optimally store the relevant information.

For example, a Wi-Fi client interface requires an SSID, a security mode, and passphrase (for a security enabled access point).

[Network Profile API](../sl-net-reference/net-profile-functions)

## Credentials

Many network interfaces require secure information such as passphrases or keys to define the network.
It is important that the secure information is kept separate from the network profile. So, the SL Network Manager defines a credential management API that provides access to secure content via a credential ID. The storage of the credential data can be implemented in different ways depending on the support provided by the host platform.  

[Network Credential API](../sl-net-reference/net-credential-functions)

## Certificates

In addition to credential information, some networks and network services also require access to certificate storage to verify remote hosts or prove client identity.
The Network Management component provides API to manage individual certificates as well as a bulk certificate store.

[Network Certificate API](../sl-net-reference/net-certificate-functions)
