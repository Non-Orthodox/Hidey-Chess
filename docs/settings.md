# Settings list

## Settings

| Long name | Short name | Type | arguments | Description |
|-|:-:|:-:|-|-|
| network_port | n | int | Network port | Local network port to use. |
| peer_ip_address | a | string | IP address | IP address of peer to connect to. |
| peer_network_port | p | int | Network port | Network port of peer to connect to. |

## Commands

| Long name | arguments | Description |
|-|:-|-|
|  | (command)* | sic. Execute arguments as commands. |
| != | var0/value0 var1/value1 | Test whether both vars/values are not equal. Mixing vars and values is allowed. |
| == | var0/value0 var1/value1 | Test whether both vars/values are equal. Mixing vars and values is allowed. |
| echo | value | Print the string `value`. |
| print | var | Print the contents of setting `var`. |
| set | var value | Set the setting `var` to the literal `value`. |
