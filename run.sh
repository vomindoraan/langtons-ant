#!/usr/bin/env bash
cd LangtonsAnt

if command -v lxterminal &>/dev/null; then
    lxterminal --geometry=1024x768 -t KIOSK_APP -e ./LangtonsAnt
elif command -v osascript &>/dev/null; then
    osascript -e 'tell app "Terminal" to do script "./LangtonsAnt"'
    # TODO: Set geometry, position, font name and size
fi

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
