import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from omnetpp.scave import results, chart, utils

relPath = "../results/"
cliMeasNames = ["measureReceivedAmount", "readSent", "commandSent", "readResponse", 
             "commandResponse", "readResponseTimeout", "commandResponseTimeout"]
evilMeasNames = ["measureBlock", "measureCompromised", "readRequestBlock", "readRequestCompromised", 
                 "commandRequestBlock", "commandRequestCompromised", "readResponseBlock", "readResponseCompromised",
                 "commandResponseBlock", "commandResponseCompromised"]

rep = 10
fileNames = []

name = "MMS_MITM_Wired_Lan_Multi_Server"
for i in range(1,3):
    fileNames.append(relPath + name + '-#' + i.__str__() + '.sca')

print(fileNames)

## This fills up the RAM
data = results.read_result_files(fileNames)
scl = results.get_scalars(data)
