#!/bin/sh

echo "Preparing resources for EmbUI FS image" 

mkdir -p ./data/css ./data/js
cat html/css/*.css | gzip -9 > ./data/css/all.css.gz
cat html/js/*.js | gzip -9 > ./data/js/all.js.gz
cat html/index.html | gzip -9 > ./data/index.html.gz
cat html/favicon.ico | gzip -9 > ./data/favicon.ico.gz

cd ./data
zip --filesync -r -0 --move --quiet ../data.zip ./*
cd ..
rm -r ./data
echo "Content of data.zip file should be used to create LittleFS image"