#!/bin/sh

echo "Preparing resources for EmbUI FS image" 

mkdir -p ./data/css ./data/js ./data/login ./data/extras
cat html/css/*.css | gzip -9 > ./data/css/all.css.gz
cat html/css/wallpaper.jpg | gzip -9 > ./data/css/wallpaper.jpg.gz
cat html/js/*.js | gzip -9 > ./data/js/all.js.gz
cat html/index.html | gzip -9 > ./data/index.html.gz
cat html/favicon.ico | gzip -9 > ./data/favicon.ico.gz
cat html/.exclude.files > ./data/.exclude.files

cat html/login/index.htm | gzip -9 > ./data/login/index.htm.gz
cat html/login/favicon.ico | gzip -9 > ./data/login/favicon.ico.gz

cat html/extras/edit.htm | gzip -9 > ./data/extras/edit.htm.gz
cat html/extras/ace.ico | gzip -9 > ./data/extras/ace.ico.gz
cat html/extras/acefull.js | gzip -9 > ./data/extras/acefull.js.gz
cat html/extras/mode-json.js | gzip -9 > ./data/extras/mode-json.js.gz
cat html/extras/worker-css.js | gzip -9 > ./data/extras/worker-css.js.gz
cat html/extras/worker-html.js | gzip -9 > ./data/extras/worker-html.js.gz
cat html/extras/worker-javascript.js | gzip -9 > ./data/extras/worker-javascript.js.gz
cat html/extras/worker-json.js | gzip -9 > ./data/extras/worker-json.js.gz

cd ./data
zip --filesync -r -0 --move --quiet ../data.zip ./*
cd ..
rm -r ./data
echo "Content of data.zip file should be used to create LittleFS image"