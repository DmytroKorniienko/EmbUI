#!/bin/sh

echo "Preparing resources for EmbUI FS image" 

# etag file
tags=etags.txt

refresh_rq=0
tzcsv=https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv

[ -f $tags ] || touch $tags

# check github file for a new hash
freshtag(){
    local url="$1"
    etag=$(curl -sL -I $url | grep etag | awk '{print $2}')
    echo "$url $etag" >> newetags.txt
    if [ $(grep -cs $etag $tags) -eq 0 ] ; then
        #echo "new tag found for $url"
        return 0
    fi
    #echo "old tag for $url"
    return 1
}


mkdir -p ./data/css ./data/js
cat html/css/pure-min.css html/css/grids.css | gzip -9 > ./data/css/pure.css.gz
cat html/css/*_default.css | gzip -9 > ./data/css/style.css.gz
cat html/css/*_light.css | gzip -9 > ./data/css/style_light.css.gz
cat html/css/*_dark.css | gzip -9 > ./data/css/style_dark.css.gz

#cp html/css/*.jpg ./data/css/
cp html/css/*.webp ./data/css/
cp html/css/*.svg ./data/css/

cat html/js/*.js | gzip -9 > ./data/js/embui.js.gz
cat html/index.html | gzip -9 > ./data/index.html.gz
cat html/favicon.ico | gzip -9 > ./data/favicon.ico.gz
cp html/.exclude.files ./data/

# update TZ info
if freshtag ${tzcsv} ; then
    echo "Updating TZ info"
    echo '"label","value"' > ./data/zones.csv
    curl -sL $tzcsv >> ./data/zones.csv
    python3 tzgen.py
    cat ./data/tz.json | gzip -9 > ./data/js/tz.json.gz
    rm -f ./data/tz.json ./data/zones.csv
else
    unzip -o -d ./data/ data.zip "js/tz.json.gz"
fi


cd ./data
zip --filesync -r -0 --quiet ../data.zip ./*
cd ..
rm -r ./data

mv -f newetags.txt $tags

echo "Content of data.zip file should be used to create LittleFS image"
