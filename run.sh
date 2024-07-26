#!/usr/bin/env bash
cd LangtonsAnt
lxterminal --geometry=1024x768 -t KIOSK_APP -e ./LangtonsAnt

# ~/.config/openbox/lxde-pi-rc.xml
# <?xml version="1.0" encoding="UTF-8"?>
# <openbox_config xmlns="http://openbox.org/3.4/rc">
#     <applications>
#         <application title="KIOSK_APP">
#             <maximized>yes</maximized>
#             <decor>no</decor>
#         </application>
#     </applications>
# </openbox_config>
