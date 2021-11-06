# based on from https://pythonexamples.org/python-csv-to-json/
import csv 
import json 

def csv_to_json(csvFilePath, jsonFilePath):
    #jsonArray = [{"label": "", "value": "000_"}]
    jsonArray = [{"label": "", "value": ""}]

    #read csv file
    with open(csvFilePath, encoding='utf-8') as csvf:
        #load csv file data using csv library's dictionary reader
        csvReader = csv.DictReader(csvf)

        i = 1
        #convert each csv row into python dict
        for row in csvReader:
            # reformat values - pad it with index, so that we could use it as a key for dropdown list
            row["value"]=f'{i:03}_{row["value"]}'
            jsonArray.append(row)
            i += 1
  
    #convert python jsonArray to JSON String and write to file
    with open(jsonFilePath, 'w', encoding='utf-8') as jsonf:
        jsonString = json.dumps(jsonArray, indent=2)
        jsonf.write(jsonString)
          
csvFilePath = r'./data/zones.csv'
jsonFilePath = r'./data/tz.json'
csv_to_json(csvFilePath, jsonFilePath)
