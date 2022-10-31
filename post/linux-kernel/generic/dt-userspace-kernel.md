# How to expose dt to user space /proc/device-tree

For the purpose of debugging, it could be useful to expose a DT to the
user space. The CONFIG_PROC_DEVICETREE configuration variable will
do that for you. You can then explore and walk through a DT in
/proc/device-tree .
