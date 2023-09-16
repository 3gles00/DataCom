import csv
import statistics
from collections import Counter

print("Input the file to be counted with")
f = str(input())

with open(f) as csvFile:
    #sets starting position to the beginning of the csv file
    csvFile.seek(0)
    csvReader = csv.reader(csvFile, delimiter=',')
    incColumns = [0, 3] #the coulmns we want
    lineCount = 0
    parsedList = [] #number of atemnpts

    for row in csvReader:
        content  = list(row[i] for i in incColumns)
        if(content[0] == '0'):
            #print(content)
            lineCount += 1
            parsedList.append(int(content[1]))

    count = Counter(parsedList)
    countValues = list(count.values())

    #print("Values: " + countValues)
    print("Standard deviation of number of attempts: ")
    print(statistics.stdev(countValues))