#!/bin/bash
echo Script para ejecutar el comando journalctl
journalctl --no-pager -o json-pretty -b|grep '"PRIORITY" : "[0-9]"'|sed "s/,//"|uniq -c
echo Fin