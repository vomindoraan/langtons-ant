#!/usr/bin/env bash
SRC_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." &>/dev/null && pwd)"
APP="${1:-$SRC_DIR/LangtonsAnt}"

cd "$SRC_DIR"
echo "Running '$APP'..."
if command -v lxterminal &>/dev/null; then
    lxterminal --geometry=1024x768 -t KIOSK_APP -e "$APP"
elif command -v osascript &>/dev/null; then
    osascript -e "tell app \"Terminal\" to do script \"$APP\""
    # TODO: Set geometry, position, font name and size
else
    "$APP"
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
