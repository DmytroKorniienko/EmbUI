import json

gloss = {}
with open ('../src/i18n.h', 'r', encoding='utf-8') as f:
    idx = 0
    for line_number, line in enumerate(f):
        if '// localization list' in line:
            i = 0
            while i < line.count('|')-1:
                idx = line.find('|', idx, len(line))
                lang = line[idx+1:line.find('|', idx+1, len(line))]
                print("locale", lang)
                gloss[lang] = {}
                i+=1
                idx +=1

        if 'static' in line[0:6]:
            idx = line.find('"', 0, len(line)-1)
            key = line[idx+1 : idx+7]
            for lng in gloss:
                if lng+':' in line:
                    idx = line.find('"', line.find(lng+':', idx, len(line)), len(line))
                    locale = line[idx+1:line.find('"', idx+1, len(line))]
                    if  gloss.get(lng) != None:
                        gloss[lng].update({key : locale})
                    else:
                        temp = {key : locale}
f.close()

f = open('./html/locale/html.json', 'r', encoding='utf-8')
parce = json.loads(f.read())
f.close()
for lng in parce:
    for pair in parce[lng]:
        gloss[lng].update({pair : parce[lng][pair]})
### Этот блок для локализации одним файлом
f = open ('./html/locale/emb.json', 'w', encoding='utf-8')
f.write(json.dumps(gloss, ensure_ascii=False))
f.close()

### Этот блок для разбивки локализации на разные файлы
# for lng in locList:
#     f = open ('./html/locale/'+lng+'.json', 'w', encoding='utf-8')
#     f.write('{"'+lng+'": ' + json.dumps(gloss[lng], ensure_ascii=False) + '}')
#     f.close()