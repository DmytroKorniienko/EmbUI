#!/bin/sh

echo "Preparing resources for EmbUI FS image" 

mkdir -p ./data/css ./data/js
cat html/css/pure-min.css html/css/grids.css | gzip -9 > ./data/css/pure.css.gz
cat html/css/*_default.css | gzip -9 > ./data/css/style.css.gz
cat html/css/*_light.css | gzip -9 > ./data/css/style_light.css.gz
cat html/css/*_dark.css | gzip -9 > ./data/css/style_dark.css.gz

cp html/css/*.jpg ./data/css/
cp html/css/*.webp ./data/css/

cat html/js/*.js | gzip -9 > ./data/js/embui.js.gz
cat html/index.html | gzip -9 > ./data/index.html.gz
cat html/favicon.ico | gzip -9 > ./data/favicon.ico.gz
cp html/.exclude.files ./data/

cd ./data
zip --filesync -r -0 --move --quiet ../data.zip ./*
cd ..
rm -r ./data
echo "Content of data.zip file should be used to create LittleFS image"