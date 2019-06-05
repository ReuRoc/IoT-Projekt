# Dokumentation
Doku kommt hier rein

## Refresh generated Documents when doc source changes

    while true; do inotifywait -r -e modify --exclude=".swp" . && make; echo "FINISHED"; done
