from datetime import datetime

from matplotlib.axes import Axes
from matplotlib.figure import Figure
import numpy as np
from pandas import read_csv
import pandas as pd
from matplotlib import pyplot as plt
import matplotlib.ticker as ticker

column_names = ['datestamp', 'date', 'time', 'weight', 'current', 'light', 'proximity']
df = read_csv('../output/datr20191206.csv', names=column_names, parse_dates=True, infer_datetime_format=True)

df.reset_index(drop=True, inplace=True)
print(df)
df['datetime'] = pd.to_datetime(df['datestamp'])
df['index'] = df['datetime']
df.set_index('index', inplace=True)
del df['datestamp']
del df['time']
del df['date']

small_window = {'name': 'rolling4', 'size': 4}
large_window = {'name': 'rolling36', 'size': 36}
events = 'events'

df[small_window['name']] = df['weight'].rolling(small_window['size']).median()
df[large_window['name']] = df['weight'].rolling(large_window['size']).median()

print(df)
# produce a series, based on the results of the window median filter,
df['diff'] = df[small_window['name']].diff(periods=-8)


threshold = 300.0
df['thresholded'] = (df['diff'] > threshold)
df['highlight'] = (df['thresholded'] == True) & (df['thresholded'].shift(1) == False)

print(df.info())


