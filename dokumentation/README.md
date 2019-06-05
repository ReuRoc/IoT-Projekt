# Dokumentation
Doku kommt hier rein

## Refresh generated Documents when doc source changes

    while true; do inotifywait -r -e modify --exclude=".swp" . && make; echo "FINISHED"; done

## PIN-Belegung esp32
https://cdn.shopify.com/s/files/1/1509/1638/files/ESP_-_32_NodeMCU_Developmentboard_Pinout_Diagram.jpg?4479111012146266271
