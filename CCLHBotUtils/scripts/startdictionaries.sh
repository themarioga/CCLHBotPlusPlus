#!/bin/sh

while true
do

echo "Iniciando CCLHbot"
./CCLHBotDictionaries/bin/CCLHBotDictionaries cclh.db

echo "Iniciando Bot de limpieza"
./CCLHBotUtils/echobot/echobot &

sleep 5

pkill -9 echobot

sleep 1

done
