#!/bin/sh


# etag file
tags=etags.txt

refresh_rq=0
tzcsv=https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv


USAGE="Usage: `basename $0` [-h] [-f]"

# parse cmd options
while getopts hf OPT; do
    case "$OPT" in
        h)
            echo $USAGE
            exit 0
            ;;
        f)
            echo "Force refresh"
            rm -f $tags
            refresh_rq=1
            ;;
        \?)
            # getopts issues an error message
            echo $USAGE >&2
            exit 1
            ;;
    esac
done

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

echo "Preparing resources for EmbUI FS image"

python3 parce_locale.py
mkdir -p ./data/css ./data/js ./data/locale
cat html/css/pure*.css html/css/grids*.css | gzip -9 > ./data/css/pure.css.gz
cat html/css/*_default.css | gzip -9 > ./data/css/style.css.gz
cat html/css/*_light.css | gzip -9 > ./data/css/style_light.css.gz
cat html/css/*_dark.css | gzip -9 > ./data/css/style_dark.css.gz
cat html/css/embui_light.svg | gzip -9 > ./data/css/embui_light.svg.gz
cat html/css/embui_dark.svg | gzip -9 > ./data/css/embui_dark.svg.gz
cat html/css/custom.css | gzip -9 > ./data/css/custom.css.gz
cat html/locale/emb.json | gzip -9 > ./data/locale/emb.json.gz

#cp html/css/*.jpg ./data/css/
#cp html/css/*.svg ./data/css/
cp html/css/*.webp ./data/css/
cp html/css/*.png ./data/css/

cat html/js/*.js | gzip -9 > ./data/js/embui.js.gz
cat html/index.html | gzip -9 > ./data/index.html.gz
cat html/favicon.ico | gzip -9 > ./data/favicon.ico.gz
cp html/.exclude.files ./data/
cp html/manifest.webmanifest ./data/
#cp html/js/android-chrome-*.png ./data/js/

# update TZ info
if freshtag ${tzcsv} || [ $refresh_rq -eq 1 ] ; then
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
